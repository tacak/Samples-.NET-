#define _WIN32_WINNT 0x500
#define _UNICODE
#define UNICODE

#include <windows.h>
#include <stdio.h>
#include <tlhelp32.h>
#include <vdmdbg.h>
#include <WInternl.h>
#include <atlconv.h>

typedef LONG (WINAPI NTQIP)(HANDLE, PROCESSINFOCLASS, PVOID, ULONG, PULONG);

VOID EnumProcs(VOID);
VOID CmdLineProcs(DWORD );
DWORD SetPrivilege( HANDLE &, TOKEN_PRIVILEGES &);

VOID EnumProcs(VOID)
{
	HANDLE         hProcessSnap = NULL;
	PROCESSENTRY32 lppe;

	// PROCESSENTRY32 構造体の初期化
	memset(&lppe, 0, sizeof(lppe));
    lppe.dwSize = sizeof(lppe);
	
	// プロセススナップショットの取得
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	// Process32First および Process32Next を用いて、プロセスID・イメージ名を列挙する
	if(Process32First(hProcessSnap, &lppe)){
		do{
			wprintf(L"%5u  %s\n", lppe.th32ProcessID, lppe.szExeFile);
		}while(Process32Next(hProcessSnap, &lppe));
	}

}

VOID CmdLineProcs(DWORD dwPid)
{
	TCHAR *CmdLine;
	DWORD dwReadAddress, dwPebAddress, dwParameterAddress, dwCmdLineAddress, dwSize;
	WORD wCharCount;
	PROCESS_BASIC_INFORMATION pbi;
    NTQIP *lpfnNtQueryInformationProcess;


	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPid);

	HMODULE hLibrary = GetModuleHandle(L"ntdll.dll");
	if (NULL != hLibrary){
        lpfnNtQueryInformationProcess = (NTQIP *) GetProcAddress(hLibrary, "ZwQueryInformationProcess");
        if (NULL != lpfnNtQueryInformationProcess)
            (*lpfnNtQueryInformationProcess)(hProcess, ProcessBasicInformation, &pbi, sizeof(pbi), &dwSize);
    }

	//dwPebAddress = 0x7ffdf000 + 16;
	dwPebAddress = (DWORD)pbi.PebBaseAddress + 16;
	BOOL bSuccess = ReadProcessMemory(hProcess, (LPVOID)dwPebAddress, &dwParameterAddress, sizeof(DWORD), &dwSize);
	if(!bSuccess){
		printf("%d\n", GetLastError());
	}

	dwReadAddress = dwParameterAddress + 66;
	bSuccess = ReadProcessMemory(hProcess, (LPVOID)dwReadAddress, &wCharCount, sizeof(WORD), &dwSize);
		if(!bSuccess){
		printf("%d\n", GetLastError());
	}

	dwReadAddress = dwParameterAddress + 68;
	bSuccess = ReadProcessMemory(hProcess, (LPVOID)dwReadAddress, &dwCmdLineAddress, sizeof(DWORD), &dwSize);
	if(!bSuccess){
		printf("%d\n", GetLastError());
	}

	CmdLine = new TCHAR[wCharCount];
	memset(CmdLine, 0, wCharCount);

    bSuccess = ReadProcessMemory(hProcess, (LPVOID)dwCmdLineAddress, CmdLine, wCharCount, &dwSize);
	if(!bSuccess){
		printf("%d\n", GetLastError());
	}

	wprintf(L"CommandLine: %s\n", CmdLine);

	delete CmdLine;
}

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

int wmain(int argc, TCHAR *argv[])
{
	HANDLE              hToken;
    TOKEN_PRIVILEGES    tokenPriv = {0};

	if(argc != 2){
		EnumProcs();
	}
	else{
		SetPrivilege(hToken, tokenPriv);
		CmdLineProcs(_wtol(argv[1]));
	}

	return 0;
}
