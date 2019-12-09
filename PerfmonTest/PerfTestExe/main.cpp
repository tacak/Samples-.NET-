#include "resource.h"

#include <windows.h>
#include <windowsx.h>
#include <tchar.h>


///////////////////////////////////////////////////////////////////////////////


LONG g_lInstanceCount = 0;


///////////////////////////////////////////////////////////////////////////////


HMODULE g_hCounterDLL;

typedef LONG (__stdcall *pfnGetInstanceCount)(void);
typedef LONG (__stdcall *pfnGetPerf1)(void);
typedef LONG (__stdcall *pfnChangeInstanceCount)(INT n);
typedef LONG (__stdcall *pfnChangePerf1)(INT n);

pfnGetInstanceCount GetInstanceCount;
pfnGetPerf1 GetPerf1;
pfnChangeInstanceCount ChangeInstanceCount;
pfnChangePerf1 ChangePerf1;


///////////////////////////////////////////////////////////////////////////////


BOOL OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM lParam) {

     // Load Counter DLL which exposes performance counters in the shared memory.

     g_hCounterDLL = LoadLibrary (TEXT("PerfTestDLL"));

     if (!g_hCounterDLL) {
          ::MessageBox (
               hWnd, 
               TEXT("Failed to load the performance dll."), 
               TEXT("Error"), 
               MB_OK | MB_ICONERROR);
          return FALSE;
     }
     
     GetInstanceCount = (pfnGetInstanceCount) GetProcAddress (g_hCounterDLL, "GetInstanceCount");
     GetPerf1 = (pfnGetPerf1) GetProcAddress (g_hCounterDLL, "GetPerf1");
     ChangeInstanceCount = (pfnChangeInstanceCount) GetProcAddress (g_hCounterDLL, "ChangeInstanceCount");
     ChangePerf1 = (pfnChangePerf1) GetProcAddress (g_hCounterDLL, "ChangePerf1");

     if (!GetInstanceCount || !GetPerf1 || !ChangeInstanceCount || !GetPerf1) {
          ::MessageBox (hWnd, TEXT("Failed to get one or more function addresses."), 
               TEXT("ERROR"), MB_OK | MB_ICONERROR);
          return FALSE;
     }

     TCHAR szInstanceNumber[32];
     
     g_lInstanceCount = ChangeInstanceCount (1);

     Static_SetText (GetDlgItem(hWnd, IDC_INSTANCE_NUMBER), _ltot(g_lInstanceCount, szInstanceNumber, 10));
     
     return TRUE;

}

///////////////////////////////////////////////////////////////////////////////


void OnCommand(HWND hWnd, int nID, HWND hWndCtl, UINT codeNotify) {

     switch(nID) {
     case IDC_PERF1_UP:

          ChangePerf1 (1);
          
          break;

     case IDC_PERF1_DOWN:

          ChangePerf1 (-1);
          
          break;
          
     case IDCANCEL:

          ChangeInstanceCount (-1);
          
          EndDialog(hWnd, 0);
          break;

     }
}


///////////////////////////////////////////////////////////////////////////////


BOOL CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

     switch(uMsg) {
          HANDLE_MSG(hWnd, WM_INITDIALOG, OnInitDialog);
          HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
          return TRUE;
     }

     return FALSE;
}


///////////////////////////////////////////////////////////////////////////////


int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow ) {

       ::DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAINWND), NULL, MainWndProc);

     return 0;
}
