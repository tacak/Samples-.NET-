

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* at Thu Feb 28 23:14:23 2008
 */
/* Compiler settings for .\ATL_ActiveX.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __ATL_ActiveX_h__
#define __ATL_ActiveX_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IAtlCtrl_FWD_DEFINED__
#define __IAtlCtrl_FWD_DEFINED__
typedef interface IAtlCtrl IAtlCtrl;
#endif 	/* __IAtlCtrl_FWD_DEFINED__ */


#ifndef __AtlCtrl_FWD_DEFINED__
#define __AtlCtrl_FWD_DEFINED__

#ifdef __cplusplus
typedef class AtlCtrl AtlCtrl;
#else
typedef struct AtlCtrl AtlCtrl;
#endif /* __cplusplus */

#endif 	/* __AtlCtrl_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IAtlCtrl_INTERFACE_DEFINED__
#define __IAtlCtrl_INTERFACE_DEFINED__

/* interface IAtlCtrl */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IAtlCtrl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9878F618-39D0-4109-8523-C0E65356EA2D")
    IAtlCtrl : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IAtlCtrlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAtlCtrl * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAtlCtrl * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAtlCtrl * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IAtlCtrl * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IAtlCtrl * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IAtlCtrl * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IAtlCtrl * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IAtlCtrlVtbl;

    interface IAtlCtrl
    {
        CONST_VTBL struct IAtlCtrlVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAtlCtrl_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAtlCtrl_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAtlCtrl_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAtlCtrl_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAtlCtrl_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAtlCtrl_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAtlCtrl_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAtlCtrl_INTERFACE_DEFINED__ */



#ifndef __ATL_ActiveXLib_LIBRARY_DEFINED__
#define __ATL_ActiveXLib_LIBRARY_DEFINED__

/* library ATL_ActiveXLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_ATL_ActiveXLib;

EXTERN_C const CLSID CLSID_AtlCtrl;

#ifdef __cplusplus

class DECLSPEC_UUID("304B8BC2-1942-44AD-BE19-29666B1905DC")
AtlCtrl;
#endif
#endif /* __ATL_ActiveXLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


