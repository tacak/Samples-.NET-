========================================================================
ATL (ACTIVE TEMPLATE LIBRARY) : ATL_ActiveX �v���W�F�N�g�̊T�v
========================================================================

���� ATL_ActiveX �v���W�F�N�g�́A���[�U�[���_�C�i�~�b�N �����N ���C
�u���� (DLL: Dynamic Link Library) ���쐬���邽�߂̊J�n�_�Ƃ��Ďg�p���邽�߂ɁA
AppWizard �ɂ���č쐬����܂����B


���̃t�@�C���ɂ́A�v���W�F�N�g���\������e�t�@�C���̓��e�̊T�����L�q����Ă���
���B

ATL_ActiveX.vcproj
    ����́A�A�v���P�[�V���� �E�B�U�[�h�Ő�������� VC++ �v���W�F�N�g�̃��C����
    �v���W�F�N�g �t�@�C���ł��B
    �t�@�C���𐶐����� Visual C++ �̃o�[�W�������ƁA�A�v���P�[�V���� �E�B�U�[
    �h�őI�������v���b�g�t�H�[���A�\���A����уv���W�F�N�g�̋@�\�Ɋւ�����
    �L�q����Ă��܂��B

ATL_ActiveX.idl
    ���̃t�@�C���ɂ́A�v���W�F�N�g�Œ�`�����^�C�v ���C�u������ IDL ��`�A
    �C���^�[�t�F�C�X�A����уR�N���X���܂܂�܂��B
    ���̃t�@�C���� MIDL �R���p�C���ɂ���ď�������A���̂��̂���������܂��B
        C++ �̃C���^�[�t�F�C�X��`�� GUID �錾 (ATL_ActiveX.h)
        GUID ��`                              (ATL_ActiveX_i.c)
        �^�C�v ���C�u����                      (ATL_ActiveX.tlb)
        �}�[�V�������O �R�[�h                  (ATL_ActiveX_p.c ����� dlldata.c)

ATL_ActiveX.h
    ���̃t�@�C���ɂ́AC++ �̃C���^�[�t�F�C�X��`����� 
    ATL_ActiveX.idl. �Œ�`����鍀�ڂ� GUID �錾���܂܂�܂��B
    �R���p�C������ MIDL �ɂ���čĐ�������܂��B

ATL_ActiveX.cpp
    ���̃t�@�C���ɂ́A�I�u�W�F�N�g �}�b�v����� DLL �̃G�N�X�|�[�g�̎�����
    �܂܂�܂��B

ATL_ActiveX.rc
    ����́A�v���O�����Ŏg�p���� Microsoft Windows �̑S���\�[�X�̈ꗗ�ł��B

ATL_ActiveX.def
    ���̃��W���[����`�t�@�C���́ADLL �ɕK�v�ȃG�N�X�|�[�g�Ɋւ�����������J
    �ɒ񋟂��܂��B���̃G�N�X�|�[�g��񂪊܂܂�Ă��܂��B
        DllGetClassObject  
        DllCanUnloadNow    
        GetProxyDllInfo    
        DllRegisterServer
        DllUnregisterServer

/////////////////////////////////////////////////////////////////////////////
���̑��̕W���t�@�C��:

StdAfx.h, StdAfx.cpp
    �����̃t�@�C���́A�R���p�C���ς݃w�b�_�[ (PCH) �t�@�C�� 
    ATL_ActiveX.pch �ƃv���R���p�C���ς݌^�t�@�C�� StdAfx.obj 
    ���r���h���邽�߂Ɏg�p���܂��B

Resource.h
    ����́A���\�[�X ID ���`����W���̃w�b�_�[ �t�@�C���ł��B

/////////////////////////////////////////////////////////////////////////////
�v���L�V/�X�^�u DLL �v���W�F�N�g����у��W���[����`�t�@�C��:

ATL_ActiveXps.vcproj
    ���̃t�@�C���́A�K�v�ɉ����ăv���L�V/�X�^�u DLL ���\�z���邽�߂̃v���W�F�N
    �g �t�@�C���ł��B
        ���C�� �v���W�F�N�g�� IDL �t�@�C���ɃC���^�[�t�F�C�X�����Ȃ��Ƃ� 1 ��
        �܂܂�Ă��邱�ƁA����уv���L�V/�X�^�u DLL ���r���h����O�� IDL 
        �t�@�C�����R���p�C�����邱�Ƃ��K�v�ł��B���̃v���Z�X�ɂ���āA
        dlldata.c, �v���L�V/�X�^�u DLL ���r���h���邽�߂ɕK�v�� 
	ATL_ActiveX_i.c �� ATL_ActiveX_p.c ����������
        �܂��B

ATL_ActiveXps.def
    ���̃��W���[����`�t�@�C���́A�v���L�V/�X�^�u�ɕK�v�ȃG�N�X�|�[�g�Ɋւ���
    ���������J�ɒ񋟂��܂��B

/////////////////////////////////////////////////////////////////////////////




