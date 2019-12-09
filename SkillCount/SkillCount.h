// SkillCount.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error このファイルを PCH に含める前に、'stdafx.h' を含めてください。
#endif

#include "resource.h"		// メイン シンボル


// CSkillCountApp:
// このクラスの実装については、SkillCount.cpp を参照してください。
//

class CSkillCountApp : public CWinApp
{
public:
	CSkillCountApp();

// オーバーライド
	public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CSkillCountApp theApp;
