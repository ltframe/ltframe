// dllmain.cpp : Defines the entry point for the DLL application.
#include <config.h>

#include <JavaScriptCore/InitializeThreading.h>
#include <JavaScriptCore/MainThread.h>
#include <WebCore/Page.h>
#include <WebCore/PageGroup.h>
#include <WebCore/IconDatabase.h>
#include <WebCore/WebCoreInstanceHandle.h>
#include <WebCore/RenderThemeWin.h>
#include <WebCore/ResourceHandleManager.h>
#include <WebCore/Console.h>
#include "IntRect.h"

static void enableDataExecutionPrevention()
{
	// Enable Data Execution prevention at runtime rather than via /NXCOMPAT
	// http://blogs.msdn.com/michael_howard/archive/2008/01/29/new-nx-apis-added-to-windows-vista-sp1-windows-xp-sp3-and-windows-server-2008.aspx

	const DWORD enableDEP = 0x00000001;

	HMODULE hMod = ::GetModuleHandleW(L"Kernel32.dll");
	if (!hMod)
		return;

	typedef BOOL (WINAPI *PSETDEP)(DWORD);

	PSETDEP procSet = reinterpret_cast<PSETDEP>(::GetProcAddress(hMod, "SetProcessDEPPolicy"));
	if (!procSet)
		return;

	// Enable Data Execution Prevention, but allow ATL thunks (for compatibility with the version of ATL that ships with the Platform SDK).
	procSet(enableDEP);
}

static void enableTerminationOnHeapCorruption()
{
	// Enable termination on heap corruption on OSes that support it (Vista and XPSP3).
	// http://msdn.microsoft.com/en-us/library/aa366705(VS.85).aspx

	const HEAP_INFORMATION_CLASS heapEnableTerminationOnCorruption = static_cast<HEAP_INFORMATION_CLASS>(1);

	HMODULE hMod = ::GetModuleHandleW(L"kernel32.dll");
	if (!hMod)
		return;

	typedef BOOL (WINAPI*HSI)(HANDLE, HEAP_INFORMATION_CLASS, PVOID, SIZE_T);
	HSI heapSetInformation = reinterpret_cast<HSI>(::GetProcAddress(hMod, "HeapSetInformation"));
	if (!heapSetInformation)
		return;

	heapSetInformation(0, heapEnableTerminationOnCorruption, 0, 0);
}

static void disableUserModeCallbackExceptionFilter()
{
	const DWORD PROCESS_CALLBACK_FILTER_ENABLED = 0x1;
	typedef BOOL (NTAPI *getProcessUserModeExceptionPolicyPtr)(LPDWORD lpFlags);
	typedef BOOL (NTAPI *setProcessUserModeExceptionPolicyPtr)(DWORD dwFlags);

	HMODULE lib = LoadLibrary(TEXT("kernel32.dll"));
	ASSERT(lib);

	getProcessUserModeExceptionPolicyPtr getPolicyPtr = (getProcessUserModeExceptionPolicyPtr)GetProcAddress(lib, "GetProcessUserModeExceptionPolicy");
	setProcessUserModeExceptionPolicyPtr setPolicyPtr = (setProcessUserModeExceptionPolicyPtr)GetProcAddress(lib, "SetProcessUserModeExceptionPolicy");

	DWORD dwFlags;
	if (!getPolicyPtr || !setPolicyPtr || !getPolicyPtr(&dwFlags)) {
		FreeLibrary(lib);
		return;
	}

	// If this flag isn't cleared, exceptions that are thrown when running in a 64-bit version of
	// Windows are ignored, possibly leaving Safari in an inconsistent state that could cause an 
	// unrelated exception to be thrown.
	// http://support.microsoft.com/kb/976038
	// http://blog.paulbetts.org/index.php/2010/07/20/the-case-of-the-disappearing-onload-exception-user-mode-callback-exceptions-in-x64/
	setPolicyPtr(dwFlags & ~PROCESS_CALLBACK_FILTER_ENABLED);

	FreeLibrary(lib);
}


extern void __CFInitialize(void);

void init_libs()
{
	_putenv("WEBKIT_IGNORE_SSL_ERRORS=1");
	 pthread_win32_process_attach_np ();
	__CFInitialize();
}

typedef void (__cdecl* _PVFV) ();
#pragma section(".CRT$XCG", long, read)
__declspec(allocate(".CRT$XCG")) _PVFV init_section[] = { init_libs };


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	
	enableDataExecutionPrevention();
	enableTerminationOnHeapCorruption();
	disableUserModeCallbackExceptionFilter();
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		{
			WebCore::setInstanceHandle(hModule);
		}
		break;

	case DLL_PROCESS_DETACH:
		WebCore::RenderThemeWin::setWebKitIsBeingUnloaded();
		pthread_win32_thread_detach_np ();
		break;

	case DLL_THREAD_ATTACH:
		pthread_win32_thread_attach_np ();
		break;

	case DLL_THREAD_DETACH:
		pthread_win32_thread_detach_np ();
		break;
	}
	return TRUE;
}

