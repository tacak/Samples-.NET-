// SkillCount.h : PROJECT_NAME �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C���ł��B
//

#pragma once

#ifndef __AFXWIN_H__
	#error ���̃t�@�C���� PCH �Ɋ܂߂�O�ɁA'stdafx.h' ���܂߂Ă��������B
#endif

#include "resource.h"		// ���C�� �V���{��


// CSkillCountApp:
// ���̃N���X�̎����ɂ��ẮASkillCount.cpp ���Q�Ƃ��Ă��������B
//

class CSkillCountApp : public CWinApp
{
public:
	CSkillCountApp();

// �I�[�o�[���C�h
	public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
};

extern CSkillCountApp theApp;
