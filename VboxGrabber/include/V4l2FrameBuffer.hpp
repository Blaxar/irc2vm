#ifndef V4L2FRAMEBUFFER_HPP
#define V4L2FRAMEBUFFER_HPP

#include <stdio.h>
#include <stdlib.h>
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

    V4l2FrameBuffer(PRUint32 pixelFormat = BitmapFormat_RGBA);

private:
    ~V4l2FrameBuffer();

protected:
    uint32_t _width, _height;
	PRUint32 _bitsPerPixel;
	PRUint32 _pixelFormat;
	PRUint32 _capabilities;
};

#endif //V4L2FRAMEBUFFER_HPP
