#ifndef V4L2DEVICEMMAP_HPP
#define V4L2DEVICEMMAP_HPP

#include <V4l2device.hpp>

class V4l2deviceMmap: public V4l2device
{

    public:
	
    V4l2deviceMmap(std::string dev, uint32_t width, uint32_t height);
	
    ~V4l2deviceMmap();

    protected:

	void write(const void* data, ssize_t size);
	
    void write_image(const void *p, int size, void* data);

    int write_frame(const void* data);

    void uninit_device(void);

    void init_write(unsigned int buffer_size);

    void init_device(void);

    void start_streaming(void);

    void stop_streaming(void);

	int get_buffer_size(void);
	
};

#endif //V4L2DEVICEMMAP_HPP
