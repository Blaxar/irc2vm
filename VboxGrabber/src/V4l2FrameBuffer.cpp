#include <V4l2FrameBuffer.hpp>
#include <iostream>

NS_IMPL_ISUPPORTS1(V4l2FrameBuffer, IFramebuffer)

V4l2FrameBuffer::V4l2FrameBuffer(PRUint32 pixelFormat): _pixelFormat(pixelFormat), _capabilities(FramebufferCapabilities_UpdateImage)
{
  /* member initializers and constructor code */
}

V4l2FrameBuffer::~V4l2FrameBuffer()
{
  /* destructor code */
}

/* readonly attribute unsigned long width; */
NS_IMETHODIMP V4l2FrameBuffer::GetWidth(PRUint32 *aWidth)
{
	*aWidth = _width;
    return NS_OK;
}

/* readonly attribute unsigned long height; */
NS_IMETHODIMP V4l2FrameBuffer::GetHeight(PRUint32 *aHeight)
{
	*aHeight = _height;
    return NS_OK;
}

/* readonly attribute unsigned long bitsPerPixel; */
NS_IMETHODIMP V4l2FrameBuffer::GetBitsPerPixel(PRUint32 *aBitsPerPixel)
{
	*aBitsPerPixel = _bitsPerPixel;
    return NS_OK;
}

/* readonly attribute unsigned long bytesPerLine; */
NS_IMETHODIMP V4l2FrameBuffer::GetBytesPerLine(PRUint32 *aBytesPerLine)
{
	*aBytesPerLine = (_width*_bitsPerPixel)/8;
    return NS_OK;
}

/* readonly attribute PRUint32 pixelFormat; */
NS_IMETHODIMP V4l2FrameBuffer::GetPixelFormat(PRUint32 *aPixelFormat)
{
	*aPixelFormat = _pixelFormat;
    return NS_OK;
}

/* readonly attribute unsigned long heightReduction; */
NS_IMETHODIMP V4l2FrameBuffer::GetHeightReduction(PRUint32 *aHeightReduction)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute IFramebufferOverlay overlay; */
NS_IMETHODIMP V4l2FrameBuffer::GetOverlay(IFramebufferOverlay **aOverlay)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute long long winId; */
NS_IMETHODIMP V4l2FrameBuffer::GetWinId(PRInt64 *aWinId)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void getCapabilities (out unsigned long capabilitiesSize, [array, size_is (capabilitiesSize), retval] out PRUint32 capabilities); */
NS_IMETHODIMP V4l2FrameBuffer::GetCapabilities(PRUint32 *capabilitiesSize, PRUint32 **capabilities)
{
	*capabilities = new PRUint32;
	**capabilities = _capabilities;
	*capabilitiesSize = sizeof(PRUint32);
    return NS_OK;
}

/* void notifyUpdate (in unsigned long x, in unsigned long y, in unsigned long width, in unsigned long height); */
NS_IMETHODIMP V4l2FrameBuffer::NotifyUpdate(PRUint32 x, PRUint32 y, PRUint32 width, PRUint32 height)
{
    return NS_ERROR_NOT_AVAILABLE;
}

/* void notifyUpdateImage (in unsigned long x, in unsigned long y, in unsigned long width, in unsigned long height, in unsigned long imageSize, [array, size_is (imageSize)] in octet image); */
NS_IMETHODIMP V4l2FrameBuffer::NotifyUpdateImage(PRUint32 x, PRUint32 y, PRUint32 width, PRUint32 height, PRUint32 imageSize, PRUint8 *image)
{
    static uint16_t _count = 0;
	std::cout << "Image(" << _count << "): "<< "x: " << x << " y: " << y << " width: " << width << " height: " << height << std::endl;
	_count++;
    return NS_OK;
}

/* void notifyChange (in unsigned long screenId, in unsigned long xOrigin, in unsigned long yOrigin, in unsigned long width, in unsigned long height); */
NS_IMETHODIMP V4l2FrameBuffer::NotifyChange(PRUint32 screenId, PRUint32 xOrigin, PRUint32 yOrigin, PRUint32 width, PRUint32 height)
{
    std::cout << "Change: "<< "xOrigin: " << xOrigin << " yOrigin: " << yOrigin << " width: " << width << " height: " << height << std::endl;
    return NS_OK;
}

/* void videoModeSupported (in unsigned long width, in unsigned long height, in unsigned long bpp, [retval] out boolean supported); */
NS_IMETHODIMP V4l2FrameBuffer::VideoModeSupported(PRUint32 width, PRUint32 height, PRUint32 bpp, PRBool *supported)
{
	*supported = 1;
    return NS_OK;
}

/* [noscript] void getVisibleRegion (in octetPtr rectangles, in unsigned long count, [retval] out unsigned long countCopied); */
NS_IMETHODIMP V4l2FrameBuffer::GetVisibleRegion(PRUint8 * rectangles, PRUint32 count, PRUint32 *countCopied)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* [noscript] void setVisibleRegion (in octetPtr rectangles, in unsigned long count); */
NS_IMETHODIMP V4l2FrameBuffer::SetVisibleRegion(PRUint8 * rectangles, PRUint32 count)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* [noscript] void processVHWACommand (in octetPtr command); */
NS_IMETHODIMP V4l2FrameBuffer::ProcessVHWACommand(PRUint8 * command)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void notify3DEvent (in unsigned long type, in unsigned long dataSize, [array, size_is (dataSize)] in octet data); */
NS_IMETHODIMP V4l2FrameBuffer::Notify3DEvent(PRUint32 type, PRUint32 dataSize, PRUint8 *data)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
