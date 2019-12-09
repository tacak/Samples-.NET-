// SkillCountDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "SkillCount.h"
#include "SkillCountDlg.h"
#include ".\skillcountdlg.h"

#pragma comment(lib, "keyHook.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// アプリケーションのバージョン情報に使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ダイアログ データ
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

// 実装
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CSkillCountDlg ダイアログ



CSkillCountDlg::CSkillCountDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSkillCountDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSkillCountDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, prog1);
	DDX_Control(pDX, IDC_PROGRESS2, prog2);
	DDX_Control(pDX, IDC_PROGRESS3, prog3);
	DDX_Control(pDX, IDC_PROGRESS4, prog4);
	DDX_Control(pDX, IDC_PROGRESS5, prog5);
	DDX_Control(pDX, IDC_PROGRESS6, prog6);
	DDX_Control(pDX, IDC_EDIT1, edit1);
	DDX_Control(pDX, IDC_EDIT2, edit2);
	DDX_Control(pDX, IDC_EDIT3, edit3);
	DDX_Control(pDX, IDC_EDIT4, edit4);
	DDX_Control(pDX, IDC_EDIT5, edit5);
	DDX_Control(pDX, IDC_EDIT6, edit6);
}

BEGIN_MESSAGE_MAP(CSkillCountDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CSkillCountDlg メッセージ ハンドラ

BOOL CSkillCountDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// "バージョン情報..." メニューをシステム メニューに追加します。

	// IDM_ABOUTBOX は、システム コマンドの範囲内になければなりません。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。
	int *pnt;
	HANDLE hMapFile;

	hMapFile = CreateFileMapping((HANDLE)0xFFFFFFFF, NULL, PAGE_READWRITE, 0, sizeof(int[6]), "CountObject");
	pnt = (int *) MapViewOfFile(hMapFile , FILE_MAP_WRITE , 0 , 0 , 0);
	*pnt = 0;
	*(pnt+1) = 0;
	*(pnt+2) = 0;
	*(pnt+3) = 0;
	*(pnt+4) = 0;
	*(pnt+5) = 0;

	prog1.SetRange(0, 240);
	prog2.SetRange(0, 240);
	prog3.SetRange(0, 90);
	prog4.SetRange(0, 90);
	prog5.SetRange(0, 180);
	prog6.SetRange(0, 60);
	edit1.SetWindowText("200");
	edit2.SetWindowText("0");
	edit3.SetWindowText("0");
	edit4.SetWindowText("0");
	edit5.SetWindowText("0");
	edit6.SetWindowText("0");

	CWinThread *hThread;
	hThread = AfxBeginThread(ThreadFunc, this); 

	SetHook();

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

void CSkillCountDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CSkillCountDlg::OnPaint() 
{
		//prog1.SetPos(150);
		//edit1.SetWindowText("150");
		//prog2.SetPos(30);
		//edit2.SetWindowText("30");
		//prog3.SetPos(30);
		//edit3.SetWindowText("30");
		//prog4.SetPos(30);
		//edit4.SetWindowText("30");
		//prog5.SetPos(30);
		//edit5.SetWindowText("30");
		//prog6.SetPos(30);
		//edit6.SetWindowText("30");

	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CSkillCountDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSkillCountDlg::OnBnClickedOk()
{
	// TODO : ここにコントロール通知ハンドラ コードを追加します。
	//OnOK();
}

void CSkillCountDlg::OnBnClickedCancel()
{
	// TODO : ここにコントロール通知ハンドラ コードを追加します。
	OnCancel();
}

UINT CSkillCountDlg::ThreadFunc(LPVOID lpParameter)
{
	HANDLE hMapFile;
	int cnt[6] = {0};
	int *pnt;
	char buf[10];

	CSkillCountDlg *sc = (CSkillCountDlg *)lpParameter;

	hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "CountObject");
	if (!hMapFile){
		return FALSE;
	}
	pnt = (int *)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(int[6]));

	while(1){
		sc->prog1.SetPos(*(pnt));
		sc->prog2.SetPos(*(pnt+1));
		sc->prog3.SetPos(*(pnt+2));
		sc->prog4.SetPos(*(pnt+3));
		sc->prog5.SetPos(*(pnt+4));
		//sc->prog6.SetPos(*(pnt+5));

		sprintf(buf, "%d", *pnt);
		sc->edit1.SetWindowText(buf);
		sprintf(buf, "%d", *(pnt+1));
		sc->edit2.SetWindowText(buf);
		sprintf(buf, "%d", *(pnt+2));
		sc->edit3.SetWindowText(buf);
		sprintf(buf, "%d", *(pnt+3));
		sc->edit4.SetWindowText(buf);
		sprintf(buf, "%d", *(pnt+4));
		sc->edit5.SetWindowText(buf);
		//sprintf(buf, "%d", *(pnt+5));
		//sc->edit6.SetWindowText(buf);

		if(*(pnt) > 0)
			*(pnt) -= 1;
		if(*(pnt+1) > 0)
			*(pnt+1) -= 1;
		if(*(pnt+2) > 0){
			if(*(pnt+2) == 10)
				MessageBeep(MB_ICONASTERISK);
			*(pnt+2) -= 1;
		}
		if(*(pnt+3) > 0){
			if(*(pnt+3) == 10)
				MessageBeep(MB_ICONASTERISK);
			*(pnt+3) -= 1;
		}
		if(*(pnt+4) > 0){
			if(*(pnt+4) == 10)
				MessageBeep(MB_ICONHAND);
			*(pnt+4) -= 1;
		}
		//if(*(pnt+5) > 0){
		//	if(*(pnt+5) == 10)
		//		MessageBeep(MB_ICONHAND);
		//	*(pnt+5) -= 1;
		//}

		Sleep(1000);
	}

	return TRUE;
}