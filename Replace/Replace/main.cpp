#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <ctype.h>

void Replace(wchar_t result[], int bufsize, wchar_t oldString[], wchar_t newString[])
{
	wchar_t *buf = new wchar_t[bufsize];
	wchar_t uphead, lowerString[100];
	int idx, i, j, oldlen, newlen;
	wchar_t sString[100];
	
	// �o�b�t�@�̈�ɑS�R�s�[���A�o�͗̈���N���A
	wcscpy_s(buf, bufsize, result);
	memset(result, 0, bufsize);
	
	//�S�p���ǂ����̔��f�����邩�������B
	//lower �Ƃ� upper ���S�p�ɑ΂��Ă܂Ƃ��ɓ����񂩂�����B
	
	// �u���Ώە�����̃T�C�Y
	oldlen = wcslen(oldString);
	
	// �u���敶����̃T�C�Y
	newlen = wcslen(newString);
	
	// �S�������̕ϊ��Ώƕ���������
	for(i = 0; i < oldlen; i++){
		lowerString[i] = towlower(oldString[i]);
	}
	lowerString[i] = L'\0';

	// �������̑啶���o�[�W����
	uphead = towupper(oldString[0]);
	
	idx = 0;
	for(i = 0; buf[i] != L'\0' && i < bufsize; i++){
		if(buf[i] == uphead || buf[i] == lowerString[0]){
			for(j = 0; j < oldlen; j++){
				sString[j] = towlower(buf[i+j]);
			}
			sString[j] = L'\0';
			
			if(wcscmp(sString, lowerString) == 0){
				for(j = 0; j < newlen; j++){
					result[idx++] = newString[j];
				}
				i += oldlen -1;
				continue;
			}
		}
		result[idx++] = buf[i];
	}
}

void wmain(void)
{
	_wsetlocale(LC_ALL, L"");
	
	wchar_t *ss = new wchar_t[100];
	memset(ss, 0, sizeof(wchar_t) * 100);
	
	wchar_t *oldss = L"�h���R��", *newss = L"�h���C���R���g���[��";
	wcscpy(ss, L"����̓h���R���ł��B������h���R���ł��B");
	
	Replace(ss, 100, oldss, newss);
	
	wprintf(L"%s\n", ss);
}