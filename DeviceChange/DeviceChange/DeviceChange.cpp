// DeviceChange.cpp : �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include "DeviceChange.h"

#pragma comment(lib, "setupapi.lib")

#define MAX_LOADSTRING 100

// �O���[�o���ϐ�:
HINSTANCE hInst;								// ���݂̃C���^�[�t�F�C�X
TCHAR szTitle[MAX_LOADSTRING];					// �^�C�g�� �o�[�̃e�L�X�g
TCHAR szWindowClass[MAX_LOADSTRING];			// ���C�� �E�B���h�E �N���X��

// ���̃R�[�h ���W���[���Ɋ܂܂��֐��̐錾��]�����܂�:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

TCHAR FirstDriveFromMask (ULONG unitmask);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: �����ɃR�[�h��}�����Ă��������B
	MSG msg;
	HACCEL hAccelTable;

	// �O���[�o������������������Ă��܂��B
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_DEVICECHANGE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// �A�v���P�[�V�����̏����������s���܂�:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DEVICECHANGE));

	// ���C�� ���b�Z�[�W ���[�v:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  �֐�: MyRegisterClass()
//
//  �ړI: �E�B���h�E �N���X��o�^���܂��B
//
//  �R�����g:
//
//    ���̊֐�����юg�����́A'RegisterClassEx' �֐����ǉ����ꂽ
//    Windows 95 ���O�� Win32 �V�X�e���ƌ݊�������ꍇ�ɂ̂ݕK�v�ł��B
//    �A�v���P�[�V�������A�֘A�t����ꂽ
//    �������`���̏������A�C�R�����擾�ł���悤�ɂ���ɂ́A
//    ���̊֐����Ăяo���Ă��������B
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DEVICECHANGE));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_DEVICECHANGE);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   �֐�: InitInstance(HINSTANCE, int)
//
//   �ړI: �C���X�^���X �n���h����ۑ����āA���C�� �E�B���h�E���쐬���܂��B
//
//   �R�����g:
//
//        ���̊֐��ŁA�O���[�o���ϐ��ŃC���X�^���X �n���h����ۑ����A
//        ���C�� �v���O���� �E�B���h�E���쐬����ѕ\�����܂��B
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // �O���[�o���ϐ��ɃC���X�^���X�������i�[���܂��B

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  �֐�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  �ړI:  ���C�� �E�B���h�E�̃��b�Z�[�W���������܂��B
//
//  WM_COMMAND	- �A�v���P�[�V���� ���j���[�̏���
//  WM_PAINT	- ���C�� �E�B���h�E�̕`��
//  WM_DESTROY	- ���~���b�Z�[�W��\�����Ė߂�
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	TCHAR szBuffer[1024], szDosName[MAX_PATH], szPathName[5], szPathNameSlash[5];
	DWORD dCount;
	PDEV_BROADCAST_HDR lpdr;
	PDEV_BROADCAST_VOLUME lpdbv;
	HDEVINFO hDev;
	SP_DEVINFO_DATA dd;
	DEVINST dInst;

	switch (message)
	{
	case WM_DEVICECHANGE:
		if(wParam == DBT_DEVICEARRIVAL && lParam != 0){
			// �ڑ����ꂽ�f�o�C�X�̃f�o�C�X�^�C�v�𒲂ׂ�
			lpdr = (PDEV_BROADCAST_HDR)lParam;
			// �{�����[���^�C�v�ȊO�̏ꍇ�͏������s��Ȃ�
			if(lpdr->dbch_devicetype != DBT_DEVTYP_VOLUME)
				break;

			lpdbv = (PDEV_BROADCAST_VOLUME)lpdr;

			// �ǉ����ꂽ�h���C�u�̃p�X�����𓾂�
			_stprintf(szPathName, _T("%c:"), FirstDriveFromMask(lpdbv ->dbcv_unitmask));

			// ������ \ �L����t�������p�X�������쐬����
			_tcscpy(szPathNameSlash, szPathName);
			_tcscat(szPathNameSlash, _T("\\"));
			
			// �p�X�����̕ύX���ɂ��AWM_DEVICECHANGBE ���ʒm����邽�߁A
			// �ǉ����ꂽ�h���C�u�̃p�X������ Z: �������ꍇ�ɂ͏����ΏۊO�Ƃ���
			if(szPathName[0] == 'Z')
				break;

			// DOS �f�o�C�X���̎擾
			QueryDosDevice(szPathName, szDosName, MAX_PATH);

			// �C���X�g�[���ς݂̑S�Ẵf�o�C�X���Z�b�g�𓾂�
			hDev = SetupDiGetClassDevs(NULL, 0, 0, DIGCF_ALLCLASSES);
			dd.cbSize = sizeof(SP_DEVINFO_DATA);

			// �f�o�C�X����񋓂���
			for(int i = 0; SetupDiEnumDeviceInfo(hDev, i, &dd); i++){
				// �q�m�[�h�̏��𓾂�
				CM_Get_Child(&dInst, dd.DevInst, 0);
				// ���m�[�h�̏��𓾂�
				CM_Get_Child(&dInst, dInst, 0);

				// ���m�[�h�����݂���ꍇ�̏���
				if(dInst != NULL){
					// �f�o�C�X�̃I�u�W�F�N�g���𓾂�
					dCount = 4000;
					CM_Get_DevNode_Registry_Property(dInst, CM_DRP_PHYSICAL_DEVICE_OBJECT_NAME, NULL, (void *)szBuffer, &dCount, 0);

					// �����œ����l����̏����Ŏ擾���� DOS �f�o�C�X���Ɠ������ǂ����𔻒肷��
					if(_tcscmp(szDosName, szBuffer) == 0){
						// �f�o�C�X���𓾂�
						SetupDiGetDeviceRegistryProperty(hDev,&dd, SPDRP_DEVICEDESC, NULL, (BYTE *)szBuffer, 4000 ,0);
						// ���m�[�h�������A���f�o�C�X���� USB ��e�ʋL�����u�f�o�C�X�̏ꍇ�́AUSB �������Ɣ��f���A
						// �p�X�����̕ύX�������s��
						if(_tcscmp(szBuffer, _T("USB ��e�ʋL�����u�f�o�C�X")) == 0){

							// �p�X�����񂩂�A��ӂ̃{�����[�����𓾂�
							if(!GetVolumeNameForVolumeMountPoint(szPathNameSlash, szBuffer, 4000)){
								_stprintf(szBuffer, _T("GetVolumeNameForVolumeMountPoint - ErrorCode ( %d )"), GetLastError());
								MessageBox(hWnd, szBuffer, _T("Error"), MB_OK);
								break;
							}

							// �p�X������Ɋ��蓖�Ă��Ă���{�����[�����A���}�E���g���܂�
							if(!DeleteVolumeMountPoint(szPathNameSlash)){
								_stprintf(szBuffer, _T("DeleteVolumeMountPoint - ErrorCode ( %d )"), GetLastError());
								MessageBox(hWnd, szBuffer, _T("Error"), MB_OK);
								break;
							}

							// �V�����p�X�������w�肵�A�{�����[�����}�E���g���܂�
							if(!SetVolumeMountPoint(_T("Z:\\"), szBuffer)){
								_stprintf(szBuffer, _T("SetVolumeMOuntPoint - ErrorCode ( %d )"), GetLastError());
								MessageBox(hWnd, szBuffer, _T("Error"), MB_OK);
								break;
							}

							// SUBST ���Ăяo���p�^�[���𗘗p����ꍇ�́A�ȉ��̎����̃R�����g�A�E�g���������A
							// ��� GetVolumeNameForVolumeMountPoint �` SetVolumeMountPoint �̈�A�̎������폜����
							/*
							// ��U���蓖�Ă���������
							_stprintf(szBuffer, _T("SUBST Z: %s\\ /D"), drive);
							_tsystem(szBuffer);

							// ���蓖�Ă�
							_stprintf(szBuffer, _T("SUBST Z: %s\\"), drive);
							_tsystem(szBuffer);
							*/
							MessageBox(hWnd, _T("USB ���������ڑ����ꂽ���߁A�p�X���� Z: ���Ċ��蓖�Ă��܂���"), _T("Infomation"), MB_OK);
						}
					}
				}			
			}
		}
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// �I�����ꂽ���j���[�̉��:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: �`��R�[�h�������ɒǉ����Ă�������...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// �o�[�W�������{�b�N�X�̃��b�Z�[�W �n���h���ł��B
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

TCHAR FirstDriveFromMask (ULONG unitmask)
{
     TCHAR i;

      for (i = 0; i < 26; ++i)
      {
         if (unitmask & 0x1)
            break;
         unitmask = unitmask >> 1;
      }

      return (i + _T('A'));

}