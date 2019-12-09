// TestService.cpp : WinMain の実装


#include "stdafx.h"
#include "resource.h"
#include "TestService_i.h"

#include <stdio.h>

class CTestServiceModule : public CAtlServiceModuleT< CTestServiceModule, IDS_SERVICENAME >
{
public :
	DECLARE_LIBID(LIBID_TestServiceLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_TESTSERVICE, "{DB005CBD-CEF2-483F-8F2E-35CE6CE07075}")
	HRESULT InitializeSecurity() throw()
	{
		// TODO : CoInitializeSecurity を呼び出し、サービスに適切なセキュリティ設定を 
		// 指定します。
		// 推奨 - PKT レベルの認証、 
		// RPC_C_IMP_LEVEL_IDENTIFY の偽装レベル、 
		// および適切な NULL 以外のセキュリティ記述子。

		return S_OK;
	}
	void ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
	HRESULT PreMessageLoop(int nShowCmd);
	int WinMain(int nShowCmd);
	HRESULT PostMessageLoop();
	HRESULT Run(int nShowCmd);
};

CTestServiceModule _AtlModule;



//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
                                LPTSTR /*lpCmdLine*/, int nShowCmd)
{
    return _AtlModule.WinMain(nShowCmd);
}


void CTestServiceModule::ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv)
{
	// TODO: ここに特定なコードを追加するか、もしくは基本クラスを呼び出してください。
	CAtlServiceModuleT<CTestServiceModule,100>::ServiceMain(dwArgc, lpszArgv);
}

HRESULT CTestServiceModule::PreMessageLoop(int nShowCmd)
{
	//Sleep(10000);

	// TODO: ここに特定なコードを追加するか、もしくは基本クラスを呼び出してください。
	return CAtlServiceModuleT<CTestServiceModule,100>::PreMessageLoop(nShowCmd);
}

int CTestServiceModule::WinMain(int nShowCmd)
{
	//Sleep(10000);
	// TODO: ここに特定なコードを追加するか、もしくは基本クラスを呼び出してください。
	return CAtlServiceModuleT<CTestServiceModule,100>::WinMain(nShowCmd);
}

HRESULT CTestServiceModule::PostMessageLoop()
{
	//Sleep(10000);

	// TODO: ここに特定なコードを追加するか、もしくは基本クラスを呼び出してください。
	return CAtlServiceModuleT<CTestServiceModule,100>::PostMessageLoop();
}

HRESULT CTestServiceModule::Run(int nShowCmd)
{
	//Sleep(10000);

	return S_OK;
}