// TestDLL.cpp : DLL アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <stdio.h>
#include <malloc.h>
#include <objbase.h>

#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

void _stdcall TestFunc(char **pp)
{
	pp = (char **)malloc(sizeof(char *) * 3);
	for(int i = 0; i < 3; i++){
		*(pp + i) = (char *)malloc(20);
		sprintf(*(pp + i), "TestFunc%d", i + 1);
	}
}

void _stdcall TestFunc2(char *p)
{
	p = (char *)malloc(20);
	strcpy(p, "ABCDE");
}

void _stdcall TestFunc3(char **pp)
{
	*pp = (char *)CoTaskMemAlloc(20);
	strcpy(*pp, "TestFunc3");
}


void _stdcall TestFunc4(char **pp)
{
	*pp = (char *)malloc(20);
	strcpy(*pp, "TestFunc4");
}

void _stdcall TestFunc5(char **pp)
{
	pp = (char **)CoTaskMemAlloc(sizeof(char *) * 3);
	for(int i = 0; i < 3; i++){
		*(pp + i) = (char *)CoTaskMemAlloc(20);
		sprintf(*(pp + i), "TestFunc5-%d", i + 1);
	}
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

