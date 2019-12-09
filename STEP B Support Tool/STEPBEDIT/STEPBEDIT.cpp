// STEPBEDIT.cpp : DLL �A�v���P�[�V�����p�ɃG�N�X�|�[�g�����֐����`���܂��B
//

#include "stdafx.h"
#include <stdio.h>
#include <windows.h>
#include <locale.h>
#include <stdlib.h>

WCHAR *swh=L"< http://www.swh.nttdata.co.jp/swh/pubqa/cid/%s >\r\n"
           L"���\�����[�V�����E�F�A�n�E�X for GroupWide �������p�̕��͈ȉ��� URL �����Q�Ƃ��������B\r\n"
		   L"�| GWNet �ŐV�Z�p�@Q&A -\r\n"
		   L"< https://www.swh.groupwide.net/swh/pubqa/cid/%s >\r\n";

WCHAR *swhtips=L"< http://www.swh.nttdata.co.jp/swh/tips/cid/%s >\r\n"
			   L"���\�����[�V�����E�F�A�n�E�X for GroupWide �������p�̕��͈ȉ��� URL �����Q�Ƃ��������B\r\n"
			   L"�| GWNet �ŐV�Z�p�@Q&A -\r\n"
			   L"< https://www.swh.groupwide.net/swh/tips/cid/%s >\r\n";

inline void BeforeFullWidthInSpace(WCHAR result[], WCHAR buf[], WORD pType[], WORD tmpType, DWORD &idx, DWORD i)
{
	// �O�̕������S�p�ŁA����"�A"����Ȃ�������󔒓����
	if(i > 0 && ((pType[i-1] & C3_HALFWIDTH) != C3_HALFWIDTH) && buf[i-1] != L'�A'){
		result[idx++] = L' ';
	}
	else if(i == 0){
		// �s���Ή�
		if(tmpType != -1 && ((tmpType & C3_HALFWIDTH) != C3_HALFWIDTH)){
			result[idx++] = L' ';
		}
	}
}

inline void BeforeHalfWidthInSpace(WCHAR result[], WCHAR buf[], WORD pType[], WORD tmpType, DWORD &idx, DWORD i)
{
	// �O�̕��������p�ŁA�X�y�[�X����Ȃ�������󔒓����A�����̕����� "�A" ����Ȃ�������
	if(i > 0 && ((pType[i-1] & C3_HALFWIDTH) == C3_HALFWIDTH) && buf[i-1] != L'�@' && buf[i-1] != L' ' && buf[i] != L'�A'){
		result[idx++] = L' ';
	}
	else if(i == 0){
		// �s���Ή�
		if(tmpType != -1 && idx > 0 && ((tmpType & C3_HALFWIDTH) == C3_HALFWIDTH) && result[idx-1] != L'�@' && result[idx-1] != L' ' && result[idx-1] != L'\n'){
			result[idx++] = L' ';
		}
	}
}

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
			
			BOOL UrlFlg = FALSE;
			if(wcscmp(sString, lowerString) == 0){
				for(j = i; j >= 6 && buf[j] != L'\n'; j--){
					if((buf[j-6] == L'h' && buf[j-5] == L't' && buf[j-4] == L't' && buf[j-3] == L'p' && buf[j-2] == L':' && buf[j-1] == L'/' && buf[j] == L'/') ||
					  (buf[j-6] == L'h' && buf[j-5] == L't' && buf[j-4] == L't' && buf[j-3] == L'p' && buf[j-2] == L's' && buf[j-1] == L':' && buf[j] == L'/')){
						UrlFlg = TRUE;
					}
				}
				if(!UrlFlg){
					for(j = 0; j < newlen; j++){
						result[idx++] = newString[j];
					}
					i += oldlen -1;
					continue;
				}
			}
		}
		result[idx++] = buf[i];
	}
}

void FormatStepB(LPWSTR result, int bufsize, size_t ReturnSize)
{
	_wsetlocale(LC_ALL, L"");

	WCHAR *p, qa[20];
	WCHAR work;
	WCHAR buf[512], url[512];
	DWORD len;
	WORD *pType = new WORD[512];
	WORD *pType2 = new WORD[512];
	BOOL beforeUrlFlg = FALSE;

	memset(result, 0, bufsize);

	FILE *fin;
	_wfopen_s(&fin, L"tmpfile", L"r");

	DWORD idx = 0;
	WORD tmpType = -1;
	BOOL NoEditFlg = FALSE;
	DWORD i, init;
	while(fgetws(buf, 512, fin) != NULL){
		//
		// �ȍ~�͍s�P�ʂ̏���
		//

		// �������̎擾�ƕϐ��̏�����
		len = wcslen(buf);
		init = 0;

		// �����^�C�v�̎擾
		GetStringTypeEx(
			 LOCALE_SYSTEM_DEFAULT    // ���j�R�[�h�Ńv���W�F�N�g�ł́A������������       
			,CT_CTYPE3                // �S�p�A���p�A�Ђ炪�ȓ��𔻒肷��^�C�v
			,buf                  // ����ΏۂƂȂ镶���z��
			,len                     // ����ΏۂƂȂ镶���z��̃T�C�Y
			,pType                    // ���茋�ʂ̔z��
			);
		GetStringTypeEx(
			 LOCALE_SYSTEM_DEFAULT    // ���j�R�[�h�Ńv���W�F�N�g�ł́A������������       
			,CT_CTYPE1                // �S�p�A���p�A�Ђ炪�ȓ��𔻒肷��^�C�v
			,buf                  // ����ΏۂƂȂ镶���z��
			,len                     // ����ΏۂƂȂ镶���z��̃T�C�Y
			,pType2                    // ���茋�ʂ̔z��
			);

		// 2010/1/18 [�ǉ�] �R�����g�Ή� ��������
		if(!NoEditFlg && (p = wcsstr(buf, L"/@@")) != NULL){
			if(wcscmp(buf, p) == 0){
				NoEditFlg = TRUE;
				continue;
			}
		}
		else if(NoEditFlg && (p = wcsstr(buf, L"@@/")) != NULL){
			if(wcscmp(buf, p) == 0){
				NoEditFlg = FALSE;
				continue;
			}
		}

		if(NoEditFlg){
			for(i = 0; buf[i] != L'\n' && i < len; i++){
				result[idx++] = buf[i];
			}
			result[idx++] = L'\r';
			result[idx++] = L'\n';
			continue;
		}
		// 2010/1/18 [�ǉ�] �R�����g�Ή� �����܂�

		//SWH �m�F (QA)
		if(wcsstr(buf, L"http://www.swh.nttdata.co.jp/swh/pubqa/cid/") != NULL ||
		   wcsstr(buf, L"https://www.swh.groupwide.net/swh/pubqa/cid/") != NULL){
			memset(qa, 0, sizeof(WCHAR) * 20);
			if((p = wcsstr(buf, L"QA20")) != NULL || (p = wcsstr(buf, L"QA19")) != NULL){
				for(int j = 0; *(p + j) != L'\n' && *(p + j) != L'>' && *(p + j) != L' ' && j < 20; j++){
					qa[j] = *(p + j);
				}
				swprintf(url, 512, swh, qa, qa);
				wcscat_s(result, bufsize, url);

				idx = idx + wcslen(url);
				continue;
			}
		}
		// SWH �m�F (Tips)
		else if(wcsstr(buf, L"http://www.swh.nttdata.co.jp/swh/tips/cid/") != NULL ||
				wcsstr(buf, L"https://www.swh.groupwide.net/swh/tips/cid/") != NULL){
			memset(qa, 0, sizeof(WCHAR) * 20);
			if((p = wcsstr(buf, L"T20")) != NULL || (p = wcsstr(buf, L"T19")) != NULL){
				for(int j = 0; *(p + j) != L'\n' && *(p + j) != L'>' && *(p + j) != L' ' && j < 20; j++){
					qa[j] = *(p + j);
				}
				swprintf(url, 512, swhtips, qa, qa);
				wcscat_s(result, bufsize, url);

				idx = idx + wcslen(url);
				continue;
			}
		}
		// 2010/1/19 [�ǉ�] �s��URL���ʑΉ� ��������
		else if((p = wcsstr(buf, L"http://")) != NULL || (p = wcsstr(buf, L"https://")) != NULL){
			if(wcscmp(p, buf) == 0){
				// �`�� URL �Ȃ̂ɁA�O�̕��������s����Ȃ�������A���s�����
				if(idx > 0 && result[idx-1] != L'\n'){
					result[idx++] = L'\r';
					result[idx++] = L'\n';
				}
				//���ʕt���Ή�
				result[idx++] = L'<';
				result[idx++] = L' ';
				while(buf[init] != L' ' && buf[init] != L'\n' && buf[init] != L'�@' && init < len){
					result[idx++] = buf[init++];
				}
				result[idx++] = L' ';
				result[idx++] = L'>';
				result[idx++] = L'\r';
				result[idx++] = L'\n';

				if(buf[init] == L'\n'){
					beforeUrlFlg = TRUE;
					continue;
				}
			}
		}
		// 2010/1/19 [�ǉ�] �s��URL���ʑΉ� �����܂�
		// 2010/1/21 [�ǉ�] �s�� ���� �����sURL���ʑΉ� ��������
		else if(beforeUrlFlg && buf[0] != L'\n'){
			if(!((pType[0] & C3_HALFWIDTH) != C3_HALFWIDTH)){
				idx -= 4;
				while(!((pType[init] & C3_HALFWIDTH) != C3_HALFWIDTH) && buf[init] != L' ' && buf[init] != L'\n' && buf[init] != L'�@' && init < len){
					result[idx++] = buf[init++];
				}
				result[idx++] = L' ';
				result[idx++] = L'>';
				result[idx++] = L'\r';
				result[idx++] = L'\n';

				if(buf[init] == L'\n'){
					beforeUrlFlg = TRUE;
					continue;
				}
			}
		}
		beforeUrlFlg = FALSE;
		// 2010/1/21 [�ǉ�] �s�� ���� �����sURL���ʑΉ� �����܂�

		//
		// �ȍ~�͈ꕶ�����Ƃ̏���
		//
		for( i = init; i < len; i++ ){
			// ���s�m�F
			if(buf[i] == L'\n'){
				tmpType = -1;
				if(i > 0){
					// 2010/1/18 [�ǉ�] �ӏ���������Ή�(�w��o�C�g�����̏ꍇ�̉��s�͎c���悤��) ��������
					char str[512];
					size_t slen;
					wcstombs_s(&slen, str, 512, buf, 512);
					if(strlen(str) <= ReturnSize){
						result[idx++] = L'\r';
						result[idx] = L'\n';
					}
					// 2010/1/18 [�ǉ�] �ӏ���������Ή�(�w��o�C�g�����̏ꍇ�̉��s�͎c���悤��) �����܂�

					// 2010/1/19 [�ǉ�] �r������Ή� --- �� === �ŏI����Ă�s�̉��s�͎c�� ��������
					else if(i >= 3 && 
							((buf[i-3] == L'-' && buf[i-2] == L'-' && buf[i-1] == L'-') ||
							(buf[i-3] == L'=' && buf[i-2] == L'=' && buf[i-1] == L'=') ||
							(buf[i-3] == L'#' && buf[i-2] == L'#' && buf[i-1] == L'#') ||
							(buf[i-3] == L'*' && buf[i-2] == L'*' && buf[i-1] == L'*') ||
							(buf[i-3] == L'-' && buf[i-2] == L'-' && buf[i-1] == L'+'))){
						result[idx++] = L'\r';
						result[idx] = L'\n';
					}
					// 2010/1/19 [�ǉ�] �r������Ή� --- �� === �ŏI����Ă�s�̉��s�͎c�� �����܂�

					// ����̕����̌��̉��s�͎c��
					else if(idx > 0 && 
					        (result[idx-1] == L'�B' || result[idx-1] == L'>' || result[idx-1] == L':' || 
					         result[idx-1] == L';' || result[idx-1] == L')')){
						result[idx++] = L'\r';
						result[idx] = L'\n';
					}
					// ��L�ȊO�̉��s�͏���
					else{
						// ���s�������ꍇ�A�s�����肪�K�v�Ȃ��߁A�ЂƂO�̕����̃^�C�v��ێ����Ă����B
						tmpType = pType[i-1];

						continue;
					}
				}
				// ��s�͎c��
				else{
					result[idx++] = L'\r';
					result[idx] = L'\n';
				}
			}
			// �ەt�������Ή�1�`9�܂ł����B
			else if(buf[i] >= L'�@' && buf[i] <= L'�H'){
				// �O�̕������S�p��������󔒓����
				BeforeFullWidthInSpace(result, buf, pType, tmpType, idx, i);

				pType[i] = C3_HALFWIDTH;
				result[idx++] = L'(';
				result[idx++] = buf[i] - 9263;
				result[idx] = L')';
			}
			// �e��S�p�L���Ή�
			else if(buf[i] == L'��' || buf[i] == L'�I' || buf[i] == L'��' || buf[i] == L'��' || 
				    buf[i] == L'�i' || buf[i] == L'�j' || buf[i] == L'��' || buf[i] == L'��' ||
					buf[i] == L'��' || buf[i] == L'��' || buf[i] == L'��' || buf[i] == L'�f' ||
					buf[i] == L'�o' || buf[i] == L'�p' || buf[i] == L'�O' || buf[i] == L'��' ||
					buf[i] == L'�h' || buf[i] == L'��' || buf[i] == L'�{' || buf[i] == L'�G' ||
					buf[i] == L'�e' || buf[i] == L'�^' || buf[i] == L'�|' || buf[i] == L'�b' ||
					buf[i] == L'�F' || buf[i] == L'�D' || buf[i] == L'�@'){
				// �O�̕������S�p�������甼�p�����
				BeforeFullWidthInSpace(result, buf, pType, tmpType, idx, i);

				pType[i] = C3_HALFWIDTH;
				LCMapString(GetUserDefaultLCID(), LCMAP_HALFWIDTH, &(buf[i]), 1, &work, 1);
				result[idx] = work;
			}
			// �e�피�p�L���Ή�
			else if(buf[i] == L'@' || buf[i] == L'!' || buf[i] == L'#' || buf[i] == L'*' || 
				    buf[i] == L'(' || buf[i] == L')' || buf[i] == L'<' || buf[i] == L'>' ||
					buf[i] == L'$' || buf[i] == L'%' || buf[i] == L'&' || buf[i] == L'\'' ||
					buf[i] == L'{' || buf[i] == L'}' || buf[i] == L'^' || buf[i] == L'=' ||
					buf[i] == L'"' || buf[i] == L'\\' || buf[i] == L'+' || buf[i] == L';' ||
					buf[i] == L'`' || buf[i] == L'[' || buf[i] == L']' || buf[i] == L'/' ||
					buf[i] == L':' || buf[i] == L'-' || buf[i] == L'|' || buf[i] == L'~' || 
					buf[i] == L'.' || buf[i] == L','){

				// 2010/1/20 [�ǉ�] �����s URL �Ή� �����܂�
				BeforeFullWidthInSpace(result, buf, pType, tmpType, idx, i);

				result[idx] = buf[i];
			}
			// ���p�J�i�𔭌������ꍇ�̏���
			else if((pType[i] & C3_HALFWIDTH) && (pType[i] & C3_KATAKANA)){
				// �O�̕��������p�ŁA�X�y�[�X����Ȃ�������󔒓����
				BeforeHalfWidthInSpace(result, buf, pType, tmpType, idx, i);

				pType[i] = C3_FULLWIDTH;
				if(buf[i] == L'�'){
					continue;
				}
				LCMapString(GetUserDefaultLCID(), LCMAP_FULLWIDTH, &(buf[i]), 2, &work, 1);
				result[idx] = work;
			}
			// �S�p�p���𔭌������ꍇ�̏���
			else if((pType[i] & C3_FULLWIDTH) && (pType2[i] & C1_DIGIT) ||
				    (pType[i] & C3_FULLWIDTH) && (pType2[i] & C1_UPPER) ||
					(pType[i] & C3_FULLWIDTH) && (pType2[i] & C1_LOWER)){
				// �O�̕������S�p��������󔒓����
				BeforeFullWidthInSpace(result, buf, pType, tmpType, idx, i);

				pType[i] = C3_HALFWIDTH;
				LCMapString(GetUserDefaultLCID(), LCMAP_HALFWIDTH, &(buf[i]), 1, &work, 1);
				result[idx] = work;
			}
			// ���p�p�����𔭌������ꍇ�̏���
			else if((pType[i] & C3_HALFWIDTH) && (pType2[i] & C1_DIGIT) ||
				    (pType[i] & C3_HALFWIDTH) && (pType2[i] & C1_UPPER) ||
					(pType[i] & C3_HALFWIDTH) && (pType2[i] & C1_LOWER)){

				// �O�̕������S�p��������󔒓����
				BeforeFullWidthInSpace(result, buf, pType, tmpType, idx, i);

				result[idx] = buf[i];
			}
			// ���̑��̑S�p�����𔭌������ꍇ�̏���
			else if((pType[i] & C3_HALFWIDTH) != C3_HALFWIDTH){
				// �O�̕��������p�ŁA�X�y�[�X����Ȃ�������󔒓����
				BeforeHalfWidthInSpace(result, buf, pType, tmpType, idx, i);

				// 2010/1/18 [�ǉ�] �B�̌�ɂ͉��s������Ή� ��������
				if(buf[i] == L'�B' && buf[i+1] != L'\n'){
					result[idx++] = buf[i];
					result[idx++] = L'\r';
					result[idx] = L'\n';
				}
				// 2010/1/18 [�ǉ�] �B�̌�ɂ͉��s������Ή� �����܂�
				else{
					result[idx] = buf[i];
				}
			}
			// ����ȊO
			else{
				result[idx] = buf[i];
			}
			idx++;
		}
	}
	fclose(fin);

	// 2010/1/20 [�ǉ�] �����ϊ��Ή� ��������
	WCHAR oldString[100], newString[100];
	_wfopen_s(&fin, L"dictionary.txt", L"r");

	while(!feof(fin)){
		// �u���敶����̓ǂݎ��ƁA���s�폜 & �t�@�C���̏I�[�`�F�b�N
		fgetws(newString, 100, fin);
		if(feof(fin)) break;
		for(i = 0; newString[i] != L'\n'; i++);
		newString[i] = L'\0';

		// �ŏ��ɑ啶���A�������𒼂����߂ɍŏ��ɏ�������
		Replace(result, bufsize, newString, newString);

		// �u����������̓ǂݎ��ƁA���s�폜 & �t�@�C���̏I�[�`�F�b�N
		fgetws(oldString, 100, fin);
		if(feof(fin)) break;
		for(i = 0; oldString[i] != L'\n'; i++);
		oldString[i] = L'\0';

		while(oldString[0] != L'\0' && !feof(fin)){
			Replace(result, bufsize, oldString, newString);

			// �u����������̓ǂݎ��ƁA���s�폜 & �t�@�C���̏I�[�`�F�b�N
			fgetws(oldString, 100, fin);
			if(feof(fin)) break;
			for(i = 0; oldString[i] != L'\n'; i++);
			oldString[i] = L'\0';
		}
	}
	// 2010/1/20 [�ǉ�] �����ϊ��Ή� �����܂�
	fclose(fin);

	delete pType;
	delete pType2;
}