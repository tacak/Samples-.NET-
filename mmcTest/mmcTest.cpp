// mmcTest.cpp : DLL エクスポートの実装です。

#include "stdafx.h"
#include "resource.h"

// このモジュール属性は、DllMain、DllRegisterServer および DllUnregisterServer を自動的に実装します。
[ module(dll, uuid = "{CB2F2CD7-4F80-4D64-9D37-8DB924BE8CB5}", 
		 name = "mmcTest", 
		 helpstring = "mmcTest 1.0 タイプ ライブラリ",
		 resource_name = "IDR_MMCTEST") ]
class CmmcTestModule
{
public:
// CAtlDllModuleT メンバのオーバーライド
};
		 
