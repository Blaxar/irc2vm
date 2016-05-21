#include <VboxGrabber.hpp>

VboxGrabber::VboxGrabber(std::string vmName, PRUint32 format): _format(format)
{

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

	}

}

PRUint32 VboxGrabber::grab(uint8_t** data, uint16_t width, uint16_t height, uint8 screenID)
{
	PRUint32 size;
	_display->TakeScreenShotToArray(0, width, height, _format, &size, data);
    _eventQ->ProcessPendingEvents();

	return size;
}

VboxGrabber::~VboxGrabber()
{
	
	PRUint32 state; _session->GetState(&state);
	if(state == SessionState_Locked) _session->UnlockMachine();
	if(_machine) _machine->Release();
}
