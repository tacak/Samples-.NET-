#define _WIN32_WINNT 0x500
#define _UNICODE
#define UNICODE

#include <stdio.h>
#include <locale.h>
#include <windows.h>
#include <tlhelp32.h>
#include <WInternl.h>
#include <atlconv.h>
#include "PEB.h"

typedef LONG (WINAPI NTQIP)(HANDLE, PROCESSINFOCLASS, PVOID, ULONG, PULONG);

VOID EnumProcs(VOID);
VOID CmdLineProcs(DWORD);
DWORD SetPrivilege( HANDLE &, TOKEN_PRIVILEGES &);

// プロセスIDおよびプロセス名の列挙を行う関数
VOID EnumProcs(VOID)
{
	HANDLE         hProcessSnap = NULL;
	PROCESSENTRY32 pe;

	// PROCESSENTRY32 構造体の初期化
	memset(&pe, 0, sizeof(PROCESSENTRY32));
    pe.dwSize = sizeof(PROCESSENTRY32);
	
	// プロセススナップショットの取得
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	// Process32First および Process32Next を用いて、プロセスIDおよびプロセス名を列挙する
	if(Process32First(hProcessSnap, &pe)){
		do{
			wprintf(L"%5u  %s\n", pe.th32ProcessID, pe.szExeFile);
		}while(Process32Next(hProcessSnap, &pe));
	}

}

// 指定されたプロセスIDを持つプロセスのコマンドライン引数文字列を取得する関数
VOID CmdLineProcs(DWORD dwPid)
{
    NTQIP *lpfnNtQueryInformationProcess;

	TCHAR *cCmdLine;
	DWORD dwPebAddress, dwSize;

	PROCESS_BASIC_INFORMATION pbi;
	FULL_PEB peb;
	RTL_USER_PROCESS_PARAMETERS ppe;

	// 指定されたプロセス ID に対応するプロセスを開く
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPid);
	if(hProcess == NULL){
		wprintf(L"プロセスID %d のプロセスのオープンに失敗しました\n", dwPid);
		return;
	}

	// Windows XP SP1 以前のバージョンは PEB のアドレスが 0x7ffdf000 固定されている
	dwPebAddress = 0x7ffdf000;

	/****************************************************************
	<PEB のアドレスの取得>
	Windows XP SP2 以降の OS では、PEB のアドレスが固定ではなくなったため。
	ただし、利用している ZwQueryInformationProcess API は、MSDN ライブラリにも
	記載されているとおり、今後のバージョンの Windows では利用できなくなる可能性
	があるので、サービスパックの適用や OS の入れ替え時には注意する必要がある
	http://msdn.microsoft.com/library/en-us/dllproc/base/zwqueryinformationprocess.asp?frame=true
	****************************************************************/
	HMODULE hLibrary = LoadLibrary(L"ntdll.dll");
	if (NULL != hLibrary){
        lpfnNtQueryInformationProcess = (NTQIP *) GetProcAddress(hLibrary, "ZwQueryInformationProcess");
        if (NULL != lpfnNtQueryInformationProcess)
            (*lpfnNtQueryInformationProcess)(hProcess, ProcessBasicInformation, &pbi, sizeof(pbi), &dwSize);
    }
	FreeLibrary(hLibrary);
	
	// ZwQueryInformationProcess API で取得した PEB のアドレスを設定する
	dwPebAddress = (DWORD)pbi.PebBaseAddress;

	// PEB の読み込みを行う
	BOOL bSuccess = ReadProcessMemory(hProcess, (LPVOID)dwPebAddress, &peb, sizeof(FULL_PEB), &dwSize);
	if(!bSuccess){
		CloseHandle(hProcess);
		return;
	}
	
	// プロセスパラメータブロックの読み取りを行う
	bSuccess = ReadProcessMemory(hProcess, (LPVOID)peb.ProcessParameters, &ppe, sizeof(RTL_USER_PROCESS_PARAMETERS), &dwSize);
	if(!bSuccess){
		CloseHandle(hProcess);
		return;
	}

	// コマンドライン文字列を格納するバッファを確保し、初期化する
	cCmdLine = new TCHAR[ppe.CommandLine.MaximumLength];
	memset(cCmdLine, 0, ppe.CommandLine.MaximumLength);

	// コマンドライン文字列の読み込みを行う
	bSuccess = ReadProcessMemory(hProcess, (LPVOID)ppe.CommandLine.Buffer, cCmdLine, ppe.CommandLine.MaximumLength, &dwSize);
	if(!bSuccess){
		delete cCmdLine;
		CloseHandle(hProcess);
		return;
	}

	// コマンドライン文字列の表示
	wprintf(L"CommandLine: %s\n", cCmdLine);

	// 開放処理
	delete cCmdLine;
	CloseHandle(hProcess);
}

// SeDebugPrivilege 特権の設定を行う関数
DWORD SetPrivilege( HANDLE &hToken, TOKEN_PRIVILEGES &tokenPriv )
{
    DWORD   dwResult = ERROR_SUCCESS;
    LUID    luidDebug;

    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken) != FALSE)
    {
        if (LookupPrivilegeValue(L"", SE_DEBUG_NAME, &luidDebug) != FALSE)
        {
            tokenPriv.PrivilegeCount           = 1;
            tokenPriv.Privileges[0].Luid       = luidDebug;
            tokenPriv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

            if (AdjustTokenPrivileges(hToken, FALSE, &tokenPriv, sizeof(tokenPriv), NULL, NULL) != FALSE)
                dwResult = GetLastError();
        }
        else
            dwResult = GetLastError();
    }
    else
        dwResult = GetLastError();
        
    return (dwResult);
}


int wmain(int argc, WCHAR *argv[])
{
	HANDLE              hToken;
    TOKEN_PRIVILEGES    tokenPriv = {0};

	// ロケールの設定
	_wsetlocale(LC_ALL, L"");

	switch(argc){
	case 1:
		// 引数無しで呼ばれた場合には、プロセスの列挙を行う
		EnumProcs();
		break;
	case 2:
		// 1 つの引数が与えられた場合には、引数に渡された値を PID とみなし、コマンドライン文字列の取得を行う
		// また、SeDebugPrivilege 特権が必要となるため、その設定を併せて行う
		if(SetPrivilege(hToken, tokenPriv) == ERROR_SUCCESS)
			CmdLineProcs(_wtol(argv[1]));
		else
			wprintf(L"SeDebugPrivilege 特権が取得できませんでした\n");
		break;
	default:
		// それ以外の数の引数が与えられた場合には、利用方法を表示する
		wprintf(L"利用方法: tlist2.exe [ProcessID]\n");
		break;
	}

	return 0;
}
