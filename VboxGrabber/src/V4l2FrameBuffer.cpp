#include <V4l2FrameBuffer.hpp>
#include <iostream>

extern "C"{

#include <math.h>

#include <libavcodec/avcodec.h>

#include <libavutil/avutil.h>
#include <libavutil/opt.h>
#include <libavutil/avassert.h>
#include <libavutil/channel_layout.h>
#include <libavutil/mathematics.h>
#ifdef __linux__
#include <libavutil/timestamp.h>
#endif/*__linux__*/


#include <libavutil/common.h>
#include <libavutil/imgutils.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>

#include <assert.h>

}

NS_IMPL_ISUPPORTS1(V4l2FrameBuffer, IFramebuffer)

V4l2FrameBuffer::V4l2FrameBuffer(uint32_t dstWidth, uint32_t dstHeight, AVPixelFormat dstPixelFormat):
_dstWidth(dstWidth), _dstHeight(dstHeight), _srcWidth(0), _srcHeight(0),
_dstPixelFormat(dstPixelFormat), _srcFrame(NULL), _dstFrame(NULL), _swsCtx(NULL), _src(NULL),
	_capabilities(FramebufferCapabilities_UpdateImage), _count(0)
{

	_vbox2ffmpeg.insert(std::pair<PRUint32, AVPixelFormat>
						(BitmapFormat_RGBA, AV_PIX_FMT_RGB32));
	_vbox2ffmpeg.insert(std::pair<PRUint32, AVPixelFormat>
						(BitmapFormat_BGRA, AV_PIX_FMT_BGR32));
	_bitsPerPixel = 32;
    _dstFrame = av_frame_alloc();
    av_image_alloc(_dstFrame->data, _dstFrame->linesize,
                   _dstWidth, _dstHeight,
                   _dstPixelFormat, 16);

	_frameSize = _dstWidth*_dstHeight*4*sizeof(uint8_t);
	_frameData = (uint8_t*) malloc(_frameSize);
}

uint32_t V4l2FrameBuffer::fetch(uint8_t** data)
{
	
	*data = (uint8_t*)malloc(_frameSize);
	memcpy(*data, _frameData, _frameSize);
	/*
	for(int x=0; x<_dstWidth; x++)
		for(int y=0; y<_dstHeight; y++)
		{
			(*data)[(y*_dstWidth+x)*4+0]=(x/(float)_dstWidth)*255;
			(*data)[(y*_dstWidth+x)*4+1]=(y/(float)_dstWidth)*255;
		} */
	
	return _frameSize;
	
}

V4l2FrameBuffer::~V4l2FrameBuffer()
{
    if(_swsCtx != NULL) sws_freeContext(_swsCtx);
    if(_srcFrame != NULL) av_frame_free(&_srcFrame);
    if(_dstFrame != NULL) av_frame_free(&_dstFrame);
}

/* readonly attribute unsigned long width; */
NS_IMETHODIMP V4l2FrameBuffer::GetWidth(PRUint32 *aWidth)
{
	*aWidth = _srcWidth;
    return NS_OK;
}

/* readonly attribute unsigned long height; */
NS_IMETHODIMP V4l2FrameBuffer::GetHeight(PRUint32 *aHeight)
{
	*aHeight = _srcHeight;
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
	*aBytesPerLine = (_srcWidth*_bitsPerPixel)/8;
    return NS_OK;
}

/* readonly attribute PRUint32 pixelFormat; */
NS_IMETHODIMP V4l2FrameBuffer::GetPixelFormat(PRUint32 *aPixelFormat)
{
	*aPixelFormat = BitmapFormat_BGRA;
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

	if(_swsCtx != NULL){
		
		if(_srcWidth == width && _srcHeight == height)
			memcpy(_src, image, imageSize);
        else
			for(int r=0; r<height; r++)
				memcpy(&_src[((y+r)*_srcWidth+x)*4], &image[r*width*4], width*4);
		
		av_image_fill_arrays(_srcFrame->data, _srcFrame->linesize, _src, _srcPixelFormat, _srcWidth, _srcHeight, 16);
		
		sws_scale(_swsCtx, (const uint8_t * const*)_srcFrame->data, _srcFrame->linesize,
				  0, _srcHeight, _dstFrame->data, _dstFrame->linesize);

		av_image_copy_to_buffer(_frameData, _frameSize, _dstFrame->data, _dstFrame->linesize, _dstPixelFormat, _dstWidth, _dstHeight, 16);
	}
	_count++;
    return NS_OK;
}

/* void notifyChange (in unsigned long screenId, in unsigned long xOrigin, in unsigned long yOrigin, in unsigned long width, in unsigned long height); */
NS_IMETHODIMP V4l2FrameBuffer::NotifyChange(PRUint32 screenId, PRUint32 xOrigin, PRUint32 yOrigin, PRUint32 width, PRUint32 height)
{

	updateCtx(width, height);
	
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

void V4l2FrameBuffer::updateCtx(uint32_t width, uint32_t height)
{

	if(width != _srcWidth || height != _srcHeight)
	{
		
		PRUint32 pxFmt;
		GetPixelFormat(&pxFmt);
		_srcPixelFormat = _vbox2ffmpeg[pxFmt];
		_srcWidth = width;
		_srcHeight = height;

		if(_src != NULL) free(_src);
		_src = (uint8_t*) malloc(_srcWidth*_srcHeight*4);
		
		if(_srcFrame != NULL) free(_srcFrame);
		_srcFrame = av_frame_alloc();
		av_image_alloc(_srcFrame->data, _srcFrame->linesize,
					   _srcWidth, _srcHeight,
					   _srcPixelFormat, 16);
		
		if(_swsCtx != NULL) sws_freeContext(_swsCtx);
		_swsCtx = sws_getContext(_srcWidth, _srcHeight, _srcPixelFormat,
								 _dstWidth, _dstHeight, _dstPixelFormat,
								 SWS_BILINEAR, NULL, NULL, NULL);
	}
	
}
