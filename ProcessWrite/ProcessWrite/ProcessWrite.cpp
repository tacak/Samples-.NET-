#include "resource.h"

#include <windows.h>
#include <windowsx.h>
#include <tchar.h>

///////////////////////////////////////////////////////////////////////////////
void OnCommand(HWND hWnd, int nID, HWND hWndCtl, UINT codeNotify)
{
	TCHAR tText[100];
	DWORD dPid, dAddress;
	HANDLE hProcess;
	byte bData[] = {0x00, 0x00, 0x00, 0x00};

	switch(nID) {
	case IDC_BUTTON1:
		GetWindowText(GetDlgItem(hWnd, IDC_EDIT1), tText, 100);
		dPid = _tcstoul(tText, NULL, 10);

		GetWindowText(GetDlgItem(hWnd, IDC_EDIT2), tText, 100);
		dAddress = _tcstoul(tText, NULL, 16);

		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dPid);
		if(!WriteProcessMemory(hProcess, (LPVOID)dAddress, (LPVOID)bData, 4, NULL)){
			MessageBox(hWnd, _T("ílÇÃèëÇ´çûÇ›Ç…é∏îsÇµÇ‹ÇµÇΩ"), _T("Error"), MB_OK);
		}
		CloseHandle(hProcess);
		break;

	case IDCANCEL:
		EndDialog(hWnd, 0);
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg) {
		HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
		return TRUE;
	}

	return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	::DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, MainWndProc);

	return 0;
}
