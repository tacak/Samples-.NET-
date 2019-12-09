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
	
	// バッファ領域に全コピーしつつ、出力領域をクリア
	wcscpy_s(buf, bufsize, result);
	memset(result, 0, bufsize);
	
	//全角かどうかの判断がいるかもしれん。
	//lower とか upper が全角に対してまともに動かんかったら。
	
	// 置換対象文字列のサイズ
	oldlen = wcslen(oldString);
	
	// 置換先文字列のサイズ
	newlen = wcslen(newString);
	
	// 全小文字の変換対照文字列を作る
	for(i = 0; i < oldlen; i++){
		lowerString[i] = towlower(oldString[i]);
	}
	lowerString[i] = L'\0';

	// 頭文字の大文字バージョン
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
	
	wchar_t *oldss = L"ドメコン", *newss = L"ドメインコントローラ";
	wcscpy(ss, L"これはドメコンです。これもドメコンです。");
	
	Replace(ss, 100, oldss, newss);
	
	wprintf(L"%s\n", ss);
}