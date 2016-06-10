#include <V4l2deviceMmap.hpp>

extern "C"{

#include <stdlib.h>
#include <assert.h>

#include <sys/time.h>
#include <sys/mman.h>

}
	
V4l2deviceMmap::V4l2deviceMmap(std::string dev, uint32_t width, uint32_t height):
V4l2device(dev, width, height)
{

}
	
V4l2deviceMmap::~V4l2deviceMmap()
{

}

void V4l2deviceMmap::write(const void* data, ssize_t size)
{
	static uint8_t c;
	struct v4l2_buffer buf;
    unsigned int i;

	CLEAR(buf);

    buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
    buf.memory = V4L2_MEMORY_MMAP;

	fd_set fds;
    struct timeval tv;
    int r;

    FD_ZERO(&fds);
    FD_SET(fd, &fds);

    /* Timeout. */
    tv.tv_sec = 0;
    tv.tv_usec = 10000;

    if(select(fd + 1, &fds, NULL, NULL, &tv) == -1)
		if (EINTR == errno)
            return;
        else errno_throw("select");
	
    if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
        switch (errno) {
            case EAGAIN:
                return;

            case EIO:
                                /* Could ignore EIO, see spec. */

                                /* fall through */

            default:
                errno_throw("VIDIOC_DQBUF");
        }
    }else{

		assert(buf.index < n_buffers);
		gettimeofday(&buf.timestamp, NULL);
		buf.bytesused = size;
		assert(buf.bytesused == _width*_height*4);
		//memset(buffers[buf.index].start,c++,size);
		memcpy(buffers[buf.index].start, data, size);

		if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
			errno_throw("VIDIOC_QBUF");
	}

    //process_image(buffers[buf.index].start, buf.bytesused, data);
	
}

void V4l2deviceMmap::uninit_device(void)
{

unsigned int i;

for (i = 0; i < n_buffers; ++i)
     if (-1 == munmap(buffers[i].start, buffers[i].length))
         errno_throw("munmap");

free(buffers);
	
}

void V4l2deviceMmap::init_write(unsigned int buffer_size)
{

}

void V4l2deviceMmap::init_device(void)
{

	struct v4l2_capability cap;
    struct v4l2_cropcap cropcap;
    struct v4l2_crop crop;
    struct v4l2_format fmt;
    unsigned int min;

    if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) {
        if (EINVAL == errno) {
            fprintf(stderr, "%s is no V4L2 device\n",
                    _dev.c_str());
            exit(EXIT_FAILURE);
        } else {
            errno_throw("VIDIOC_QUERYCAP");
        }
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_OUTPUT)) {
        fprintf(stderr, "%s is no video output device\n",
                _dev.c_str());
        exit(EXIT_FAILURE);
    }

    if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
        fprintf(stderr, "%s does not support streaming i/o\n",
                _dev.c_str());
        exit(EXIT_FAILURE);
    }

	CLEAR(fmt);

    fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
    if (force_format) {

        fmt.fmt.pix.width       = _width;
        fmt.fmt.pix.height      = _height;
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_BGR32;
        fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;

        if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt))
            errno_throw("VIDIOC_S_FMT");
		
        /* Note VIDIOC_S_FMT may change width and height. */
    } else {
		printf("Not forcing format.\n");
        /* Preserve original settings as set by v4l2-ctl for example */
        if (-1 == xioctl(fd, VIDIOC_G_FMT, &fmt))
            errno_throw("VIDIOC_G_FMT");
    }

	/* Buggy driver paranoia. */
        min = fmt.fmt.pix.width * 2;
        if (fmt.fmt.pix.bytesperline < min)
                fmt.fmt.pix.bytesperline = min;
        min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
        if (fmt.fmt.pix.sizeimage < min)
                fmt.fmt.pix.sizeimage = min;

	struct v4l2_requestbuffers req;

    CLEAR(req);

    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
    req.memory = V4L2_MEMORY_MMAP;

    if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
        if (EINVAL == errno) {
            fprintf(stderr, "%s does not support "
                    "memory mapping\n", _dev.c_str());
            exit(EXIT_FAILURE);
        } else {
            errno_throw("VIDIOC_REQBUFS");
        }
    }

    if (req.count < 2) {
        fprintf(stderr, "Insufficient buffer memory on %s\n",
                _dev.c_str());
        exit(EXIT_FAILURE);
    }

    buffers = (buffer*) calloc(req.count, sizeof(*buffers));

    if (!buffers) {
        fprintf(stderr, "Out of memory\n");
        exit(EXIT_FAILURE);
    }

    for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
        struct v4l2_buffer buf;
        CLEAR(buf);

        buf.type        = V4L2_BUF_TYPE_VIDEO_OUTPUT;
        buf.memory      = V4L2_MEMORY_MMAP;
        buf.index       = n_buffers;

        if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))
            errno_throw("VIDIOC_QUERYBUF");

        buffers[n_buffers].length = buf.length;
        buffers[n_buffers].start =
        mmap(NULL /* start anywhere */,
             buf.length,
             PROT_READ | PROT_WRITE /* required */,
             MAP_SHARED /* recommended */,
             fd, buf.m.offset);

        if (MAP_FAILED == buffers[n_buffers].start)
            errno_throw("mmap");
    }
	
}

void V4l2deviceMmap::start_streaming(void)
{

	unsigned int i;
    enum v4l2_buf_type type;

	for (i = 0; i < n_buffers; ++i) {
        struct v4l2_buffer buf;

        CLEAR(buf);
        buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
            errno_throw("VIDIOC_QBUF");
        }
        type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
        if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
            errno_throw("VIDIOC_STREAMON");
	
}

void V4l2deviceMmap::stop_streaming(void)
{

	enum v4l2_buf_type type;

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
         if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type))
             errno_throw("VIDIOC_STREAMOFF");
	
}

int V4l2deviceMmap::get_buffer_size(void)
{
	return 0;
}
