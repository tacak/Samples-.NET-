// TestCLR.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <windows.h>

using namespace System;
using namespace System::Runtime::InteropServices;

void Test(wchar_t *str)
{
	::MessageBox(NULL, str, L"info", MB_OK);
}

void TestFree(void *p)
{
	IntPtr ip(p);
	Marshal::FreeHGlobal(ip);
}

wchar_t **TestFunction()
{
	/*
	wchar_t **wc;

	wc = (wchar_t **)malloc(sizeof(wchar_t *) * 3);
	array<System::String^> ^strs = ClassLibrary1::Class1::TestFunc(L"Message");

	int i = 0;
	IntPtr p;
	for each(System::String ^str in strs){
		p = Marshal::StringToHGlobalUni(str);
		wc[i] = static_cast<wchar_t *>(p.ToPointer());

		Test(wc[i]);

		//Marshal::FreeHGlobal(p);
		TestFree(wc[i]);

		Test(wc[i++]);
	}

	//for(i = 0; i < 3; i++){
		//Test(wc[i]);
		//TestFree(wc[i]);
		//Marshal::FreeHGlobal(p);
	//}
	*/
	return 0;

	/*
	std::string text;  // 文字列を受け止めるバッファ
String^ textString = gcnew String("適当なサンプル文字列");  // 変換する文字列

// System::String をChar 型 = wchar_t 型の配列にする
array<Char>^ warr = textString->ToCharArray();

// 配列がガベージ・コレクトによって移動しないようピンニング
pin_ptr<Char> wptr = &warr[0];

// 変換後文字サイズを取得
char *buffer = 0;
int len = ::WideCharToMultiByte(CP_UTF8, 0, wptr, num, buffer, 0, NULL, NULL);
if ( len > 0 ) {

    // 文字バッファを取得して
    buffer = new char[len + 1];
    memset(buffer, 0, len+1);

    // UNICODE を MultiByte に変換
    ::WideCharToMultiByte(CP_UTF8, 0, wptr, num, buffer, len, NULL, NULL);

    // バッファをMultiByteの文字型に代入
    text = buffer
	*/
}

