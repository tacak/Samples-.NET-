// TestCLRDLL.cpp : DLL アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <malloc.h>

using namespace System;
using namespace System::Runtime::InteropServices;

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

#ifdef _MANAGED
#pragma managed(pop)
#endif

void TestFree(void *p)
{
	IntPtr ip(p);
	Marshal::FreeHGlobal(ip);
}

wchar_t ** _stdcall TestFunction()
{
	
	wchar_t **wc = 0;
	/*
	wc = (wchar_t **)malloc(sizeof(wchar_t *) * 3);
	array<System::String^> ^strs = ClassLibrary1::Class1::TestFunc(L"Message");

	int i = 0;
	IntPtr p;
	for each(System::String ^str in strs){
		p = Marshal::StringToHGlobalUni(str);
		wc[i] = static_cast<wchar_t *>(p.ToPointer());
	}
	*/
	return wc;
}