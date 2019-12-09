// DeviceChange.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "DeviceChange.h"

#pragma comment(lib, "setupapi.lib")

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;								// 現在のインターフェイス
TCHAR szTitle[MAX_LOADSTRING];					// タイトル バーのテキスト
TCHAR szWindowClass[MAX_LOADSTRING];			// メイン ウィンドウ クラス名

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

TCHAR FirstDriveFromMask (ULONG unitmask);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: ここにコードを挿入してください。
	MSG msg;
	HACCEL hAccelTable;

	// グローバル文字列を初期化しています。
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_DEVICECHANGE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// アプリケーションの初期化を実行します:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DEVICECHANGE));

	// メイン メッセージ ループ:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
//  コメント:
//
//    この関数および使い方は、'RegisterClassEx' 関数が追加された
//    Windows 95 より前の Win32 システムと互換させる場合にのみ必要です。
//    アプリケーションが、関連付けられた
//    正しい形式の小さいアイコンを取得できるようにするには、
//    この関数を呼び出してください。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DEVICECHANGE));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_DEVICECHANGE);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します。
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // グローバル変数にインスタンス処理を格納します。

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:  メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND	- アプリケーション メニューの処理
//  WM_PAINT	- メイン ウィンドウの描画
//  WM_DESTROY	- 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	TCHAR szBuffer[1024], szDosName[MAX_PATH], szPathName[5], szPathNameSlash[5];
	DWORD dCount;
	PDEV_BROADCAST_HDR lpdr;
	PDEV_BROADCAST_VOLUME lpdbv;
	HDEVINFO hDev;
	SP_DEVINFO_DATA dd;
	DEVINST dInst;

	switch (message)
	{
	case WM_DEVICECHANGE:
		if(wParam == DBT_DEVICEARRIVAL && lParam != 0){
			// 接続されたデバイスのデバイスタイプを調べる
			lpdr = (PDEV_BROADCAST_HDR)lParam;
			// ボリュームタイプ以外の場合は処理を行わない
			if(lpdr->dbch_devicetype != DBT_DEVTYP_VOLUME)
				break;

			lpdbv = (PDEV_BROADCAST_VOLUME)lpdr;

			// 追加されたドライブのパス文字を得る
			_stprintf(szPathName, _T("%c:"), FirstDriveFromMask(lpdbv ->dbcv_unitmask));

			// 末尾に \ 記号を付加したパス文字を作成する
			_tcscpy(szPathNameSlash, szPathName);
			_tcscat(szPathNameSlash, _T("\\"));
			
			// パス文字の変更時にも、WM_DEVICECHANGBE が通知されるため、
			// 追加されたドライブのパス文字が Z: だった場合には処理対象外とする
			if(szPathName[0] == 'Z')
				break;

			// DOS デバイス名の取得
			QueryDosDevice(szPathName, szDosName, MAX_PATH);

			// インストール済みの全てのデバイス情報セットを得る
			hDev = SetupDiGetClassDevs(NULL, 0, 0, DIGCF_ALLCLASSES);
			dd.cbSize = sizeof(SP_DEVINFO_DATA);

			// デバイス情報を列挙する
			for(int i = 0; SetupDiEnumDeviceInfo(hDev, i, &dd); i++){
				// 子ノードの情報を得る
				CM_Get_Child(&dInst, dd.DevInst, 0);
				// 孫ノードの情報を得る
				CM_Get_Child(&dInst, dInst, 0);

				// 孫ノードが存在する場合の処理
				if(dInst != NULL){
					// デバイスのオブジェクト名を得る
					dCount = 4000;
					CM_Get_DevNode_Registry_Property(dInst, CM_DRP_PHYSICAL_DEVICE_OBJECT_NAME, NULL, (void *)szBuffer, &dCount, 0);

					// ここで得た値が上の処理で取得した DOS デバイス名と同じかどうかを判定する
					if(_tcscmp(szDosName, szBuffer) == 0){
						// デバイス名を得る
						SetupDiGetDeviceRegistryProperty(hDev,&dd, SPDRP_DEVICEDESC, NULL, (BYTE *)szBuffer, 4000 ,0);
						// 孫ノードを持ち、かつデバイス名が USB 大容量記憶装置デバイスの場合は、USB メモリと判断し、
						// パス文字の変更処理を行う
						if(_tcscmp(szBuffer, _T("USB 大容量記憶装置デバイス")) == 0){

							// パス文字列から、一意のボリューム名を得る
							if(!GetVolumeNameForVolumeMountPoint(szPathNameSlash, szBuffer, 4000)){
								_stprintf(szBuffer, _T("GetVolumeNameForVolumeMountPoint - ErrorCode ( %d )"), GetLastError());
								MessageBox(hWnd, szBuffer, _T("Error"), MB_OK);
								break;
							}

							// パス文字列に割り当てられているボリュームをアンマウントします
							if(!DeleteVolumeMountPoint(szPathNameSlash)){
								_stprintf(szBuffer, _T("DeleteVolumeMountPoint - ErrorCode ( %d )"), GetLastError());
								MessageBox(hWnd, szBuffer, _T("Error"), MB_OK);
								break;
							}

							// 新しいパス文字を指定し、ボリュームをマウントします
							if(!SetVolumeMountPoint(_T("Z:\\"), szBuffer)){
								_stprintf(szBuffer, _T("SetVolumeMOuntPoint - ErrorCode ( %d )"), GetLastError());
								MessageBox(hWnd, szBuffer, _T("Error"), MB_OK);
								break;
							}

							// SUBST を呼び出すパターンを利用する場合は、以下の実装のコメントアウトを解除し、
							// 上の GetVolumeNameForVolumeMountPoint ～ SetVolumeMountPoint の一連の実装を削除する
							/*
							// 一旦割り当てを解除する
							_stprintf(szBuffer, _T("SUBST Z: %s\\ /D"), drive);
							_tsystem(szBuffer);

							// 割り当てる
							_stprintf(szBuffer, _T("SUBST Z: %s\\"), drive);
							_tsystem(szBuffer);
							*/
							MessageBox(hWnd, _T("USB メモリが接続されたため、パス文字 Z: を再割り当てしました"), _T("Infomation"), MB_OK);
						}
					}
				}			
			}
		}
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 選択されたメニューの解析:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 描画コードをここに追加してください...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// バージョン情報ボックスのメッセージ ハンドラです。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

TCHAR FirstDriveFromMask (ULONG unitmask)
{
     TCHAR i;

      for (i = 0; i < 26; ++i)
      {
         if (unitmask & 0x1)
            break;
         unitmask = unitmask >> 1;
      }

      return (i + _T('A'));

}