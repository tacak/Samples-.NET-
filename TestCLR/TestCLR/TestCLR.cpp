// TestCLR.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
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
	std::string text;  // ��������󂯎~�߂�o�b�t�@
String^ textString = gcnew String("�K���ȃT���v��������");  // �ϊ����镶����

// System::String ��Char �^ = wchar_t �^�̔z��ɂ���
array<Char>^ warr = textString->ToCharArray();

// �z�񂪃K�x�[�W�E�R���N�g�ɂ���Ĉړ����Ȃ��悤�s���j���O
pin_ptr<Char> wptr = &warr[0];

// �ϊ��㕶���T�C�Y���擾
char *buffer = 0;
int len = ::WideCharToMultiByte(CP_UTF8, 0, wptr, num, buffer, 0, NULL, NULL);
if ( len > 0 ) {

    // �����o�b�t�@���擾����
    buffer = new char[len + 1];
    memset(buffer, 0, len+1);

    // UNICODE �� MultiByte �ɕϊ�
    ::WideCharToMultiByte(CP_UTF8, 0, wptr, num, buffer, len, NULL, NULL);

    // �o�b�t�@��MultiByte�̕����^�ɑ��
    text = buffer
	*/
}

