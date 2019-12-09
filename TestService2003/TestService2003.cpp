// TestService2003.cpp : WinMain の実装

#include "stdafx.h"
#include "resource.h"

// このモジュール属性は、WinMain を自動的に実装します。
[ module(SERVICE, uuid = "{BAB61CF1-D683-44E1-B976-1C0DA6380EC0}", 
		 name = "TestService2003", 
		 helpstring = "TestService2003 1.0 タイプ ライブラリ", 
		 resource_name="IDS_SERVICENAME") ]
class CTestService2003Module : public CAtlServiceModuleT 
{
public:
	HRESULT InitializeSecurity() throw()
	{
		// TODO : CoInitializeSecurity を呼び出し、サービスに適切なセキュリティ設定を
		//指定します。
		// 推奨 - PKT レベルの認証、
		// RPC_C_IMP_LEVEL_IDENTIFY の偽装レベル、
		// および適切な Null 以外のセキュリティ記述子。

		return S_OK;
	}
};

