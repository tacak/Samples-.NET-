
/******************************************************************************
*
*	aCount.C - Windows XP Application 
*	指定したドライバの機能別稼働回数とディスパッチ関数アドレスを表示するアプリケ
*   ーション
*
*	Copyright (c) 2002 AIdesign
*
******************************************************************************/

#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <conio.h>
#include "aCount.h"

#define	IRP_MJ_MAXIMUM_FUNCTION	0x1b

    UCHAR *MJTable[IRP_MJ_MAXIMUM_FUNCTION+1] = {
        "IRP_MJ_CREATE",                   // 0x00
        "IRP_MJ_CREATE_NAMED_PIPE",        // 0x01
        "IRP_MJ_CLOSE",                    // 0x02
        "IRP_MJ_READ",                     // 0x03
        "IRP_MJ_WRITE",                    // 0x04
        "IRP_MJ_QUERY_INFORMATION",        // 0x05
        "IRP_MJ_SET_INFORMATION",          // 0x06
        "IRP_MJ_QUERY_EA",                 // 0x07
        "IRP_MJ_SET_EA",                   // 0x08
        "IRP_MJ_FLUSH_BUFFERS",            // 0x09
        "IRP_MJ_QUERY_VOLUME_INFORMATION", // 0x0a
        "IRP_MJ_SET_VOLUME_INFORMATION",   // 0x0b
        "IRP_MJ_DIRECTORY_CONTROL",        // 0x0c
        "IRP_MJ_FILE_SYSTEM_CONTROL",      // 0x0d
        "IRP_MJ_DEVICE_CONTROL",           // 0x0e
        "IRP_MJ_INTERNAL_DEVICE_CONTROL",  // 0x0f
        "IRP_MJ_SHUTDOWN",                 // 0x10
        "IRP_MJ_LOCK_CONTROL",             // 0x11
        "IRP_MJ_CLEANUP",                  // 0x12
        "IRP_MJ_CREATE_MAILSLOT",          // 0x13
        "IRP_MJ_QUERY_SECURITY",           // 0x14
        "IRP_MJ_SET_SECURITY",             // 0x15
        "IRP_MJ_QUERY_POWER",              // 0x16
        "IRP_MJ_SET_POWER",                // 0x17
        "IRP_MJ_DEVICE_CHANGE",            // 0x18
        "IRP_MJ_QUERY_QUOTA",              // 0x19
        "IRP_MJ_SET_QUOTA",                // 0x1a
        "IRP_MJ_PNP_POWER"                 // 0x1b
    };

LRESULT CALLBACK WindowFunc(HWND, UINT, WPARAM, LPARAM);

char szWinName[] = "MyWin"; /* ウィンドウクラスの名前 */

char str[255];  /* 出力文字列を格納する */

int Y=0;        /* 現在の出力位置 */
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
    "Driver-Analyzer",   /* タイトル */
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

unsigned long	Count=0;
unsigned long	fCount[2 * (IRP_MJ_MAXIMUM_FUNCTION+1)];
char	Buf[256];

LRESULT CALLBACK WindowFunc(HWND hwnd, UINT message,
                            WPARAM wParam, LPARAM lParam)
{
  HDC			hdc;
  TEXTMETRIC	tm;
  int response, i, j, l;
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
			SetBkColor(hdc, RGB(166, 202, 240));/* 背景色をｽｶｲﾌﾞﾙｰに設定する */
          GetTextMetrics(hdc, &tm);		/* テキストメトリックスを取得する */
		  tm.tmExternalLeading=1;		// 2->1

		Handle=
		CreateFile("\\\\.\\aCount",GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|
		FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(Handle == INVALID_HANDLE_VALUE){
            MessageBox(hwnd, "No file(ANALYSIS.sys), Quit the Program", "Exit", MB_OK);
			PostQuitMessage(0);
			break;
		}

		  l = DeviceIoControl(Handle, 8192, &i, 4, fCount,
									4*(IRP_MJ_MAXIMUM_FUNCTION+1)*2, &j, NULL);

          Y = 0;
		  for(i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; ++i){
		    sprintf(str, "%5d", fCount[i]);
		    TextOut(hdc, 0,   Y, str, strlen(str));		/* 参照回数 */
		    sprintf(str, "%5d", i);
		    TextOut(hdc, 50,  Y, str, strlen(str));		/* MajorFunction番号 */
		    sprintf(str, "%s", MJTable[i]);
			TextOut(hdc, 100, Y, str, strlen(str));		/* 文字列を出力する */
			sprintf(str, "%08X", fCount[i+IRP_MJ_MAXIMUM_FUNCTION+1]);
			TextOut(hdc, 440, Y, str, strlen(str));		/* 関数アドレス */
            Y = Y + tm.tmHeight + tm.tmExternalLeading; /* 次の行 */
		  }
		CloseHandle(Handle);

          ReleaseDC(hwnd, hdc); /* デバイスコンテキストを解放する */
          break;
        case IDM_BEEP:
          Beep(2000, 500);
          break;
        case IDM_HELP: 
          MessageBox(hwnd, "Version 1.00 [2002-09-03]", "Help", MB_OK);
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