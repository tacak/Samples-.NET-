#include <windows.h>
#include "KeyHook.h"

#pragma data_seg("MY_DATA")
HHOOK hMyHook = 0;
#pragma data_seg()

HINSTANCE hInst;

BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD dwReason, LPVOID lpReserved)
{
    switch (dwReason) {
        case DLL_PROCESS_ATTACH:
            hInst = hInstDLL;
            break;
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}

int SetHook()
{
    hMyHook = SetWindowsHookEx(WH_KEYBOARD_LL, MyHookProc, hInst, 0);
	if (hMyHook == NULL){
		return 1;
	}

	return 0;
}

int ResetHook()
{
	if (UnhookWindowsHookEx(hMyHook) == 0){
		return 1;
	}

	return 0;
}

LRESULT CALLBACK MyHookProc(int nCode, WPARAM wp, LPARAM lp)
{
    if (nCode < 0)
        return CallNextHookEx(hMyHook, nCode, wp, lp);

    return 0;
}