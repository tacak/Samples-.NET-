// AtlCtrl.cpp : CAtlCtrl の実装
#include "stdafx.h"
#include "AtlCtrl.h"


// CAtlCtrl

LRESULT CAtlCtrl::OnLButtonDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。
	::MessageBox(NULL,L"Test Message", L"info", MB_OK);

	return 0;
}
