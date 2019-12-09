#define WIN32_LEAN_AND_MEAN
#define STRICT

#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <tlhelp32.h>
#include <Dbghelp.h>
#include <stdio.h>

// ImageDirectoryEntryToData
#pragma comment(lib, "Dbghelp.lib")

// ひとつのモジュールに対してAPIフックを行う関数
void ReplaceIATEntryInOneMod(PCSTR pszModuleName, PROC pfnCurrent, PROC pfnNew, HMODULE hmodCaller) 
{
    ULONG ulSize;
    PIMAGE_IMPORT_DESCRIPTOR pImportDesc;
    pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)ImageDirectoryEntryToData(hmodCaller, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &ulSize);

    if (pImportDesc == NULL)
        return;

    while(pImportDesc->Name) {
        PSTR pszModName = (PSTR) ((PBYTE) hmodCaller + pImportDesc->Name);
        if (lstrcmpiA(pszModName, pszModuleName) == 0) 
            break;
        pImportDesc++;
    }

    if (pImportDesc->Name == 0)
        return;

    PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA) 
        ((PBYTE) hmodCaller + pImportDesc->FirstThunk);

    while(pThunk->u1.Function) {
        PROC *ppfn = (PROC*) &pThunk->u1.Function;
        BOOL fFound = (*ppfn == pfnCurrent);
        if (fFound) {
            DWORD dwDummy;
            VirtualProtect(ppfn, sizeof(ppfn), PAGE_EXECUTE_READWRITE, &dwDummy);
            WriteProcessMemory(GetCurrentProcess(), ppfn, &pfnNew, sizeof(pfnNew), NULL);
            return;
        }
        pThunk++;
    }
    return;
}

// すべてのモジュールに対してAPIフックを行う関数
void ReplaceIATEntryInAllMods(PCSTR pszModuleName, PROC pfnCurrent, PROC pfnNew) 
{
    HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
    if(hModuleSnap == INVALID_HANDLE_VALUE)
        return;

    MODULEENTRY32 me;
    me.dwSize = sizeof(me);
    BOOL bModuleResult = Module32First(hModuleSnap, &me);

	// それぞれのモジュールに対してReplaceIATEntryInOneModを実行
    while(bModuleResult) {        
        ReplaceIATEntryInOneMod(pszModuleName, pfnCurrent, pfnNew, me.hModule);
        bModuleResult = Module32Next(hModuleSnap, &me);
    }
    CloseHandle(hModuleSnap);
}

// フックする関数のプロトタイプを定義
typedef int (WINAPI *PFNMESSAGEBOXA)(HWND, PCSTR, PCSTR, UINT);
typedef int (WINAPI *PFNMESSAGEBOXW)(HWND, PCWSTR, PCWSTR, UINT);

int WINAPI Hook_MessageBoxA(HWND hWnd, PCSTR pszText, PCSTR pszCaption, UINT uType)
{
    // オリジナルMessageBoxAを呼び出す
    PROC pfnOrig = GetProcAddress(GetModuleHandleA("user32.dll"), "MessageBoxA");
    int nResult = ((PFNMESSAGEBOXA) pfnOrig)(hWnd, pszText, "Hook_MessageBoxA", uType);

    return nResult;
}

int WINAPI Hook_MessageBoxW(HWND hWnd, PCWSTR pszText, PCWSTR pszCaption, UINT uType)
{
	FILE *fout;

	_wfopen_s(&fout, L"C:\\MessageBoxW.txt", L"a");
	fwprintf(fout, L"MessageBoxW(%p, %s, %s, %d)\n", hWnd, pszText, pszCaption, uType);
	fclose(fout);

	// オリジナルMessageBoxWを呼び出す
    PROC pfnOrig = GetProcAddress(GetModuleHandleA("user32.dll"), "MessageBoxW");

	BYTE *code;
	code = (BYTE *)VirtualAlloc(NULL, 10, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	*code = 0x8B;
	*(code+1) = 0xFF;
	*(code+2) = 0x55;
	*(code+3) = 0x8B;
	*(code+4) = 0xEC;
	*(code+5) = 0xE9;

	DWORD address = (DWORD)pfnOrig - (DWORD)code - 10 + 5;
	memmove(code+6, (LPVOID)&address, sizeof(DWORD));

    int nResult = ((PFNMESSAGEBOXW) code)(hWnd, pszText, L"Hook_MessageBoxW", uType);

    return nResult;
}

void JmpMessageBox(PROC ppfn, PROC hookfn)
{
	DWORD dwDummy;
	BYTE code[5] = {90};

	code[0] = 0xE9;
	DWORD address = (DWORD)hookfn - (DWORD)ppfn - 5;
	memmove(code+1, (LPVOID)&address, sizeof(DWORD));
	
	if(!VirtualProtect(ppfn, sizeof(code), PAGE_EXECUTE_READWRITE, &dwDummy)){
		return;
	}

	WriteProcessMemory(GetCurrentProcess(), ppfn, code, sizeof(code), NULL);
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	PROC pfnOrig;
	pfnOrig = ::GetProcAddress(GetModuleHandleA("user32.dll"), "MessageBoxW");

	// IAT 書き換え前の MessageBox の呼び出し
	MessageBox(NULL, _T("TestMessage"), _T("Info"), MB_OK);
/*
	// IAT 書き換え ( MessageBoxA )
    pfnOrig = ::GetProcAddress(GetModuleHandleA("user32.dll"), "MessageBoxA");
	ReplaceIATEntryInAllMods("user32.dll", pfnOrig, (PROC)Hook_MessageBoxA);

	// IAT 書き換え ( MessageBoxW )
    pfnOrig = ::GetProcAddress(GetModuleHandleA("user32.dll"), "MessageBoxW");
    ReplaceIATEntryInAllMods("user32.dll", pfnOrig, (PROC)Hook_MessageBoxW);
*/
	JmpMessageBox(pfnOrig, (PROC)Hook_MessageBoxW);

	// IAT 書き換え後の MessageBox の呼び出し
    MessageBox(NULL, _T("TestMessage1"), _T("Info1"), MB_OK);
	MessageBox(NULL, _T("TestMessage2"), _T("Info2"), MB_OK);
	MessageBox(NULL, _T("TestMessage3"), _T("Info3"), MB_OK);
	MessageBox(NULL, _T("TestMessage4"), _T("Info4"), MB_OK);
	MessageBox(NULL, _T("TestMessage5"), _T("Info5"), MB_OK);

    return 0;
}