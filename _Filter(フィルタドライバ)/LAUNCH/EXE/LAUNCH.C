// LAUNCH.C
/******************************************************************************
*
*       Launch.C - Windows XP/2000 Dynamic Driver Installer
*
*               Copyright (c) 2002 M.Takiguchi
*
*       PROGRAM: Filter Driver Launch
*
*       PURPOSE: Loads and unloads device drivers. This code
*               is taken from the instdrv example in the XP/2000 DDK.
*
******************************************************************************/

#include <windows.h>
#include <string.h>
#include <stdio.h>
#include "Launch.h"
#include "UdebugView.h"

LRESULT	CALLBACK WindowFunc(HWND, UINT, WPARAM, LPARAM);
BOOL	CALLBACK DialogFunc1(HWND, UINT, WPARAM, LPARAM);
BOOL	CALLBACK DialogFunc2(HWND, UINT, WPARAM, LPARAM);

char szWinName[] = "MyWin"; /* ウィンドウクラスの名前 */

HINSTANCE hInst;

int WINAPI WinMain(HINSTANCE hThisInst, HINSTANCE hPrevInst, 
                   LPSTR lpszArgs, int nWinMode)
{
  HWND hwnd;
  MSG msg;
  WNDCLASSEX wcl;
  HACCEL hAccel;

  /* ウィンドウクラスを定義する */
  wcl.cbSize = sizeof(WNDCLASSEX); 

  wcl.hInstance 	= hThisInst;	/* このインスタンスのハンドル */
  wcl.lpszClassName = szWinName;	/* ウィンドウクラスの名前 */
  wcl.lpfnWndProc	= WindowFunc; 	/* ウィンドウ関数 */
  wcl.style			= 0;			/* デフォルトのスタイル */

  wcl.hIcon			= LoadIcon(NULL, IDI_APPLICATION);	/* 標準アイコン */
  wcl.hIconSm		= LoadIcon(NULL, IDI_WINLOGO);		/* 小さいアイコン */
  wcl.hCursor		= LoadCursor(NULL, IDC_ARROW);		/* カーソルスタイル */
 
  wcl.lpszMenuName	= "MyMenu";		/* メインメニュー */
  wcl.cbClsExtra	= 0;			/* そのほかの情報は必要ない */
  wcl.cbWndExtra	= 0;			/* そのほかの情報は必要ない */

  /* ウィンドウの背景を白にする */
  wcl.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH); 

  /* ウィンドウクラスを登録する */
  if(!RegisterClassEx(&wcl)) return 0;

  /* ウィンドウクラスを登録したので、ウィンドウを作成できる */
  hwnd = CreateWindow(
    szWinName,					/* ウィンドウクラスの名前 */
    "ドライバ  インスト－ラ",	/* タイトル */
    WS_OVERLAPPEDWINDOW,		/* ウィンドウスタイル - 標準 */
    CW_USEDEFAULT,				/* X座標 - Windowsに決めさせる */
    CW_USEDEFAULT,				/* Y座標 - Windowsに決めさせる */
    CW_USEDEFAULT,				/* 幅 - Windowsに決めさせる */
    CW_USEDEFAULT,				/* 高さ - Windowsに決めさせる */
    HWND_DESKTOP,				/* 親ウィンドウはない */
    NULL,						/* ウィンドウクラスメニューを上書きしない */
    hThisInst,					/* プログラムのこのインスタンスのハンドル */
    NULL						/* このほかの引数は必要ない */
  );

  hInst = hThisInst; /* 現在のインスタンスハンドルを保存する */

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
      DispatchMessage(&msg);  /* Windowsに制御を返す */
    }
  }
  return msg.wParam;
}

/* この関数はWindowsによって呼び出され、
   メッセージキューからメッセージを受け取る
*/
LRESULT CALLBACK WindowFunc(HWND hwnd, UINT message,
                            WPARAM wParam, LPARAM lParam)
{
  int response;

  switch(message) {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
        case IDM_DIALOG1: 
          DialogBox(hInst, "MyDB1", hwnd, (DLGPROC) DialogFunc1);
      	  PostQuitMessage(0);
		  break;
        case IDM_DIALOG2: 
          DialogBox(hInst, "MyDB2", hwnd, (DLGPROC) DialogFunc2);
      	  PostQuitMessage(0);
		  break;
        case IDM_EXIT:  
          response = MessageBox(hwnd, "Quit the Program?", "Exit", MB_YESNO);
          if(response == IDYES) PostQuitMessage(0);
          break;
        case IDM_HELP: 
          MessageBox(hwnd, "Version 1.00, 2002-11-28, Copyright AIdesign",
          												"Help", MB_OK);
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

  char	DriverName[256];
  char	str[80];

/* システムフォルダにドライバをインストールする関数 */
BOOL CALLBACK DialogFunc1(HWND hdwnd, UINT message,
                         WPARAM wParam, LPARAM lParam)
{
  int	i;
  switch(message) {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
		case IDOK:
		  GetDlgItemText(hdwnd, ID_EB1, str, 80);	// エディットボックス処理
		  lstrcpy(DriverName, str);
		  lstrcat(str, ".SYS");						// .SYSを付加する
		  i = MessageBox(hdwnd, str, "次のドライバをインストールしますか？",
		  								MB_YESNOCANCEL);
		  if(i==IDCANCEL)	return 1;
		  if(i==IDYES){								// インストールを開始する
				uPrint("goto InstallDriver %x\n", i);
				i = InstallDriver(DriverName, ".\\");
				uPrint("Install-4 %x\n", i);
				if(i==TRUE)
					MessageBox(hdwnd, "インストール完了", "normal", MB_OK);
			  else{
				sprintf(str,"インストール中にエラーがありました(ErrorCode=%x)",
															GetLastError());
				MessageBox(hdwnd, str, "error", MB_OK);
			  }
		  }
		  else
			  MessageBox(hdwnd, "インストール中止", "normal", MB_OK);
		  PostQuitMessage(0);
		  break;
      }
  }
  return 0;
}

/* システムフォルダからドライバをアンインストールする関数 */
BOOL CALLBACK DialogFunc2(HWND hdwnd, UINT message,
                         WPARAM wParam, LPARAM lParam)
{
  int	i;

  switch(message) {
    case WM_COMMAND:
      switch(LOWORD(wParam)) {
		case IDOK:
		  GetDlgItemText(hdwnd, ID_EB1, str, 80);	// エディットボックス処理
		  lstrcpy(DriverName, str);
		  lstrcat(str, ".SYS");						// .SYSを付加する
		  i = MessageBox(hdwnd, str,"次のドライバをアンインストールしますか？",
		  								MB_YESNOCANCEL);
		  if(i==IDCANCEL)	return 1;
		  if(i==IDYES){		// uninstall
			uPrint("UnInstall-M1 %s\n", DriverName);
			if(i==IDYES){
			  i = UnInstallDriver(DriverName);
			  uPrint("UnInstall-M2 %x %s\n", i, DriverName);
			  if(i==TRUE)
				MessageBox(hdwnd, "アンインストール完了", "normal", MB_OK);
			  else{
				sprintf(str,"アンインストール中にエラーがありました(ErrorCode=%x)",
															GetLastError());
				MessageBox(hdwnd, str, "error", MB_OK);
			  }
			}
			else
			  MessageBox(hdwnd, "アンインストール中止", "normal", MB_OK);
		  }
		  PostQuitMessage(0);
		  break;
      }
  }
  return 0;
}
