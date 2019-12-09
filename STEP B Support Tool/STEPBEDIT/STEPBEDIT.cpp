// STEPBEDIT.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//

#include "stdafx.h"
#include <stdio.h>
#include <windows.h>
#include <locale.h>
#include <stdlib.h>

WCHAR *swh=L"< http://www.swh.nttdata.co.jp/swh/pubqa/cid/%s >\r\n"
           L"※ソリューションウェアハウス for GroupWide をご利用の方は以下の URL をご参照ください。\r\n"
		   L"－ GWNet 版新技術　Q&A -\r\n"
		   L"< https://www.swh.groupwide.net/swh/pubqa/cid/%s >\r\n";

WCHAR *swhtips=L"< http://www.swh.nttdata.co.jp/swh/tips/cid/%s >\r\n"
			   L"※ソリューションウェアハウス for GroupWide をご利用の方は以下の URL をご参照ください。\r\n"
			   L"－ GWNet 版新技術　Q&A -\r\n"
			   L"< https://www.swh.groupwide.net/swh/tips/cid/%s >\r\n";

inline void BeforeFullWidthInSpace(WCHAR result[], WCHAR buf[], WORD pType[], WORD tmpType, DWORD &idx, DWORD i)
{
	// 前の文字が全角で、かつ"、"じゃなかったら空白入れる
	if(i > 0 && ((pType[i-1] & C3_HALFWIDTH) != C3_HALFWIDTH) && buf[i-1] != L'、'){
		result[idx++] = L' ';
	}
	else if(i == 0){
		// 行頭対応
		if(tmpType != -1 && ((tmpType & C3_HALFWIDTH) != C3_HALFWIDTH)){
			result[idx++] = L' ';
		}
	}
}

inline void BeforeHalfWidthInSpace(WCHAR result[], WCHAR buf[], WORD pType[], WORD tmpType, DWORD &idx, DWORD i)
{
	// 前の文字が半角で、スペースじゃなかったら空白入れる、かつ今の文字が "、" じゃなかったら
	if(i > 0 && ((pType[i-1] & C3_HALFWIDTH) == C3_HALFWIDTH) && buf[i-1] != L'　' && buf[i-1] != L' ' && buf[i] != L'、'){
		result[idx++] = L' ';
	}
	else if(i == 0){
		// 行頭対応
		if(tmpType != -1 && idx > 0 && ((tmpType & C3_HALFWIDTH) == C3_HALFWIDTH) && result[idx-1] != L'　' && result[idx-1] != L' ' && result[idx-1] != L'\n'){
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
		// 以降は行単位の処理
		//

		// 文字数の取得と変数の初期化
		len = wcslen(buf);
		init = 0;

		// 文字タイプの取得
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

		// 2010/1/18 [追加] コメント対応 ここから
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
		// 2010/1/18 [追加] コメント対応 ここまで

		//SWH 確認 (QA)
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
		// SWH 確認 (Tips)
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
		// 2010/1/19 [追加] 行頭URL括弧対応 ここから
		else if((p = wcsstr(buf, L"http://")) != NULL || (p = wcsstr(buf, L"https://")) != NULL){
			if(wcscmp(p, buf) == 0){
				// 冒頭 URL なのに、前の文字が改行じゃなかったら、改行いれる
				if(idx > 0 && result[idx-1] != L'\n'){
					result[idx++] = L'\r';
					result[idx++] = L'\n';
				}
				//括弧付け対応
				result[idx++] = L'<';
				result[idx++] = L' ';
				while(buf[init] != L' ' && buf[init] != L'\n' && buf[init] != L'　' && init < len){
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
		// 2010/1/19 [追加] 行頭URL括弧対応 ここまで
		// 2010/1/21 [追加] 行頭 かつ 複数行URL括弧対応 ここから
		else if(beforeUrlFlg && buf[0] != L'\n'){
			if(!((pType[0] & C3_HALFWIDTH) != C3_HALFWIDTH)){
				idx -= 4;
				while(!((pType[init] & C3_HALFWIDTH) != C3_HALFWIDTH) && buf[init] != L' ' && buf[init] != L'\n' && buf[init] != L'　' && init < len){
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
		// 2010/1/21 [追加] 行頭 かつ 複数行URL括弧対応 ここまで

		//
		// 以降は一文字ごとの処理
		//
		for( i = init; i < len; i++ ){
			// 改行確認
			if(buf[i] == L'\n'){
				tmpType = -1;
				if(i > 0){
					// 2010/1/18 [追加] 箇条書き判定対応(指定バイト未満の場合の改行は残すように) ここから
					char str[512];
					size_t slen;
					wcstombs_s(&slen, str, 512, buf, 512);
					if(strlen(str) <= ReturnSize){
						result[idx++] = L'\r';
						result[idx] = L'\n';
					}
					// 2010/1/18 [追加] 箇条書き判定対応(指定バイト未満の場合の改行は残すように) ここまで

					// 2010/1/19 [追加] 罫線判定対応 --- や === で終わってる行の改行は残す ここから
					else if(i >= 3 && 
							((buf[i-3] == L'-' && buf[i-2] == L'-' && buf[i-1] == L'-') ||
							(buf[i-3] == L'=' && buf[i-2] == L'=' && buf[i-1] == L'=') ||
							(buf[i-3] == L'#' && buf[i-2] == L'#' && buf[i-1] == L'#') ||
							(buf[i-3] == L'*' && buf[i-2] == L'*' && buf[i-1] == L'*') ||
							(buf[i-3] == L'-' && buf[i-2] == L'-' && buf[i-1] == L'+'))){
						result[idx++] = L'\r';
						result[idx] = L'\n';
					}
					// 2010/1/19 [追加] 罫線判定対応 --- や === で終わってる行の改行は残す ここまで

					// 特定の文字の後ろの改行は残す
					else if(idx > 0 && 
					        (result[idx-1] == L'。' || result[idx-1] == L'>' || result[idx-1] == L':' || 
					         result[idx-1] == L';' || result[idx-1] == L')')){
						result[idx++] = L'\r';
						result[idx] = L'\n';
					}
					// 上記以外の改行は消す
					else{
						// 改行を消す場合、行頭判定が必要なため、ひとつ前の文字のタイプを保持しておく。
						tmpType = pType[i-1];

						continue;
					}
				}
				// 空行は残す
				else{
					result[idx++] = L'\r';
					result[idx] = L'\n';
				}
			}
			// 丸付き数字対応1～9までだけ。
			else if(buf[i] >= L'①' && buf[i] <= L'⑨'){
				// 前の文字が全角だったら空白入れる
				BeforeFullWidthInSpace(result, buf, pType, tmpType, idx, i);

				pType[i] = C3_HALFWIDTH;
				result[idx++] = L'(';
				result[idx++] = buf[i] - 9263;
				result[idx] = L')';
			}
			// 各種全角記号対応
			else if(buf[i] == L'＠' || buf[i] == L'！' || buf[i] == L'＃' || buf[i] == L'＊' || 
				    buf[i] == L'（' || buf[i] == L'）' || buf[i] == L'＜' || buf[i] == L'＞' ||
					buf[i] == L'＄' || buf[i] == L'％' || buf[i] == L'＆' || buf[i] == L'’' ||
					buf[i] == L'｛' || buf[i] == L'｝' || buf[i] == L'＾' || buf[i] == L'＝' ||
					buf[i] == L'”' || buf[i] == L'￥' || buf[i] == L'＋' || buf[i] == L'；' ||
					buf[i] == L'‘' || buf[i] == L'／' || buf[i] == L'－' || buf[i] == L'｜' ||
					buf[i] == L'：' || buf[i] == L'．' || buf[i] == L'　'){
				// 前の文字が全角だったら半角入れる
				BeforeFullWidthInSpace(result, buf, pType, tmpType, idx, i);

				pType[i] = C3_HALFWIDTH;
				LCMapString(GetUserDefaultLCID(), LCMAP_HALFWIDTH, &(buf[i]), 1, &work, 1);
				result[idx] = work;
			}
			// 各種半角記号対応
			else if(buf[i] == L'@' || buf[i] == L'!' || buf[i] == L'#' || buf[i] == L'*' || 
				    buf[i] == L'(' || buf[i] == L')' || buf[i] == L'<' || buf[i] == L'>' ||
					buf[i] == L'$' || buf[i] == L'%' || buf[i] == L'&' || buf[i] == L'\'' ||
					buf[i] == L'{' || buf[i] == L'}' || buf[i] == L'^' || buf[i] == L'=' ||
					buf[i] == L'"' || buf[i] == L'\\' || buf[i] == L'+' || buf[i] == L';' ||
					buf[i] == L'`' || buf[i] == L'[' || buf[i] == L']' || buf[i] == L'/' ||
					buf[i] == L':' || buf[i] == L'-' || buf[i] == L'|' || buf[i] == L'~' || 
					buf[i] == L'.' || buf[i] == L','){

				// 2010/1/20 [追加] 複数行 URL 対応 ここまで
				BeforeFullWidthInSpace(result, buf, pType, tmpType, idx, i);

				result[idx] = buf[i];
			}
			// 半角カナを発見した場合の処理
			else if((pType[i] & C3_HALFWIDTH) && (pType[i] & C3_KATAKANA)){
				// 前の文字が半角で、スペースじゃなかったら空白入れる
				BeforeHalfWidthInSpace(result, buf, pType, tmpType, idx, i);

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
				// 前の文字が全角だったら空白入れる
				BeforeFullWidthInSpace(result, buf, pType, tmpType, idx, i);

				pType[i] = C3_HALFWIDTH;
				LCMapString(GetUserDefaultLCID(), LCMAP_HALFWIDTH, &(buf[i]), 1, &work, 1);
				result[idx] = work;
			}
			// 半角英数字を発見した場合の処理
			else if((pType[i] & C3_HALFWIDTH) && (pType2[i] & C1_DIGIT) ||
				    (pType[i] & C3_HALFWIDTH) && (pType2[i] & C1_UPPER) ||
					(pType[i] & C3_HALFWIDTH) && (pType2[i] & C1_LOWER)){

				// 前の文字が全角だったら空白入れる
				BeforeFullWidthInSpace(result, buf, pType, tmpType, idx, i);

				result[idx] = buf[i];
			}
			// その他の全角文字を発見した場合の処理
			else if((pType[i] & C3_HALFWIDTH) != C3_HALFWIDTH){
				// 前の文字が半角で、スペースじゃなかったら空白入れる
				BeforeHalfWidthInSpace(result, buf, pType, tmpType, idx, i);

				// 2010/1/18 [追加] 。の後には改行を入れる対応 ここから
				if(buf[i] == L'。' && buf[i+1] != L'\n'){
					result[idx++] = buf[i];
					result[idx++] = L'\r';
					result[idx] = L'\n';
				}
				// 2010/1/18 [追加] 。の後には改行を入れる対応 ここまで
				else{
					result[idx] = buf[i];
				}
			}
			// それ以外
			else{
				result[idx] = buf[i];
			}
			idx++;
		}
	}
	fclose(fin);

	// 2010/1/20 [追加] 辞書変換対応 ここから
	WCHAR oldString[100], newString[100];
	_wfopen_s(&fin, L"dictionary.txt", L"r");

	while(!feof(fin)){
		// 置換先文字列の読み取りと、改行削除 & ファイルの終端チェック
		fgetws(newString, 100, fin);
		if(feof(fin)) break;
		for(i = 0; newString[i] != L'\n'; i++);
		newString[i] = L'\0';

		// 最初に大文字、小文字を直すために最初に処理する
		Replace(result, bufsize, newString, newString);

		// 置換元文字列の読み取りと、改行削除 & ファイルの終端チェック
		fgetws(oldString, 100, fin);
		if(feof(fin)) break;
		for(i = 0; oldString[i] != L'\n'; i++);
		oldString[i] = L'\0';

		while(oldString[0] != L'\0' && !feof(fin)){
			Replace(result, bufsize, oldString, newString);

			// 置換元文字列の読み取りと、改行削除 & ファイルの終端チェック
			fgetws(oldString, 100, fin);
			if(feof(fin)) break;
			for(i = 0; oldString[i] != L'\n'; i++);
			oldString[i] = L'\0';
		}
	}
	// 2010/1/20 [追加] 辞書変換対応 ここまで
	fclose(fin);

	delete pType;
	delete pType2;
}