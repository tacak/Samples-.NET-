#include <stdio.h>
#include <windows.h>
#include <locale.h>

WCHAR *gw=L"※ GW 版をご参照の方はこちら。\n< http://hogeuga/cid/%s >\n";
WCHAR *swh=L"< http://ugahoge/cid/%s >\n※ GW 版をご参照の方はこちら。\n< http://hogeuga/cid/%s >\n";

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
			 LOCALE_SYSTEM_DEFAULT    // ユニコード版プロジェクトでは、無視される引数       
			,CT_CTYPE3                // 全角、半角、ひらがな等を判定するタイプ
			,buf                  // 判定対象となる文字配列
			,len                     // 判定対象となる文字配列のサイズ
			,pType                    // 判定結果の配列
			);
		GetStringTypeEx(
			 LOCALE_SYSTEM_DEFAULT    // ユニコード版プロジェクトでは、無視される引数       
			,CT_CTYPE1                // 全角、半角、ひらがな等を判定するタイプ
			,buf                  // 判定対象となる文字配列
			,len                     // 判定対象となる文字配列のサイズ
			,pType2                    // 判定結果の配列
			);

		BOOL Urlretflg = TRUE;
		for( int i = 0; i < len; i++ ){
			if(Urlretflg && wcsstr(buf, L"http://") != NULL){
				result[idx++] = L'\n';
				Urlretflg = FALSE;
			}

			/*
			URL 検索・追加処理
			strstr()で調べて、strcat() で追加。idxをそれなりに増やす。
			URL処理したら、ブレークでいいかもしれん。
			*/
			//SWH 確認
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
			//GW 確認
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

			// 改行確認
			if(buf[i] == L'\n'){
				if(i > 0){
					if(buf[i-1] == L'。' || buf[i-1] == L'>'){
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
			// 各種全角記号対応
			else if(buf[i] == L'＠' || buf[i] == L'！' || buf[i] == L'＃' || buf[i] == L'　' || 
				    buf[i] == L'（' || buf[i] == L'）' || buf[i] == L'＜' || buf[i] == L'＞' ||
					buf[i] == L'＄' || buf[i] == L'％' || buf[i] == L'＆' || buf[i] == L'’' ||
					buf[i] == L'｛' || buf[i] == L'｝' || buf[i] == L'＾' || buf[i] == L'＝' ||
					buf[i] == L'”' || buf[i] == L'￥' || buf[i] == L'＋' || buf[i] == L'；' ||
					buf[i] == L'：' || buf[i] == L'＊'){
				if(i > 0 && ((pType[i-1] & C3_HALFWIDTH) != C3_HALFWIDTH)){
					result[idx++] = L' ';
				}
				pType[i] = C3_HALFWIDTH;
				LCMapString(GetUserDefaultLCID(), LCMAP_HALFWIDTH, &(buf[i]), 1, &work, 1);
				result[idx] = work;
			}
			// 半角カナを発見した場合の処理
			else if((pType[i] & C3_HALFWIDTH) && (pType[i] & C3_KATAKANA)){
				if(i > 0 && ((pType[i-1] & C3_HALFWIDTH) == C3_HALFWIDTH)){
					result[idx++] = L' ';
				}
				pType[i] = C3_FULLWIDTH;
				if(buf[i] == L'ﾞ'){
					continue;
				}
				LCMapString(GetUserDefaultLCID(), LCMAP_FULLWIDTH, &(buf[i]), 2, &work, 1);
				result[idx] = work;
			}
			// 全角英数を発見した場合の処理
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
			// 半角英数字を発見した場合の処理
			else if((pType[i] & C3_HALFWIDTH) && (pType2[i] & C1_DIGIT) ||
				    (pType[i] & C3_HALFWIDTH) && (pType2[i] & C1_UPPER) ||
					(pType[i] & C3_HALFWIDTH) && (pType2[i] & C1_LOWER)){
				if(i > 0 && ((pType[i-1] & C3_HALFWIDTH) != C3_HALFWIDTH)){
					result[idx++] = L' ';
				}
				result[idx] = buf[i];
			}
			// その他の全角文字を発見した場合の処理
			else if((pType[i] & C3_HALFWIDTH) != C3_HALFWIDTH){
				if(i > 0 && ((pType[i-1] & C3_HALFWIDTH) == C3_HALFWIDTH)){
					result[idx++] = L' ';
				}
				result[idx] = buf[i];
			}
			// それ以外
			else{
				result[idx] = buf[i];
			}
			idx++;
		}
	}
	wprintf(L"\n\n=====\n%s\n", result);
	fclose(fin);	
}