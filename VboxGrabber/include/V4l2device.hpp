#ifndef V4L2DEVICE_HPP
#define V4L2DEVICE_HPP

#include <stdio.h>
#include <stdlib.h>

#include <string>

extern "C"{

#include <sys/types.h>
#include <linux/videodev2.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

static int c_open(const char* filename, int oflag, mode_t mode){return open(filename, oflag, mode);}
static int c_close(int fd){return close(fd);}
static ssize_t c_write(int fd, void *buf, size_t count){return write(fd, buf, count);}

#define CLEAR(x) memset(&(x), 0, sizeof(x))
#define errno_throw(s) throw V4l2deviceException(std::string(s)+" error "+std::to_string(errno)+", "+strerror(errno))

}

class V4l2deviceException : public std::exception {
  private:
    std::string err_msg;

  public:
    V4l2deviceException(const char *msg) : err_msg(msg) {};
	V4l2deviceException(const std::string msg) : err_msg(msg) {};
    ~V4l2deviceException() throw() {};
    const char *what() const throw() { return this->err_msg.c_str(); };
};

class V4l2device
{

    public:
	
    V4l2device(std::string dev, uint32_t width, uint32_t height):
		_dev(dev), _width(width), _height(height), force_format(true) {}
	
    virtual ~V4l2device(){}

    void close(void)
	{

		stop_streaming();
		uninit_device();

		if (-1 == c_close(fd))
               errno_throw("close");

        fd = -1;
		
	}

    void open(void)
	{

		const char* dev_name = _dev.c_str();

        struct stat st;

        if (-1 == stat(dev_name, &st))
                errno_throw(std::string("Cannot identify ")+dev_name);

        if (!S_ISCHR(st.st_mode))
				throw  V4l2deviceException(std::string(dev_name)+" is no device");

        fd = c_open(dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);

        if (-1 == fd)
			errno_throw(std::string("Cannot open ")+dev_name);
		
		init_device();
        start_streaming();
		
	}
    
	virtual void write(const void* data, ssize_t size) = 0;

    protected:

	std::string _dev;
	const uint32_t _width, _height;

    struct buffer {
        void   *start;
        size_t  length;
    };

    int              fd = -1;
    buffer          *buffers;
    unsigned int     n_buffers;
    int              out_buf;
    int              force_format;

    int xioctl(int fh, int request, void *arg)
	{
        int r;

        do {
                r = ioctl(fh, request, arg);
        } while (-1 == r && EINTR == errno);

        return r;
	}

    virtual void uninit_device(void) = 0;

    virtual void init_write(unsigned int buffer_size) = 0;

    virtual void init_device(void) = 0;

	virtual void start_streaming(void) = 0;

	virtual void stop_streaming(void) = 0;

	virtual int get_buffer_size(void) = 0;
	
};

#endif //V4L2DEVICE_HPP
