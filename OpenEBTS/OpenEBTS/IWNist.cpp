#include "Includes.h"


#ifdef WIN32

BOOL APIENTRY DllMain( HANDLE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
			FreeImage_Initialise();
			FreeImage_SetOutputMessage(FreeImageErrorHandler);
			break;
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			FreeImage_DeInitialise();
			break;
    }

    return TRUE;
}

#else

void __attribute ((constructor)) init_function()
{
	FreeImage_Initialise();
	// Specify error trap function to FreeImage
	FreeImage_SetOutputMessage(FreeImageErrorHandler);
}

void __attribute ((destructor)) fini_function()
{
	FreeImage_DeInitialise();
}

#endif