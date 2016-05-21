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

class VboxGrabber
{

    public:
	
    VboxGrabber(std::string vmName, PRUint32 format=BitmapFormat_RGBA);
    ~VboxGrabber();

    PRUint32 grab(uint8_t** data, uint16_t width, uint16_t height, uint8 screenID = 0);

    protected:

	PRUint32 _format;
	
	nsCOMPtr<nsIServiceManager> _serviceManager;
	nsCOMPtr<nsIEventQueue> _eventQ;
	nsCOMPtr<nsIComponentManager> _manager;
	nsCOMPtr<IVirtualBox> _virtualBox;
    IMachine* _machine;
    nsCOMPtr<ISession> _session;
    IConsole* _console;
	
	IDisplay* _display;
	
};

#endif //VBOXGRABBER_HPP
