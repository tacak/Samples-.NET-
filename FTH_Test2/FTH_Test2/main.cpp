#include <stdio.h>
#include <windows.h>

void main(void)
{

	char* p = (char *)HeapAlloc(GetProcessHeap(), 0, 100);
	HeapDestroy(p);
	memset(p, 0, 10);

	for(int i = 0; i < 1000; i++){
		p = (char *)HeapAlloc(GetProcessHeap(), 0, 100);
	}
}