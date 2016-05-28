#ifndef V4L2FRAMEBUFFER_HPP
#define V4L2FRAMEBUFFER_HPP

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

class V4l2FrameBuffer : public IFramebufferPlus
{
public:
	NS_DECL_ISUPPORTS
	NS_DECL_IFRAMEBUFFER
	
    V4l2FrameBuffer(uint32_t dstWidth, uint32_t dstHeight,
				    AVPixelFormat dstPixelFormat = AV_PIX_FMT_RGB32);

    uint32_t fetch(uint8_t** data);
	
private:
	
    ~V4l2FrameBuffer();

protected:

	void updateCtx(uint32_t width, uint32_t height);

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
	
};

#endif //V4L2FRAMEBUFFER_HPP
