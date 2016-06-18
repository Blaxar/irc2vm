#include <VboxGrabber.hpp>
#include <V4l2deviceMmap.hpp>
#include <AvFrameBuffer.hpp>

extern "C"{

#include <assert.h>
#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
	
}

#include <iostream>

using namespace std;

VboxGrabber::VboxGrabber(std::string vmName, std::string dev, uint32_t width, uint32_t height, uint8_t screenID, bool useFrameBuffer):
	_screenID(screenID), _dev(dev), _width(width), _height(height), _format(BitmapFormat_BGRA),
	_srcData(NULL), _frameBuffer(NULL)//_frameBuffer(new AvFrameBuffer(width, height))
{

	if(useFrameBuffer) _frameBuffer = new AvFrameBuffer(width, height);
	
	//V4l2 device
	_v4l2device = std::make_unique<V4l2deviceMmap>(_dev, width, height);
	_v4l2device->open();
	
	//Virtual Machine handling
	nsresult rc;
	
	if (sizeof(PRUnichar) != sizeof(wchar_t))
        throw VboxGrabberException("Error: sizeof(PRUnichar) {"+to_string((unsigned long) sizeof(PRUnichar))+
								   "} != sizeof(wchar_t) {"+to_string((unsigned long) sizeof(wchar_t))+"}!\n"
								   "Probably, you forgot the -fshort-wchar compiler option.\n");


    rc = NS_InitXPCOM2(getter_AddRefs(_serviceManager), nsnull, nsnull);
    if (NS_FAILED(rc))
		throw VboxGrabberException("Error: XPCOM could not be initialized! rc="+to_string(rc));

    rc = NS_GetMainEventQ(getter_AddRefs(_eventQ));
    if (NS_FAILED(rc))
        throw VboxGrabberException("Error: could not get main event queue! rc="+to_string(rc));

        /*
         * Now XPCOM is ready and we can start to do real work.
         * IVirtualBox is the root interface of VirtualBox and will be
         * retrieved from the XPCOM component manager. We use the
         * XPCOM provided smart pointer nsCOMPtr for all objects because
         * that's very convenient and removes the need deal with reference
         * counting and freeing.
         */
    rc = NS_GetComponentManager(getter_AddRefs(_manager));
    if (NS_FAILED(rc))
        throw VboxGrabberException("Error: could not get component manager! rc="+to_string(rc));

    rc = _manager->CreateInstanceByContractID(NS_VIRTUALBOX_CONTRACTID,
                                             nsnull,
                                             NS_GET_IID(IVirtualBox),
                                             getter_AddRefs(_virtualBox));
    if (NS_FAILED(rc))
        throw VboxGrabberException("Error, could not instantiate VirtualBox object! rc="+to_string(rc));


	nsAutoString nsVmName;
	nsVmName.AssignWithConversion(vmName.c_str(), vmName.length());
	
	rc = _virtualBox->FindMachine(nsVmName.get(), &_machine);
	
	if(_machine)
    {
		PRBool isAccessible = PR_FALSE;
		_machine->GetAccessible(&isAccessible);
					
		if (NS_FAILED(rc))
		    throw VboxGrabberException("Error: could not get component manager! rc="+to_string(rc));

		rc = _manager->CreateInstanceByContractID(NS_SESSION_CONTRACTID,
												 nsnull,
												 NS_GET_IID(ISession),
												 getter_AddRefs(_session));
		
		if (NS_FAILED(rc))
		    throw VboxGrabberException("Error, could not instantiate session object! rc="+to_string(rc));

		PRUint32 state;
		_machine->GetState(&state);

		if(state == MachineState_Running)
		{

           rc = _machine->LockMachine(_session, LockType_Shared);
		   if (NS_FAILED(rc))
		       throw VboxGrabberException("Error, could not lock the machine for the session! rc="+to_string(rc));
		
		}else
			throw VboxGrabberException("Error, this machine is not currently running! rc="+to_string(rc));

		_session->GetConsole(&_console);
		_console->GetDisplay(&_display);
		if(_frameBuffer != NULL) _display->AttachFramebuffer(_screenID, _frameBuffer, &_frameBufferID);
		//else throw VboxGrabberException("Failed to attach framebuffer (NULL).");
		//else _srcData = (uint8_t*) malloc(_width*_height*4);
			
	}

}

uint32_t VboxGrabber::grab()
{
	PRUint32 size;
	uint8_t* data = NULL;
    
	if(_frameBuffer != NULL) size = _frameBuffer->fetch(&data);
	else{
		size = _width*_height*4;
		_threadQueue.push(thread(&VboxGrabber::takeScreenshot, this));
		/*nsresult rc = _display->TakeScreenShotToArray(_screenID, _width, _height, _format, &size, &data);
		if(NS_FAILED(rc))
		{
			if(data != NULL) free(data);
		    return 0;
		} */
	}
	
    //_v4l2device->write((const void*) data, size);
	
    //free(data);
	_eventQ->ProcessPendingEvents();
	return size;
}

void VboxGrabber::takeScreenshot()
{
	PRUint32 size;
	uint8_t* data = NULL;
	_display->TakeScreenShotToArray(_screenID, _width, _height, _format, &size, &data);
	_v4l2device->write((const void*) data, size);
    free(data);
	if(_threadQueue.size() > 1){_threadQueue.front().join(); _threadQueue.pop();} //We join and pop one previous (done) thread, so we don't stack them in the queue
}

VboxGrabber::~VboxGrabber()
{

	PRUint32 state; _session->GetState(&state);
	if(_srcData != NULL) free(_srcData);
	if(state == SessionState_Locked) _session->UnlockMachine();
	if(_frameBuffer != NULL) _display->DetachFramebuffer(_screenID, _frameBufferID);
	if(_machine) _machine->Release();
	_v4l2device->close();

}
