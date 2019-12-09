// SkillCountDlg.h : ヘッダー ファイル
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "KeyHook.h"

// CSkillCountDlg ダイアログ
class CSkillCountDlg : public CDialog
{
// コンストラクション
public:
	CSkillCountDlg(CWnd* pParent = NULL);	// 標準コンストラクタ
	virtual ~CSkillCountDlg()
	{
		ResetHook();	
	}

// ダイアログ データ
	enum { IDD = IDD_SKILLCOUNT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート


// 実装
protected:
	static UINT ThreadFunc(LPVOID lpParameter);
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
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
