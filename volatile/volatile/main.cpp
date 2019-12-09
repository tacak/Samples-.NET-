#include <stdio.h>
#include <windows.h>

void main(void)
{
	DWORD dNum = 1;
	DWORD dPid = GetCurrentProcessId();
	
	printf("ProcessId:%ld Address:%p\n", dPid, &dNum);
	
	while(dNum){
		printf(".");
		Sleep(500);
	}
}
