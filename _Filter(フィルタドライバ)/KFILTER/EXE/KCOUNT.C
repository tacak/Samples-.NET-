/******************************************************************************
*
*	kCOUNT.C - Windows XP Application 
*	キーコード別入力カウンタを取得し表示するアプリケーション
*
*	Copyright (c) 2002-10-09 AIdesign, M.Takiguchi
*
******************************************************************************/

#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <conio.h>
#include "kCOUNT.h"

#define	YY						20
#define	MAXCHKCNT				256
#define FILE_DEVICE_UNKNOWN		0x220000

LRESULT CALLBACK WindowFunc(HWND, UINT, WPARAM, LPARAM);
HANDLE _declspec(dllimport) _stdcall KeyCountOpen(void);
int    _declspec(dllimport) _stdcall KeyCountGet(HANDLE, LPVOID, DWORD);
VOID   _declspec(dllimport) _stdcall KeyCountClose(HANDLE);

char szWinName[] = "MyWin"; /* ウィンドウクラスの名前 */

char str[255];  /* 出力文字列を格納する */
SYSTEM_INFO SystemInfo;

int Y=0, X=0;   /* 現在の出力位置 */
int maxX, maxY; /* 画面のサイズ */

int WINAPI WinMain(HINSTANCE hThisInst, HINSTANCE hPrevInst, 
                   LPSTR lpszArgs, int nWinMode)
{
  HWND hwnd;
  MSG msg;
  WNDCLASSEX wcl;
  HACCEL hAccel;

  /* ウィンドウクラスを定義する */
  wcl.cbSize = sizeof(WNDCLASSEX); 

  wcl.hInstance = hThisInst;     /* このインスタンスのハンドル */
  wcl.lpszClassName = szWinName; /* ウィンドウクラスの名前 */
  wcl.lpfnWndProc = WindowFunc;  /* ウィンドウ関数 */
  wcl.style = 0; /* デフォルトのスタイル */

  wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION);   /* 標準アイコン */
  wcl.hIconSm = LoadIcon(NULL, IDI_APPLICATION); /* 小さいアイコン */
  wcl.hCursor = LoadCursor(NULL, IDC_ARROW);     /* カーソルのスタイル */
 
  wcl.lpszMenuName = "MyMenu"; /* メインメニュー */
  wcl.cbClsExtra = 0; /* そのほかの情報は必要ない */
  wcl.cbWndExtra = 0; /* そのほかの情報は必要ない */

  /* ウィンドウの背景を白にする */
  wcl.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH); 

  /* ウィンドウクラスを登録する */
  if(!RegisterClassEx(&wcl)) return 0;

  /* ウィンドウクラスを登録したので、ウィンドウを作成できる */
  hwnd = CreateWindow(
    szWinName,           /* ウィンドウクラスの名前 */
    "Key scan count",   /* タイトル */
    WS_OVERLAPPEDWINDOW, /* ウィンドウスタイル - 標準 */
    CW_USEDEFAULT,       /* X座標 - Windowsに決めさせる */
    CW_USEDEFAULT,       /* Y座標 - Windowsに決めさせる */
    CW_USEDEFAULT,       /* 幅 - Windowsに決めさせる */
    CW_USEDEFAULT,       /* 高さ - Windowsに決めさせる */
    HWND_DESKTOP,        /* 親ウィンドウのハンドルはない */
    NULL,                /* ウィンドウクラスメニューを上書きしない */
    hThisInst,           /* プログラムのこのインスタンスのハンドル */
    NULL                 /* このほかの引数は必要ない */
  );

  /* アクセラレータをロードする */
  hAccel = LoadAccelerators(hThisInst, "MyMenu");

  /* ウィンドウを表示する */
  ShowWindow(hwnd, nWinMode);
  UpdateWindow(hwnd);

  /* メッセージループを作成する */
  while(GetMessage(&msg, NULL, 0, 0))
  {
    if(!TranslateAccelerator(hwnd, hAccel, &msg)) {
      TranslateMessage(&msg); /* キーボードメッセージを変換する */
      DispatchMessage(&msg);  /* Windows 98に制御を返す */
    }
  }
  return msg.wParam;
}

int		Count=0, KeyCount[MAXCHKCNT+2];

LRESULT CALLBACK WindowFunc(HWND hwnd, UINT message,
                            WPARAM wParam, LPARAM lParam)
{
  HDC			hdc;
  TEXTMETRIC	tm;
  int response, i=0, j, l;
  HANDLE		Handle;

  switch(message) {
    case WM_CREATE:
      /* 画面座標を取得する */
      maxX = GetSystemMetrics(SM_CXSCREEN);
      maxY = GetSystemMetrics(SM_CYSCREEN);
      break;
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDM_SHOW: 
          ++Count;
          hdc = GetDC(hwnd);			/* デバイスコンテキストを取得する */
          SetTextColor(hdc, RGB(0, 0, 0));		/* テキスト色を黒に設定する */
          if(Count & 1)
	        SetBkColor(hdc, RGB(0, 255, 255));	/* 背景色を水色に設定する */
          else
			SetBkColor(hdc, RGB(0, 255, 0));	/* 背景色を緑色に設定する */
          GetTextMetrics(hdc, &tm);		/* テキストメトリックスを取得する */
		  tm.tmExternalLeading=1;		// 2->1

		Handle = KeyCountOpen();		// 解析ドライバを開く
		if(Handle == INVALID_HANDLE_VALUE){
			l = GetLastError();
			if(l == 2L)
              sprintf(str,"kFILTER.sys no file,err_code=%x,Quit the Program",l);
            MessageBox(hwnd, str, "Exit", MB_OK);
			PostQuitMessage(0);
			break;
		}

		GetSystemInfo(&SystemInfo);		// システム情報を取得する
		sprintf(str, "SystemInfo : %d %d %d %d",
			SystemInfo.dwPageSize, SystemInfo.dwNumberOfProcessors,
			SystemInfo.dwProcessorType, SystemInfo.wProcessorLevel);
		l = strlen(str);
		if(SystemInfo.wProcessorLevel != 15)
			sprintf(&str[l], "   computer:VIA Samuel ");
		else
			sprintf(&str[l], "   computer:Epson Endeavor ");
		TextOut(hdc, 176, YY, str, strlen(str));

		sprintf(str, "copyright(c) M.Takiguchi");
		TextOut(hdc, 586, YY, str, strlen(str));

		j = KeyCountGet(Handle, KeyCount, 4+4*MAXCHKCNT);
	if(j){
		sprintf(str, "調査時間=%d秒", KeyCount[0]);	// 先頭は調査時間
		TextOut(hdc, 2, YY+24, str, strlen(str));	// その後にｷｰｺｰﾄﾞ別ﾘｰﾄﾞｶｳﾝﾀ
		j -= 4;
		sprintf(str, "コード別入力回数");
		TextOut(hdc, 290, YY+24, str, strlen(str));	// コード別入力回数タイトル
		sprintf(str, "データ数 = %d bytes", j);
		TextOut(hdc, 586, YY+24, str, strlen(str));	// データ数を表示する

		sprintf(str, "ｷｰｺｰﾄﾞ");
		TextOut(hdc, 2, YY+60, str, strlen(str));	// ヘッダー
		for(X = 6*tm.tmAveCharWidth, i=0; i<16; ++i){
		   	sprintf(str, "    %X", i);
			TextOut(hdc, X, YY+60, str, strlen(str));	// 0,1,2...A,B,C,D,E,F
            X = X + 5*tm.tmAveCharWidth;
		}
        Y = 68;
        X = 0;
		for(i=0; i<MAXCHKCNT; ++i){
		  if((i%16)==0){
	            Y = Y + tm.tmHeight + tm.tmExternalLeading; /* 次の行 */
		   		sprintf(str, "  %X", i/16);					// 上位４ビット
		    	TextOut(hdc, 16, YY+Y, str, strlen(str));	// 上位４ビット表示
	            X = 6*tm.tmAveCharWidth;
	      }
		    sprintf(str, "%5d", KeyCount[i+1]);
		    TextOut(hdc, X, YY+Y, str, strlen(str));	// コード別入力回数表示
            X = X + 5*tm.tmAveCharWidth;
            j -= 4;
            if(j <= 0)	break;
		}
	}
		KeyCountClose(Handle);							// 解析ドライバを閉じる

          ReleaseDC(hwnd, hdc); /* デバイスコンテキストを解放する */
          break;
        case IDM_BEEP:
          Beep(2000, 500);
          break;
        case IDM_HELP: 
          MessageBox(hwnd, "Version 1.02 [2002-09-26]", "Help", MB_OK);
          break;
        case IDM_EXIT:
          response = MessageBox(hwnd, "Quit the Program?", "Exit", MB_YESNO);
          if(response == IDYES) PostQuitMessage(0);
          break;
      }
      break;
    case WM_DESTROY: /* プログラムを終了する */
      PostQuitMessage(0);
      break;
    default: 
     /* 上記のswitchステートメントで指定していない
     						メッセージは、Windowsに処理させる */
     return DefWindowProc(hwnd, message, wParam, lParam);
  }
  return 0;
}
