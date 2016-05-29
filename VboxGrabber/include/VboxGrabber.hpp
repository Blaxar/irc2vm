#ifndef VBOXGRABBER_HPP
#define VBOXGRABBER_HPP

#include <stdio.h>
#include <stdlib.h>
#include <iconv.h>

/*
 * Include the XPCOM headers
 */
#include <nsMemory.h>
#include <nsString.h>
#include <nsIServiceManager.h>
#include <nsEventQueueUtils.h>

#include <nsIExceptionService.h>

#include <string>

/*
 * VirtualBox XPCOM interface. This header is generated
 * from IDL which in turn is generated from a custom XML format.
 */
#include <VirtualBox_XPCOM.h>
#include <IFramebufferPlus.hpp>

#include <V4l2device.hpp>

class VboxGrabberException : public std::exception {
  private:
    std::string err_msg;

  public:
    VboxGrabberException(const char *msg) : err_msg(msg) {};
	VboxGrabberException(const std::string msg) : err_msg(msg) {};
    ~VboxGrabberException() throw() {};
    const char *what() const throw() { return this->err_msg.c_str(); };
};

class VboxGrabber
{

    public:
	
    VboxGrabber(std::string vmName, std::string dev, uint32_t width, uint32_t height,
			    IFramebufferPlus* frameBuffer = NULL, uint8_t screenID = 0,
				PRUint32 format=BitmapFormat_RGBA);
	
    ~VboxGrabber();

    uint32_t grab();

    protected:

	std::string _dev;

	PRUint32 _format;
	uint8_t _screenID;
	const uint32_t _width, _height;
	
	nsCOMPtr<nsIServiceManager> _serviceManager;
	nsCOMPtr<nsIEventQueue> _eventQ;
	nsCOMPtr<nsIComponentManager> _manager;
	nsCOMPtr<IVirtualBox> _virtualBox;
    IMachine* _machine;
    nsCOMPtr<ISession> _session;
    IConsole* _console;
	
	IDisplay* _display;
    IFramebufferPlus* _frameBuffer;
    PRUnichar* _frameBufferID;
	V4l2device* _v4l2device;

};

#endif //VBOXGRABBER_HPP
