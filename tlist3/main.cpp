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

// �v���Z�XID����уv���Z�X���̗񋓂��s���֐�
VOID EnumProcs(VOID)
{
	HANDLE         hProcessSnap = NULL;
	PROCESSENTRY32 pe;

	// PROCESSENTRY32 �\���̂̏�����
	memset(&pe, 0, sizeof(PROCESSENTRY32));
    pe.dwSize = sizeof(PROCESSENTRY32);
	
	// �v���Z�X�X�i�b�v�V���b�g�̎擾
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	// Process32First ����� Process32Next ��p���āA�v���Z�XID����уv���Z�X����񋓂���
	if(Process32First(hProcessSnap, &pe)){
		do{
			wprintf(L"%5u  %s\n", pe.th32ProcessID, pe.szExeFile);
		}while(Process32Next(hProcessSnap, &pe));
	}

}

// �w�肳�ꂽ�v���Z�XID�����v���Z�X�̃R�}���h���C��������������擾����֐�
VOID CmdLineProcs(DWORD dwPid)
{
    NTQIP *lpfnNtQueryInformationProcess;

	TCHAR *cCmdLine;
	DWORD dwPebAddress, dwSize;

	PROCESS_BASIC_INFORMATION pbi;
	FULL_PEB peb;
	RTL_USER_PROCESS_PARAMETERS ppe;

	// �w�肳�ꂽ�v���Z�X ID �ɑΉ�����v���Z�X���J��
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPid);
	if(hProcess == NULL){
		wprintf(L"�v���Z�XID %d �̃v���Z�X�̃I�[�v���Ɏ��s���܂���\n", dwPid);
		return;
	}

	// Windows XP SP1 �ȑO�̃o�[�W������ PEB �̃A�h���X�� 0x7ffdf000 �Œ肳��Ă���
	dwPebAddress = 0x7ffdf000;

	/****************************************************************
	<PEB �̃A�h���X�̎擾>
	Windows XP SP2 �ȍ~�� OS �ł́APEB �̃A�h���X���Œ�ł͂Ȃ��Ȃ������߁B
	�������A���p���Ă��� ZwQueryInformationProcess API �́AMSDN ���C�u�����ɂ�
	�L�ڂ���Ă���Ƃ���A����̃o�[�W������ Windows �ł͗��p�ł��Ȃ��Ȃ�\��
	������̂ŁA�T�[�r�X�p�b�N�̓K�p�� OS �̓���ւ����ɂ͒��ӂ���K�v������
	http://msdn.microsoft.com/library/en-us/dllproc/base/zwqueryinformationprocess.asp?frame=true
	****************************************************************/
	HMODULE hLibrary = LoadLibrary(L"ntdll.dll");
	if (NULL != hLibrary){
        lpfnNtQueryInformationProcess = (NTQIP *) GetProcAddress(hLibrary, "ZwQueryInformationProcess");
        if (NULL != lpfnNtQueryInformationProcess)
            (*lpfnNtQueryInformationProcess)(hProcess, ProcessBasicInformation, &pbi, sizeof(pbi), &dwSize);
    }
	FreeLibrary(hLibrary);
	
	// ZwQueryInformationProcess API �Ŏ擾���� PEB �̃A�h���X��ݒ肷��
	dwPebAddress = (DWORD)pbi.PebBaseAddress;

	// PEB �̓ǂݍ��݂��s��
	BOOL bSuccess = ReadProcessMemory(hProcess, (LPVOID)dwPebAddress, &peb, sizeof(FULL_PEB), &dwSize);
	if(!bSuccess){
		CloseHandle(hProcess);
		return;
	}
	
	// �v���Z�X�p�����[�^�u���b�N�̓ǂݎ����s��
	bSuccess = ReadProcessMemory(hProcess, (LPVOID)peb.ProcessParameters, &ppe, sizeof(RTL_USER_PROCESS_PARAMETERS), &dwSize);
	if(!bSuccess){
		CloseHandle(hProcess);
		return;
	}

	// �R�}���h���C����������i�[����o�b�t�@���m�ۂ��A����������
	cCmdLine = new TCHAR[ppe.CommandLine.MaximumLength];
	memset(cCmdLine, 0, ppe.CommandLine.MaximumLength);

	// �R�}���h���C��������̓ǂݍ��݂��s��
	bSuccess = ReadProcessMemory(hProcess, (LPVOID)ppe.CommandLine.Buffer, cCmdLine, ppe.CommandLine.MaximumLength, &dwSize);
	if(!bSuccess){
		delete cCmdLine;
		CloseHandle(hProcess);
		return;
	}

	// �R�}���h���C��������̕\��
	wprintf(L"CommandLine: %s\n", cCmdLine);

	// �J������
	delete cCmdLine;
	CloseHandle(hProcess);
}

// SeDebugPrivilege �����̐ݒ���s���֐�
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

	// ���P�[���̐ݒ�
	_wsetlocale(LC_ALL, L"");

	switch(argc){
	case 1:
		// ���������ŌĂ΂ꂽ�ꍇ�ɂ́A�v���Z�X�̗񋓂��s��
		EnumProcs();
		break;
	case 2:
		// 1 �̈������^����ꂽ�ꍇ�ɂ́A�����ɓn���ꂽ�l�� PID �Ƃ݂Ȃ��A�R�}���h���C��������̎擾���s��
		// �܂��ASeDebugPrivilege �������K�v�ƂȂ邽�߁A���̐ݒ�𕹂��čs��
		if(SetPrivilege(hToken, tokenPriv) == ERROR_SUCCESS)
			CmdLineProcs(_wtol(argv[1]));
		else
			wprintf(L"SeDebugPrivilege �������擾�ł��܂���ł���\n");
		break;
	default:
		// ����ȊO�̐��̈������^����ꂽ�ꍇ�ɂ́A���p���@��\������
		wprintf(L"���p���@: tlist2.exe [ProcessID]\n");
		break;
	}

	return 0;
}
