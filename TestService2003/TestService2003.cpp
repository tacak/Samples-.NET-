// TestService2003.cpp : WinMain �̎���

#include "stdafx.h"
#include "resource.h"

// ���̃��W���[�������́AWinMain �������I�Ɏ������܂��B
[ module(SERVICE, uuid = "{BAB61CF1-D683-44E1-B976-1C0DA6380EC0}", 
		 name = "TestService2003", 
		 helpstring = "TestService2003 1.0 �^�C�v ���C�u����", 
		 resource_name="IDS_SERVICENAME") ]
class CTestService2003Module : public CAtlServiceModuleT 
{
public:
	HRESULT InitializeSecurity() throw()
	{
		// TODO : CoInitializeSecurity ���Ăяo���A�T�[�r�X�ɓK�؂ȃZ�L�����e�B�ݒ��
		//�w�肵�܂��B
		// ���� - PKT ���x���̔F�؁A
		// RPC_C_IMP_LEVEL_IDENTIFY �̋U�����x���A
		// ����ѓK�؂� Null �ȊO�̃Z�L�����e�B�L�q�q�B

		return S_OK;
	}
};

