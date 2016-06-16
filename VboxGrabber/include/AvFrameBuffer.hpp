#ifndef AVFRAMEBUFFER_HPP
#define AVFRAMEBUFFER_HPP

#include <stdio.h>
#include <stdlib.h>
#include <map>

extern "C"{

//#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/samplefmt.h>
#include <libswscale/swscale.h>

}

#include <IFramebufferPlus.hpp>

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

struct FrameImage{
	uint32_t x,y,width,height,imageSize;
	uint8_t* data;
};

class AvQueue
{

public:

	FrameImage pop()
	{
		std::unique_lock<std::mutex> mlock(_mutex);
		while(_queue.empty())
			_cond.wait(mlock); //Release the lock to other thread(s), returns when notified the condition has been met.

		FrameImage fi = _queue.front();
		_queue.pop();
		return fi;
	}
	
	void push(const FrameImage& fi)
	{
		std::unique_lock<std::mutex> mlock(_mutex);
		_queue.push(fi);
		mlock.unlock();
		_cond.notify_one(); //notify the condition has been met.
	}
	
private:

	std::queue<FrameImage> _queue;
	std::mutex _mutex;
	std::condition_variable _cond;
	
};

class AvFrameBuffer : public IFramebufferPlus
{
public:
	NS_DECL_ISUPPORTS
	NS_DECL_IFRAMEBUFFER
	
    AvFrameBuffer(uint32_t dstWidth, uint32_t dstHeight,
				    AVPixelFormat dstPixelFormat = AV_PIX_FMT_BGR32);

    uint32_t fetch(uint8_t** data);
	
private:
	
    ~AvFrameBuffer();

protected: 

	void updateCtx(uint32_t width, uint32_t height);
	void writeSrc();

	const uint32_t _dstWidth, _dstHeight;
    uint32_t _srcWidth, _srcHeight;
	PRUint32 _bitsPerPixel;
	PRUint32 _capabilities;

	std::map<PRUint32, AVPixelFormat> _vbox2ffmpeg;

	AVPixelFormat _dstPixelFormat;
	AVPixelFormat _srcPixelFormat;
	AVFrame* _srcFrame;
	AVFrame* _dstFrame;
	struct SwsContext *_swsCtx;
	uint8_t* _src;

    size_t _frameSize;
    uint8_t* _frameData;

	uint32_t _count;

	AvQueue _avq;
	std::thread _avThread;
	
};

#endif //AVFRAMEBUFFER_HPP
