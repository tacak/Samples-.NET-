// AtlCtrl.cpp : CAtlCtrl �̎���
#include "stdafx.h"
#include "AtlCtrl.h"


// CAtlCtrl

LRESULT CAtlCtrl::OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B
	::MessageBox(NULL,L"Test Message", L"info", MB_OK);

	return 0;
}
