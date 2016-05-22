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

/*
 * VirtualBox XPCOM interface. This header is generated
 * from IDL which in turn is generated from a custom XML format.
 */
#include <VirtualBox_XPCOM.h>

/* Header file */
class V4l2FrameBuffer : public IFramebuffer
{
public:
    NS_DECL_ISUPPORTS
    NS_DECL_IFRAMEBUFFER

    V4l2FrameBuffer(uint32_t dstWidth, uint32_t dstHeight,
				    AVPixelFormat dstPixelFormat = AV_PIX_FMT_RGB0);

private:
    ~V4l2FrameBuffer();

protected:

	void updateCtx(uint32_t width, uint32_t height);
	
    uint32_t _srcWidth, _srcHeight;
	uint32_t _dstWidth, _dstHeight;
	PRUint32 _bitsPerPixel;
	PRUint32 _capabilities;

	std::map<PRUint32, AVPixelFormat> _vbox2ffmpeg;

	AVPixelFormat _dstPixelFormat;
	AVPixelFormat _srcPixelFormat;
	AVFrame* _srcFrame;
	AVFrame* _dstFrame;
	struct SwsContext *_swsCtx;
	
};

#endif //V4L2FRAMEBUFFER_HPP
