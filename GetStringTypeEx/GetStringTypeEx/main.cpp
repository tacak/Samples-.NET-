#include <stdio.h>
#include <windows.h>
#include <locale.h>

WCHAR *gw=L"�� GW �ł����Q�Ƃ̕��͂�����B\n< http://hogeuga/cid/%s >\n";
WCHAR *swh=L"< http://ugahoge/cid/%s >\n�� GW �ł����Q�Ƃ̕��͂�����B\n< http://hogeuga/cid/%s >\n";

void wmain(void)
{
	_wsetlocale(LC_ALL, L"");

	FILE *fin;
	fin = _wfopen(L"C:\\test.txt", L"r");

	WCHAR *p, qa[20];
	WCHAR *result = (WCHAR *)malloc(sizeof(WCHAR) * 30000);

	WCHAR buf[512], work;
	WCHAR url[512];
	DWORD len;
	WORD *pType = new WORD[50];
	WORD *pType2 = new WORD[50];

	memset(result, 0, sizeof(WCHAR) * 30000);

	int idx = 0;
	while(fgetws(buf, 512, fin) != NULL){
		len = wcslen(buf);
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

		BOOL Urlretflg = TRUE;
		for( int i = 0; i < len; i++ ){
			if(Urlretflg && wcsstr(buf, L"http://") != NULL){
				result[idx++] = L'\n';
				Urlretflg = FALSE;
			}

			/*
			URL �����E�ǉ�����
			strstr()�Œ��ׂāAstrcat() �Œǉ��Bidx������Ȃ�ɑ��₷�B
			URL����������A�u���[�N�ł������������B
			*/
			//SWH �m�F
			if(wcsstr(buf, L"http://ugahoge") != NULL){
				memset(qa, 0, sizeof(WCHAR) * 20);
				if((p = wcsstr(buf, L"QA20")) != NULL || (p = wcsstr(buf, L"QA19")) != NULL ||
				   (p = wcsstr(buf, L"T20")) != NULL || (p = wcsstr(buf, L"T19")) != NULL){
					for(int j = 0; *(p + j) != L'\n' && *(p + j) != L'>' && *(p + j) != L' ' && j < 20; j++){
						qa[j] = *(p + j);
					}
					wcscat(result, buf);

					swprintf(url, 512, gw, qa);
					wcscat(result, url);

					idx = idx + len + wcslen(url);
					break;
				}
			}
			//GW �m�F
			else if(wcsstr(buf, L"http://hogeuga") != NULL){
				memset(qa, 0, sizeof(WCHAR) * 20);
				if((p = wcsstr(buf, L"QA20")) != NULL || (p = wcsstr(buf, L"QA19")) != NULL ||
				   (p = wcsstr(buf, L"T20")) != NULL || (p = wcsstr(buf, L"T19")) != NULL){
					for(int j = 0; *(p + j) != L'\n' && *(p + j) != L'>' && *(p + j) != L' ' && j < 20; j++){
						qa[j] = *(p + j);
					}
					swprintf(url, 512, swh, qa, qa);
					wcscat(result, url);

					idx = idx + wcslen(url);
					break;
				}
			}

			// ���s�m�F
			if(buf[i] == L'\n'){
				if(i > 0){
					if(buf[i-1] == L'�B' || buf[i-1] == L'>'){
						result[idx] = buf[i];
					}
					else{
						continue;
					}
				}
				else{
					result[idx] = buf[i];
				}
			}
			// �e��S�p�L���Ή�
			else if(buf[i] == L'��' || buf[i] == L'�I' || buf[i] == L'��' || buf[i] == L'�@' || 
				    buf[i] == L'�i' || buf[i] == L'�j' || buf[i] == L'��' || buf[i] == L'��' ||
					buf[i] == L'��' || buf[i] == L'��' || buf[i] == L'��' || buf[i] == L'�f' ||
					buf[i] == L'�o' || buf[i] == L'�p' || buf[i] == L'�O' || buf[i] == L'��' ||
					buf[i] == L'�h' || buf[i] == L'��' || buf[i] == L'�{' || buf[i] == L'�G' ||
					buf[i] == L'�F' || buf[i] == L'��'){
				if(i > 0 && ((pType[i-1] & C3_HALFWIDTH) != C3_HALFWIDTH)){
					result[idx++] = L' ';
				}
				pType[i] = C3_HALFWIDTH;
				LCMapString(GetUserDefaultLCID(), LCMAP_HALFWIDTH, &(buf[i]), 1, &work, 1);
				result[idx] = work;
			}
			// ���p�J�i�𔭌������ꍇ�̏���
			else if((pType[i] & C3_HALFWIDTH) && (pType[i] & C3_KATAKANA)){
				if(i > 0 && ((pType[i-1] & C3_HALFWIDTH) == C3_HALFWIDTH)){
					result[idx++] = L' ';
				}
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
				if(i > 0 && ((pType[i-1] & C3_HALFWIDTH) != C3_HALFWIDTH)){
					result[idx++] = L' ';
				}
				pType[i] = C3_HALFWIDTH;
				LCMapString(GetUserDefaultLCID(), LCMAP_HALFWIDTH, &(buf[i]), 1, &work, 1);
				result[idx] = work;

			}
			// ���p�p�����𔭌������ꍇ�̏���
			else if((pType[i] & C3_HALFWIDTH) && (pType2[i] & C1_DIGIT) ||
				    (pType[i] & C3_HALFWIDTH) && (pType2[i] & C1_UPPER) ||
					(pType[i] & C3_HALFWIDTH) && (pType2[i] & C1_LOWER)){
				if(i > 0 && ((pType[i-1] & C3_HALFWIDTH) != C3_HALFWIDTH)){
					result[idx++] = L' ';
				}
				result[idx] = buf[i];
			}
			// ���̑��̑S�p�����𔭌������ꍇ�̏���
			else if((pType[i] & C3_HALFWIDTH) != C3_HALFWIDTH){
				if(i > 0 && ((pType[i-1] & C3_HALFWIDTH) == C3_HALFWIDTH)){
					result[idx++] = L' ';
				}
				result[idx] = buf[i];
			}
			// ����ȊO
			else{
				result[idx] = buf[i];
			}
			idx++;
		}
	}
	wprintf(L"\n\n=====\n%s\n", result);
	fclose(fin);	
}