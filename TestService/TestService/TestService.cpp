// TestService.cpp : WinMain �̎���


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
		// TODO : CoInitializeSecurity ���Ăяo���A�T�[�r�X�ɓK�؂ȃZ�L�����e�B�ݒ�� 
		// �w�肵�܂��B
		// ���� - PKT ���x���̔F�؁A 
		// RPC_C_IMP_LEVEL_IDENTIFY �̋U�����x���A 
		// ����ѓK�؂� NULL �ȊO�̃Z�L�����e�B�L�q�q�B

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
	// TODO: �����ɓ���ȃR�[�h��ǉ����邩�A�������͊�{�N���X���Ăяo���Ă��������B
	CAtlServiceModuleT<CTestServiceModule,100>::ServiceMain(dwArgc, lpszArgv);
}

HRESULT CTestServiceModule::PreMessageLoop(int nShowCmd)
{
	//Sleep(10000);

	// TODO: �����ɓ���ȃR�[�h��ǉ����邩�A�������͊�{�N���X���Ăяo���Ă��������B
	return CAtlServiceModuleT<CTestServiceModule,100>::PreMessageLoop(nShowCmd);
}

int CTestServiceModule::WinMain(int nShowCmd)
{
	//Sleep(10000);
	// TODO: �����ɓ���ȃR�[�h��ǉ����邩�A�������͊�{�N���X���Ăяo���Ă��������B
	return CAtlServiceModuleT<CTestServiceModule,100>::WinMain(nShowCmd);
}

HRESULT CTestServiceModule::PostMessageLoop()
{
	//Sleep(10000);

	// TODO: �����ɓ���ȃR�[�h��ǉ����邩�A�������͊�{�N���X���Ăяo���Ă��������B
	return CAtlServiceModuleT<CTestServiceModule,100>::PostMessageLoop();
}

HRESULT CTestServiceModule::Run(int nShowCmd)
{
	//Sleep(10000);

	return S_OK;
}