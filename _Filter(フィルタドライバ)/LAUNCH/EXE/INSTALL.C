/******************************************************************************
*
*       INSTALL.C - Windows XP/2000 Dynamic Driver Installer
*
*                Copyright (c) 2002 M.Takiguchi
*
*       PROGRAM: Filter Driver Launch
*
*       PURPOSE: Loads and unloads device drivers. This code
*                is taken from the instdrv example in the XP/2000 DDK.
*                Base on WDM-BOOK(Walter Oney).
*
******************************************************************************/

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include "UdebugView.h"

#define	AfxMessageBox	uPrint		// デバッグに用いる、完成後コメント扱い

BOOL	CreateDriver( const char * DriverName, const char * FullDriver );
BOOL	StartDriver( const char * DriverName );
BOOL	StopDriver( schSCManager, Name );
BOOL	RemoveDriver( schSCManager, Name );
int		FindInMultiSz(LPTSTR MultiSz, int MultiSzLen, const char * Match );
TCHAR	StrBuf[_MAX_PATH];

/**************************************************************************
*
*    FUNCTION: InstallDriver(const char *DriverName,const char *DriverFromPath)
*
*    DriveName      : ドライバ名
*
*    DriverFromPath : パス名
*
*    PURPOSE        : システムフォルダにドライバをインストールする関数
*
**************************************************************************/

BOOL InstallDriver( const char * DriverName, const char * DriverFromPath)
{
	/////////////////////////////////////////////////////////////////////////
	// Get System32 directory

	TCHAR	EventMessageFile[_MAX_PATH];
	TCHAR	DriverFullPath[_MAX_PATH];
	HKEY	mru;
	DWORD	disposition;
	DWORD	dwRegValue;
	DWORD	DataSize;
	DWORD	Type;
	int		DriverNameLen;
	PVOID	sources;
	BOOL	rt=TRUE;

	uPrint("InstallDriver-IN %s %s\n", DriverName, DriverFromPath);
	if( 0==GetSystemDirectory(DriverFullPath, _MAX_PATH)) // System32Directory
	{
		AfxMessageBox("Could not find Windows system directory");
		return FALSE;
	}
	uPrint("InstallDriver-2 %s\n", DriverFullPath);	// 入手したSystem32フォルダ

	/////////////////////////////////////////////////////////////////////////
	// Copy driver .sys file across

	strcat(DriverFullPath, "\\Drivers\\");		// 転送先フルパス名
	strcat(DriverFullPath, DriverName);
	strcat(DriverFullPath, ".sys");
	strcpy(EventMessageFile, DriverFromPath);	// 転送元フルパス名
	strcat(EventMessageFile, DriverName);
	strcat(EventMessageFile, ".sys");
	uPrint("InstallDriver-3 %s -> %s\n", EventMessageFile, DriverFullPath);
		// 指定ドライバファイルをシステムフォルダにコピーする。（上書き許可）
	if( 0==CopyFile( EventMessageFile, DriverFullPath, FALSE)) // Overwrite OK
	{	// エラー発生のとき、原因をデバッグ表示する
		uPrint("Could not copy %s to %s\n", DriverFromPath, DriverFullPath);
		uPrint("%x\n", GetLastError());
		return FALSE;
	}

	/////////////////////////////////////////////////////////////////////////
	// Create driver (or stop existing driver)
	// サービス制御マーネジャとの接続を確立し、既存のサービスハンドルを開く

	if( !CreateDriver( DriverName, DriverFullPath))
		return FALSE;

	//////////////////// レジストリを作る ///////////////////////////////////
	//	Create/Open driver registry key and set its values
	//	Overwrite registry values written in driver creation

	strcpy(StrBuf, "SYSTEM\\CurrentControlSet\\Services\\");
	strcat(StrBuf, DriverName);
	uPrint("InstallDriver-4 %s %s\n", StrBuf, DriverName);
	// キーを生成またはオープンしてアクセス権を得る
	if( RegCreateKeyEx( HKEY_LOCAL_MACHINE, StrBuf, 0, "", 0, KEY_ALL_ACCESS,
						NULL, &mru, &disposition) != ERROR_SUCCESS)
	{
		AfxMessageBox("Could not create driver registry key");
		return FALSE;
	}
	// Delete ImagePath
	RegDeleteValue(mru,"ImagePath");		// ImagePathキーを削除
	// Delete DisplayName
	RegDeleteValue(mru,"DisplayName");		// DisplayNameキーを削除する
	// ErrorControl
	dwRegValue = SERVICE_ERROR_NORMAL;		// ErrorControlキーをセットする
	if( RegSetValueEx(mru,"ErrorControl",0,REG_DWORD,(CONST BYTE*)&dwRegValue,sizeof(DWORD))
		!= ERROR_SUCCESS)
	{
		AfxMessageBox("Could not create driver registry value ErrorControl");
		return FALSE;
	}
	// Start
	dwRegValue = SERVICE_AUTO_START;		// Startキーをセットする
	if( RegSetValueEx(mru,"Start",0,REG_DWORD,(CONST BYTE*)&dwRegValue,sizeof(DWORD))
		!= ERROR_SUCCESS)
	{
		AfxMessageBox("Could not create driver registry value Start");
		return FALSE;
	}
	// Type
	dwRegValue = SERVICE_KERNEL_DRIVER;		// Typeキーをセットする
	if( RegSetValueEx(mru,"Type",0,REG_DWORD,(CONST BYTE*)&dwRegValue,sizeof(DWORD))
		!= ERROR_SUCCESS)
	{
		AfxMessageBox("Could not create driver registry value Type");
		return FALSE;
	}

	RegCloseKey(mru);						// アクセス権を返上する

	uPrint("InstallDriver-4A %s\n", DriverName);
	/////////////////////////////////////////////////////////////////////////
	// Open EventLog\System registry key and set its values

	// キーを生成またはオープンしてアクセス権を得る
	if( RegCreateKeyEx( HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\System",
						0, "", 0, KEY_ALL_ACCESS, NULL, &mru, &disposition)
		!= ERROR_SUCCESS)
	{
		AfxMessageBox("Could not open EventLog\\System registry key");
		return FALSE;
	}
	// get Sources size
	DataSize = 0;
	// Sourcesキーの値を取得する、サイズのみ
	if( RegQueryValueEx(mru,"Sources",NULL,&Type,NULL,&DataSize)
		!= ERROR_SUCCESS)
	{
		AfxMessageBox("Could not read size of EventLog\\System registry value Sources");
		return FALSE;
	}
	// read Sources
	uPrint("InstallDriver-4B %s %x\n", DriverName, DataSize);
	DriverNameLen = strlen(DriverName);
	uPrint("InstallDriver-4C : %x\n", DriverNameLen);
	DataSize += DriverNameLen+1;
	uPrint("InstallDriver-4D %x\n", DataSize);
	sources = malloc(DataSize);
	rt = FALSE;
	if(sources == NULL){
		AfxMessageBox("Could not allocate PoolArea");
		goto ProcEnd;		// 確保したブロックを開放してからエラーリターン;
	}
	// Sourcesキーの値を取得する、中身を取得
	if( RegQueryValueEx(mru,"Sources",NULL,&Type, (LPBYTE)sources, &DataSize)
		!= ERROR_SUCCESS)
	{
	  AfxMessageBox("Could not read EventLog\\System registry value Sources");
		goto ProcEnd;		// return FALSE;
	}
	uPrint("DriverName= %s %x\n", DriverName, DataSize);
	// If driver not there, add and write
	if( FindInMultiSz(sources, DataSize, DriverName)==-1)
	{
		strcat(sources, DriverName);
		DataSize += DriverNameLen;

		if(RegSetValueEx(mru,"Sources",0,REG_MULTI_SZ,(CONST BYTE*)sources,DataSize)
			!= ERROR_SUCCESS)
		{
			AfxMessageBox("Could not create driver registry value Sources");
			goto ProcEnd;		// return FALSE;
		}
	}

	/////////////////////////////////////////////////////////////////////////
	// Create/Open EventLog\System\driver registry key and set its values

	strcpy(DriverFullPath, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\System\\");
	strcat(DriverFullPath, DriverName);
	if( RegCreateKeyEx( HKEY_LOCAL_MACHINE, DriverFullPath,
		0, "", 0, KEY_ALL_ACCESS, NULL, &mru, &disposition) != ERROR_SUCCESS)
	{
	  AfxMessageBox("Could not create EventLog\\System\\driver registry key");
		goto ProcEnd;		// return FALSE;
	}
	// TypesSupported
	dwRegValue = 7;
	if( RegSetValueEx(mru,"TypesSupported",0,REG_DWORD,(CONST BYTE*)&dwRegValue,sizeof(DWORD))
		!= ERROR_SUCCESS)
	{
		AfxMessageBox("Could not create EventLog\\System\\driver registry value TypesSupported");
		goto ProcEnd;		// return FALSE;
	}
	// EventMessageFile
	strcpy(EventMessageFile,"%SystemRoot%\\System32\\IoLogMsg.dll;%SystemRoot%\\System32\\Drivers\\");
	strcat(EventMessageFile, DriverName);
	strcat(EventMessageFile, ".sys");
	if( RegSetValueEx(mru,"EventMessageFile",0,REG_EXPAND_SZ,(CONST BYTE*)EventMessageFile,strlen(EventMessageFile)+1)
		!= ERROR_SUCCESS)
	{
		AfxMessageBox("Could not create EventLog\\System\\driver registry value EventMessageFile");
		goto ProcEnd;		// return FALSE;
	}
	RegCloseKey(mru);						// アクセス権を返上する

	/////////////////////////////////////////////////////////////////////////
	// Start driver service

	uPrint("DriverName: %s\n", DriverName);
	rt = StartDriver(DriverName);

ProcEnd:
	free(sources);
	uPrint("InstallDriver-OUT %x\n", rt);
	return rt;
}

/////////////////////////////////////////////////////////////////////////////

BOOL CreateDriver( const char * DriverName, const char * FullDriver)
{
	/////////////////////////////////////////////////////////////////////////
	// Open service control manager
	BOOL Stopped;
	SC_HANDLE hDriver;
	int seconds;
						// サービス制御マネージャの指定されたデータベースを開く
	SC_HANDLE hSCManager = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	uPrint("CreateDriver-IN %s   %s %x\n", DriverName, FullDriver, hSCManager);
	if( hSCManager==NULL)
	{
		AfxMessageBox("Could not open Service Control Manager");
		return FALSE;
	}

	/////////////////////////////////////////////////////////////////////////
	// If driver is running, stop it

	hDriver = OpenService(hSCManager,DriverName,SERVICE_ALL_ACCESS);
	if( hDriver!=NULL)
	{
		SERVICE_STATUS ss;
		if( ControlService(hDriver,SERVICE_CONTROL_INTERROGATE,&ss))
		{
			if( ss.dwCurrentState!=SERVICE_STOPPED)
			{
				if( !ControlService(hDriver,SERVICE_CONTROL_STOP,&ss))
				{
					AfxMessageBox("Could not stop driver");
					CloseServiceHandle(hDriver);
					CloseServiceHandle(hSCManager);
					return FALSE;
				}
				// Give it 10 seconds to stop
				Stopped = FALSE;
				for(seconds=0;seconds<10;seconds++)
				{
					Sleep(1000);
					if( ControlService(hDriver,SERVICE_CONTROL_INTERROGATE,&ss)==0 ||
						ss.dwCurrentState==SERVICE_STOPPED)
					{
						Stopped = TRUE;
						break;
					}
				}
				if( !Stopped)
				{
					AfxMessageBox("Could not stop driver");
					CloseServiceHandle(hDriver);
					CloseServiceHandle(hSCManager);
					return FALSE;
				}
			}
		}
		CloseServiceHandle(hDriver);
		CloseServiceHandle(hSCManager);
		return TRUE;
	}

	/////////////////////////////////////////////////////////////////////////
	// Create driver service

	hDriver = CreateService(hSCManager,DriverName,DriverName,SERVICE_ALL_ACCESS,
			  SERVICE_KERNEL_DRIVER,SERVICE_AUTO_START,SERVICE_ERROR_NORMAL,
			  FullDriver,NULL,NULL,NULL,NULL,NULL);
	if( hDriver==NULL)
	{
		AfxMessageBox("Could not install driver with Service Control Manager");
		CloseServiceHandle(hSCManager);
		return FALSE;
	}

	/////////////////////////////////////////////////////////////////////////
	CloseServiceHandle(hDriver);
	CloseServiceHandle(hSCManager);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

BOOL StartDriver( const char * DriverName)
{
	/////////////////////////////////////////////////////////////////////////
	// Open service control manager
	SC_HANDLE hDriver;
	SERVICE_STATUS ss;
	BOOL Started;
	int seconds;

	SC_HANDLE hSCManager = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	uPrint("StartDriver-IN %x %s\n", hSCManager, DriverName);
	if( hSCManager==NULL)
	{
		uPrint("%x\n", GetLastError());
		AfxMessageBox("Could not open Service Control Manager");
		return FALSE;
	}

	///////// サービス制御マネージャのデータベースハンドルを開く ///////////

	hDriver = OpenService(hSCManager,DriverName,SERVICE_ALL_ACCESS);
	if( hDriver==NULL)
	{
		uPrint("%x\n", GetLastError());
		AfxMessageBox("Could not open driver service");
		CloseServiceHandle(hSCManager);
		return FALSE;
	}

		// サービスが自らの現在のステータス情報をサービスマネージャに伝えて、
		// 情報を更新するよう要求する。
	if( ControlService(hDriver,SERVICE_CONTROL_INTERROGATE,&ss))
	{
		if( ss.dwCurrentState==SERVICE_RUNNING)
		{
			CloseServiceHandle(hDriver);
			CloseServiceHandle(hSCManager);
			return TRUE;
		}
	}
	else if( GetLastError()!=ERROR_SERVICE_NOT_ACTIVE)
	{
		AfxMessageBox("Could not interrogate driver service");
		CloseServiceHandle(hDriver);
		CloseServiceHandle(hSCManager);
		return FALSE;
	}
	if( !StartService(hDriver,0,NULL))	// サービスを開始する
	{
		AfxMessageBox("Could not start driver");
		CloseServiceHandle(hDriver);
		CloseServiceHandle(hSCManager);
		return FALSE;
	}
	// Give it 10 seconds to start
	Started = FALSE;
	for(seconds=0; seconds<10; seconds++)
	{
		Sleep(1000);
		if( ControlService(hDriver,SERVICE_CONTROL_INTERROGATE,&ss) &&
			ss.dwCurrentState==SERVICE_RUNNING)
		{
			Started = TRUE;
			break;
		}
	}
	if( !Started)
	{
		AfxMessageBox("Could not start driver");
		CloseServiceHandle(hDriver);
		CloseServiceHandle(hSCManager);
		return FALSE;
	}
	CloseServiceHandle(hDriver);
	CloseServiceHandle(hSCManager);
	return TRUE;
}

/****************************************************************************
*
*    FUNCTION: UnInstallDriver( const TCHAR *Name)
*
*    Name:    ドライバ名
*
*    PURPOSE:  Stops the driver and has the configuration manager unload it.
*
****************************************************************************/

BOOL UnInstallDriver( const TCHAR * Name )
{
    SC_HANDLE       schSCManager;
    BOOL bResult;

    schSCManager = OpenSCManager( NULL,			// machine (NULL == local)
                                  NULL,			// database (NULL == default)
                                  SC_MANAGER_ALL_ACCESS // access required
                                );
	uPrint("UnInstallDriver-IN %s %x\n", Name, schSCManager);

    bResult = StopDriver( schSCManager, Name );	// ドライバのサービスを停止する
	uPrint("UnInstallDriver-2 %x\n", bResult);

    bResult = RemoveDriver( schSCManager, Name );// データベースから削除する

    CloseServiceHandle( schSCManager );			// オブジェクトハンドルを閉じる

	uPrint("UnInstallDriver-OUT %x\n", bResult);
    return bResult;
}

/****************************************************************************
*
*    FUNCTION: StopDriver( IN SC_HANDLE, IN LPCTSTR)
*
*    PURPOSE: Has the configuration manager stop the driver (unload it)
*
****************************************************************************/

BOOL StopDriver( IN SC_HANDLE SchSCManager, IN LPCTSTR DriverName )
{
    SC_HANDLE       schService;
    BOOL            bResult;
    SERVICE_STATUS  serviceStatus;

    schService = OpenService( SchSCManager, DriverName, SERVICE_ALL_ACCESS );
	uPrint("StopDriver-IN %x %s %x\n", SchSCManager, DriverName, schService);
    if ( schService == NULL ){
    	uPrint("LastError=%x\n", GetLastError());
        return FALSE;
	}
								// サービスを停止するように要求する
    bResult = ControlService(schService, SERVICE_CONTROL_STOP, &serviceStatus);

    CloseServiceHandle( schService );	// オブジェクトハンドルを閉じる

	uPrint("StopDriver-OUT %x\n", bResult);
    return bResult;
}

/****************************************************************************
*
*    FUNCTION: RemoveDriver( IN SC_HANDLE, IN LPCTSTR)
*
*    PURPOSE: Deletes the driver service.
*
****************************************************************************/

BOOL RemoveDriver( IN SC_HANDLE SchSCManager, IN LPCTSTR DriverName )
{
    SC_HANDLE  schService;
    BOOL       bResult;

	uPrint("RemoveDriver-IN %x %s\n", SchSCManager, DriverName);
    schService = OpenService( SchSCManager,
                              DriverName,
                              SERVICE_ALL_ACCESS
                              );

    if ( schService == NULL )
        return FALSE;

    bResult = DeleteService( schService );	// サービス制御マネージャのデータ
    										// ベースから削除する
    CloseServiceHandle( schService );		// オブジェクトハンドルを閉じる

	uPrint("RemoveDriver-OUT %x\n", bResult);
    return bResult;
}

////////////////// 文字列を検索する ////////////////////////////////////////
//	Try to find Match in MultiSz, including Match's terminating \0

int FindInMultiSz(LPTSTR MultiSz, int MultiSzLen, const char * Match)
{
	int i, j;
	int MatchLen = strlen(Match);
	TCHAR FirstChar = *Match;
	for(i=0; i<MultiSzLen-MatchLen; i++)
	{
		if( *MultiSz++ == FirstChar)
		{
			BOOL Found = TRUE;
			LPTSTR Try = MultiSz;
			for(j=1;j<=MatchLen;j++)
				if( *Try++ != Match[j])
				{
					Found = FALSE;
					break;
				}
			if( Found)
				return i;
		}
	}
	return -1;
}

///////////////////////// end of file ////////////////////////////////
