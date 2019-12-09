// SkillCountDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "SkillCount.h"
#include "SkillCountDlg.h"
#include ".\skillcountdlg.h"

#pragma comment(lib, "keyHook.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// �A�v���P�[�V�����̃o�[�W�������Ɏg���� CAboutDlg �_�C�A���O

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

// ����
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


// CSkillCountDlg �_�C�A���O



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


// CSkillCountDlg ���b�Z�[�W �n���h��

BOOL CSkillCountDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// "�o�[�W�������..." ���j���[���V�X�e�� ���j���[�ɒǉ����܂��B

	// IDM_ABOUTBOX �́A�V�X�e�� �R�}���h�͈͓̔��ɂȂ���΂Ȃ�܂���B
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

	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�

	// TODO: �������������ɒǉ����܂��B
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

	return TRUE;  // �t�H�[�J�X���R���g���[���ɐݒ肵���ꍇ�������ATRUE ��Ԃ��܂��B
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

// �_�C�A���O�ɍŏ����{�^����ǉ�����ꍇ�A�A�C�R����`�悷�邽�߂�
//  ���̃R�[�h���K�v�ł��B�h�L�������g/�r���[ ���f�����g�� MFC �A�v���P�[�V�����̏ꍇ�A
//  ����́AFramework �ɂ���Ď����I�ɐݒ肳��܂��B

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
		CPaintDC dc(this); // �`��̃f�o�C�X �R���e�L�X�g

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// �N���C�A���g�̎l�p�`�̈���̒���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �A�C�R���̕`��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���[�U�[���ŏ��������E�B���h�E���h���b�O���Ă���Ƃ��ɕ\������J�[�\�����擾���邽�߂ɁA
//  �V�X�e�������̊֐����Ăяo���܂��B
HCURSOR CSkillCountDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSkillCountDlg::OnBnClickedOk()
{
	// TODO : �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B
	//OnOK();
}

void CSkillCountDlg::OnBnClickedCancel()
{
	// TODO : �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B
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