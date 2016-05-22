#include <VboxGrabber.hpp>

extern "C"{
	
#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
	
}

VboxGrabber::VboxGrabber(std::string vmName, uint8_t screenID, IFramebuffer* frameBuffer, PRUint32 format):
	_screenID(screenID), _dev(dev), _frameBuffer(frameBuffer), _format(format)
{

	//Video output handling

    struct stat st;

    if (-1 == stat(_dev.c_str(), &st)) {
        fprintf(stderr, "Cannot identify '%s': %d, %s\n",
                _dev.c_str(), errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (!S_ISCHR(st.st_mode)) {
        fprintf(stderr, "%s is no device\n", _dev.c_str());
        exit(EXIT_FAILURE);
    }

    fd = c_open(_dev.c_str(), O_RDWR /* required */ | O_NONBLOCK, 0);

    if (-1 == fd) {
        fprintf(stderr, "Cannot open '%s': %d, %s\n",
                _dev.c_str(), errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

	struct v4l2_capability cap;
    struct v4l2_cropcap cropcap;
    struct v4l2_crop crop;
    struct v4l2_format fmt;
    unsigned int min;

    if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) {
        if (EINVAL == errno) {
            fprintf(stderr, "%s is no V4L2 device\n",
                    dev_name);
            exit(EXIT_FAILURE);
        } else {
            errno_exit("VIDIOC_QUERYCAP");
        }
    }
	
	//Virtual Machine handling
	nsresult rc;
	
	if (sizeof(PRUnichar) != sizeof(wchar_t))
    {
        printf("Error: sizeof(PRUnichar) {%lu} != sizeof(wchar_t) {%lu}!\n"
               "Probably, you forgot the -fshort-wchar compiler option.\n",
               (unsigned long) sizeof(PRUnichar),
               (unsigned long) sizeof(wchar_t));
        return;
    }

    rc = NS_InitXPCOM2(getter_AddRefs(_serviceManager), nsnull, nsnull);
    if (NS_FAILED(rc))
    {
        printf("Error: XPCOM could not be initialized! rc=%#x\n", rc);
        return;
    }

    rc = NS_GetMainEventQ(getter_AddRefs(_eventQ));
    if (NS_FAILED(rc))
    {
        printf("Error: could not get main event queue! rc=%#x\n", rc);
        return;
    }

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
    {
        printf("Error: could not get component manager! rc=%#x\n", rc);
        return;
    }

    rc = _manager->CreateInstanceByContractID(NS_VIRTUALBOX_CONTRACTID,
                                             nsnull,
                                             NS_GET_IID(IVirtualBox),
                                             getter_AddRefs(_virtualBox));
    if (NS_FAILED(rc))
    {
        printf("Error, could not instantiate VirtualBox object! rc=%#x\n", rc);
        return;
    }

	nsAutoString nsVmName;
	nsVmName.AssignWithConversion(vmName.c_str(), vmName.length());
	
	rc = _virtualBox->FindMachine(nsVmName.get(), &_machine);
	
	if(_machine)
    {
		PRBool isAccessible = PR_FALSE;
		_machine->GetAccessible(&isAccessible);
					
		if (NS_FAILED(rc))
		{
			printf("Error: could not get component manager! rc=%#x\n", rc);
			return;
		}
		rc = _manager->CreateInstanceByContractID(NS_SESSION_CONTRACTID,
												 nsnull,
												 NS_GET_IID(ISession),
												 getter_AddRefs(_session));
		
		if (NS_FAILED(rc))
		{
			printf("Error, could not instantiate session object! rc=%#x\n", rc);
			return;
		}

		//rc = _machine->LockMachine(_session, LockType_Shared);
		if (NS_FAILED(rc))
		{
			printf("Error, could not lock the machine for the session! rc=%#x\n", rc);
			return;
		}

		nsCOMPtr<IProgress> pProgress;
		rc = _machine->LaunchVMProcess(_session, NS_LITERAL_STRING("headless").get(),
									  NS_LITERAL_STRING("").get(),
									  getter_AddRefs(pProgress));

		if (NS_FAILED(rc))
			printf("Failed to launch machine! rc=%#x\n", rc);
		else
		{
			rc = pProgress->WaitForCompletion(-1);
			PRInt32 resultCode;
			pProgress->GetResultCode(&resultCode);
			if (NS_FAILED(rc) || NS_FAILED(resultCode))
				printf("Failed to launch machine! rc=%#x\n",
					   NS_FAILED(rc) ? rc : resultCode);
		}

		_session->GetConsole(&_console);
		_console->GetDisplay(&_display);
		if(_frameBuffer != NULL) _display->AttachFramebuffer(_screenID, _frameBuffer, &_frameBufferID);

	}

}

uint32_t VboxGrabber::grab(uint8_t** data, uint16_t width, uint16_t height)
{
	PRUint32 size;
	_display->TakeScreenShotToArray(_screenID, width, height, _format, &size, data);
    _eventQ->ProcessPendingEvents();

	return size;
}

VboxGrabber::~VboxGrabber()
{
	
	PRUint32 state; _session->GetState(&state);
	if(state == SessionState_Locked) _session->UnlockMachine();
	if(_machine) _machine->Release();
}
