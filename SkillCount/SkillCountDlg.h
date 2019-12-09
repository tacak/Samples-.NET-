// SkillCountDlg.h : �w�b�_�[ �t�@�C��
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "KeyHook.h"

// CSkillCountDlg �_�C�A���O
class CSkillCountDlg : public CDialog
{
// �R���X�g���N�V����
public:
	CSkillCountDlg(CWnd* pParent = NULL);	// �W���R���X�g���N�^
	virtual ~CSkillCountDlg()
	{
		ResetHook();	
	}

// �_�C�A���O �f�[�^
	enum { IDD = IDD_SKILLCOUNT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �T�|�[�g


// ����
protected:
	static UINT ThreadFunc(LPVOID lpParameter);
	HICON m_hIcon;

	// �������ꂽ�A���b�Z�[�W���蓖�Ċ֐�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CProgressCtrl prog1;
	CProgressCtrl prog2;
	CProgressCtrl prog3;
	CProgressCtrl prog4;
	CProgressCtrl prog5;
	CProgressCtrl prog6;
	CEdit edit1;
	CEdit edit2;
	CEdit edit3;
	CEdit edit4;
	CEdit edit5;
	CEdit edit6;
};
