#include <stdio.h>
#include <windows.h>

#import "ClassLibrary1.tlb" no_namespace named_guids raw_interfaces_only

void main(void)
{
	CoInitialize(NULL);

//	_Class1 *pClass1;
//	HRESULT hResult;

	long n;
	//_Class1Ptr pp;
	//pp->TestFunc2(10, &n);

	_Class1Ptr cS(__uuidof(Class1));
//	cS->TestFunc2(5, &n);

//	SAFEARRAY *sa;


	SAFEARRAY* pSa;
	wchar_t **str;

	HRESULT ret;

	BSTR hikisu = SysAllocString(L"uga");
	ret = cS->TestFunc(hikisu, &pSa);
	//ret = cS->GetStr(&pSa);
	if( FAILED(ret) ){
		wprintf(L"%X\n", ret);
		return;
	}

	long idx[1];
	//要素位置[0][1]への値セット
	idx[0] = 1;
	SafeArrayPtrOfIndex(pSa, idx, (void HUGEP* FAR*)&str);

	//SafeArrayAccessData( pSa, (void**)&iArray);  // iArrayに戻り値が入っているのを確認。

	wprintf(L"%s\n", *str);

	SafeArrayUnaccessData( pSa );
	SafeArrayDestroy( pSa );

	return;

//	cS->TestFunc3(10, &sa);
/*
	int *idata;
	long idx[2];
	
	wprintf(L"sa=%p\n", sa);

//	HRESULT hr = SafeArrayLock(sa);
//	 if( FAILED(hr) ) return;

	//要素位置[0][1]への値セット
	idx[0] = 0;
	idx[1] = 0;
	SafeArrayPtrOfIndex(sa, idx, (void HUGEP* FAR*)&idata);
	wprintf(L"%d", *idata);
	idx[0] = 1;
	idx[1] = 0;
	SafeArrayPtrOfIndex(sa, idx, (void HUGEP* FAR*)&idata);
	wprintf(L"%d", *idata);
	idx[0] = 2;
	idx[1] = 0;
	SafeArrayPtrOfIndex(sa, idx, (void HUGEP* FAR*)&idata);
	wprintf(L"%d", *idata);
*/
	/*
	cS->TestFunc(L"hoge", &sa);

	int *idata;
	long idx[1];
	
	wprintf(L"sa=%p\n", sa);

//	HRESULT hr = SafeArrayLock(sa);
//	 if( FAILED(hr) ) return;

	//要素位置[0][1]への値セット
	idx[0] = 0;
	idx[1] = 0;
	SafeArrayPtrOfIndex(sa, idx, (void HUGEP* FAR*)&idata);
	wprintf(L"%d", *idata);
	idx[0] = 1;
	idx[1] = 0;
	SafeArrayPtrOfIndex(sa, idx, (void HUGEP* FAR*)&idata);
	wprintf(L"%d", *idata);
	idx[0] = 2;
	idx[1] = 0;
	SafeArrayPtrOfIndex(sa, idx, (void HUGEP* FAR*)&idata);
	wprintf(L"%d", *idata);

	//SafeArrayUnlock(sa);

	return;
*/
	//	ptr.CreateInstance((REFCLSID) CLSID_Class1, 0, CLSCTX_INPROC_SERVER,
//                                 (REFIID)   IID__Class1, (LPVOID*)&pClass1);
//
//	hResult = ::CoCreateInstance((REFCLSID) CLSID_Class1, 0, CLSCTX_INPROC_SERVER,
//                                 (REFIID)   IID__Class1, (LPVOID*)&pClass1);


	SAFEARRAY *psa;
	HRESULT hr = cS->TestFunc(L"hoge", &psa);
	if( FAILED(hr) ){
		wprintf(L"%X\n", hr);
		return;
	}

	wprintf(L"psa=%p\n", psa);
	
	hr = SafeArrayLock(psa);
	if( FAILED(hr) ){
		wprintf(L"%X\n", hr);
		return;
	}
	 
	 //データの操作
	 long *pdata;
	 long indices[2];
	 
	 //要素位置[0][1]への値セット
	 indices[0] = 0;
	 indices[1] = 0;
	 SafeArrayPtrOfIndex(psa, indices, (void HUGEP* FAR*)&pdata);
	 wprintf(L"%s\n", (wchar_t *)pdata);
	 
	 //要素位置[1][2]への値セット
	 indices[0] = 1;
	 indices[1] = 0;
	 SafeArrayPtrOfIndex(psa, indices, (void HUGEP* FAR*)&pdata);
	 wprintf(L"%s\n", *pdata);

	 //配列のアンロック
	 SafeArrayUnlock(psa);

	CoUninitialize();

	//wchar_t **wc = TestFunction();
	//or(int i = 0; i < 3; i++){
	//	wprintf(L"Message%d : %s\n", i+1, wc[i]);
	//}
}