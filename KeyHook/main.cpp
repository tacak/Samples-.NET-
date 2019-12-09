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
    hMyHook = SetWindowsHookEx(WH_KEYBOARD, MyHookProc, hInst, 0);
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

	int *pnt;
	HANDLE hMapFile;

	hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "CountObject");
	pnt = (int *) MapViewOfFile(hMapFile , FILE_MAP_WRITE , 0 , 0 , 0);

	switch(wp){
	case VK_F2:
		*pnt = 240;
		break;
	case VK_F3:
		*(pnt+1) = 240;
		break;
	case VK_F4:
		*(pnt+5) = 60;
		break;
	case VK_F5:
		*(pnt+4) = 180;
		break;
	case VK_F6:
		*(pnt+3) = 90;
		break;
	case VK_F7:
		*(pnt+2) = 90;
		break;
	}
    return CallNextHookEx(hMyHook, nCode, wp, lp);
}