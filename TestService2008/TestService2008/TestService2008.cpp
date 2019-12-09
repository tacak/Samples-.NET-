// TestService2008.cpp : WinMain �̎���


#include "stdafx.h"
#include "resource.h"
#include "TestService2008_i.h"

#include <stdio.h>

class CTestService2008Module : public CAtlServiceModuleT< CTestService2008Module, IDS_SERVICENAME >
{
private:
	UINT_PTR m_timer;
public :
	DECLARE_LIBID(LIBID_TestService2008Lib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_TESTSERVICE2008, "{08CBACEC-D248-4017-B961-85755221650E}")
	HRESULT InitializeSecurity() throw()
	{
		// TODO : CoInitializeSecurity ���Ăяo���A�T�[�r�X�ɓK�؂ȃZ�L�����e�B�ݒ�� 
		// �w�肵�܂��B
		// ���� - PKT ���x���̔F�؁A 
		// RPC_C_IMP_LEVEL_IDENTIFY �̋U�����x���A 
		// ����ѓK�؂� NULL �ȊO�̃Z�L�����e�B�L�q�q�B

		return S_OK;
	}
	HRESULT PreMessageLoop(int nShowCmd);
	void RunMessageLoop() throw();
};

CTestService2008Module _AtlModule;


//
extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
                                LPTSTR /*lpCmdLine*/, int nShowCmd)
{
    return _AtlModule.WinMain(nShowCmd);
}


HRESULT CTestService2008Module::PreMessageLoop(int nShowCmd)
{
	// TODO: �����ɓ���ȃR�[�h��ǉ����邩�A�������͊�{�N���X���Ăяo���Ă��������B
	HRESULT hr = CAtlServiceModuleT<CTestService2008Module,100>::PreMessageLoop(nShowCmd);

#if _ATL_VER >= 0x0700
    if (SUCCEEDED(hr) && !m_bDelayShutdown) {
      hr = CoResumeClassObjects();
    }
#endif
    if (SUCCEEDED(hr)) {
      m_timer = SetTimer(NULL, NULL, 3000, NULL);
    }
    return hr;

}

void CTestService2008Module::RunMessageLoop() throw()
{
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0) > 0)
	{
	    if (msg.message == WM_TIMER) MessageBeep(MB_OK);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
