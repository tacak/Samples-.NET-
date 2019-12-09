// FtpClient.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <windows.h>
#include <wininet.h>

#pragma comment(lib, "wininet.lib")

int _tmain(int argc, _TCHAR* argv[])
{
		HINTERNET hInternet;
	HINTERNET hFtpSession;

	/* WININETの初期化 */
	hInternet = InternetOpen(
		L"WININET Sample Program",
		INTERNET_OPEN_TYPE_DIRECT,
		NULL,
		NULL,
		0);

	/* FTPセッションの確立 */
	hFtpSession = InternetConnect(
		hInternet,
		L"localhost",
		INTERNET_DEFAULT_FTP_PORT,
		L"TacaK",
		L"tk5734",
		INTERNET_SERVICE_FTP,
		0,
		0);

	HINTERNET hFtpFile;
	DWORD dSize;
	hFtpFile = FtpOpenFile(hFtpSession, L"log.txt", GENERIC_READ, FTP_TRANSFER_TYPE_UNKNOWN, 0);
	FtpGetFileSize(hFtpFile, &dSize);


	TCHAR buf[256];
	wsprintf(buf, L"%d", dSize);

	MessageBox(NULL, buf, L"Size", MB_OK);

	/* リモートにあるファイルをローカルへ転送 */
	FtpGetFile(
		hFtpSession,
		L"log.txt",
		L"E:\\log.txt",
		FALSE,
		FILE_ATTRIBUTE_ARCHIVE,
		FTP_TRANSFER_TYPE_ASCII,
		0);

	/* 後処理 */
	InternetCloseHandle(hFtpSession);
	InternetCloseHandle(hInternet);

	return 0;
}
