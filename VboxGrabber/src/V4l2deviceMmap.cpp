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

void V4l2deviceMmap::write_image(const void *p, int size, void* data)
{

}

int V4l2deviceMmap::write_frame(const void* data)
{
	return 0;
}

void V4l2deviceMmap::uninit_device(void)
{

}

void V4l2deviceMmap::init_write(unsigned int buffer_size)
{

}

void V4l2deviceMmap::init_device(void)
{

}

void V4l2deviceMmap::start_streaming(void)
{

}

void V4l2deviceMmap::stop_streaming(void)
{

}

int V4l2deviceMmap::get_buffer_size(void)
{
	return 0;
}
