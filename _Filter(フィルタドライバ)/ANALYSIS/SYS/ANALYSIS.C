/******************************************************************************
*
*	ANALYSIS.C - Windows XP Filter Driver Module 
*	指定したドライバのディスパッチ関数のアドレスを求め、機能別稼働回数をカウント
*   して、ドライバの概略機能を把握するドライバ
*
*	Copyright (c) 2002 AIdesign
*
*	PROGRAM: ANALYSIS.C           ANALYSIS.sys         \DosDevices\aCount
*
*	PURPOSE: Filter Driver 
*
******************************************************************************/

#include <wdm.h>
#include <stdio.h>
#include "analysis.h"
#include "devext.h"
#include "KdebugView.h"

#define	DOS_DEVICE_NAME	L"\\DosDevices\\aCount"	// ｱﾌﾟﾘｹｰｼｮﾝからのﾃﾞﾊﾞｲｽ名

ULONG		FunctionCount[IRP_MJ_MAXIMUM_FUNCTION+1];

wchar_t		DrvName[64]={0};					// 解析するドライバ名

NTSTATUS ReadDriverName(PUNICODE_STRING RegistryPath);
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject,PUNICODE_STRING RegistryPath);
NTSTATUS AcDeviceCreate( PDRIVER_OBJECT DriverObject );

#pragma alloc_text( INIT, ReadDriverName )
#pragma alloc_text( INIT, DriverEntry )
#pragma alloc_text( INIT, AcDeviceCreate )

NTSTATUS ReadDriverName(PUNICODE_STRING RegistryPath)
{							// レジストリを探索してフィルタ対象ドライバを決める
	NTSTATUS			Status;
	HANDLE				hKey;
	OBJECT_ATTRIBUTES	attributes;
	UNICODE_STRING		valname;
    ULONG           	length=0, ValueKey=0;
	PKEY_VALUE_FULL_INFORMATION vp;
	wchar_t				*pt;

    InitializeObjectAttributes(&attributes,				// ZwOpenKey関数が使う
                               RegistryPath,			// OBJECT_ATTRIBUTES
                               OBJ_CASE_INSENSITIVE,	// 構造体を初期化する
                               NULL,
                               NULL
                               );

    Status = ZwOpenKey(&hKey,					// 既存のレジストリキーを開く
                       KEY_READ,
                       &attributes
                       );

    if (!NT_SUCCESS(Status))
    {
    	ZwClose(hKey);							// 名前付きオブジェクトを閉じる
    	return Status;
    }

	RtlInitUnicodeString(&valname, L"DriverName");
	Status = ZwQueryValueKey(hKey, &valname, KeyValueFullInformation,
							NULL,
							0,					// ハンドルで指定されたキーの
							&length);			// 値を取得する

	if(Status==STATUS_BUFFER_TOO_SMALL){
		vp = (PKEY_VALUE_FULL_INFORMATION)ExAllocatePool(PagedPool, length);
		Status = ZwQueryValueKey(hKey, &valname, KeyValueFullInformation,
							vp,
							length,				// ハンドルで指定されたキーの
							&length);			// 値を取得する

		pt = (wchar_t *)((PUCHAR)vp + vp->DataOffset);
		RtlCopyMemory(DrvName, pt, 64);
    	ExFreePool(vp);							// 領域開放
	}
	else										// 暗黙ドライバを設定
		RtlCopyMemory(DrvName, L"\\Device\\Beep", 26);
   	ZwClose(hKey);								// 名前付きオブジェクトを閉じる
    return Status;
}

NTSTATUS
DriverEntry(			// Windowsからの唯一の入り口
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    )
{
    NTSTATUS	status;
    ULONG		i;

	OutputPort=0x3f8;			 // デバッグ情報出力先をシリアルポートに仮り決定
	ReadDebugPort(RegistryPath); // レジストリを探索してデバッグ出力先を決める
	ReadDriverName(RegistryPath);// レジストリを探索してフィルタ対象ドライバを決める

	kPrint("DriverEntry-IN,<ANALYSIS> V1.43 %x DrvName=%S\n", OutputPort, DrvName);
    status = AcDeviceCreate(DriverObject);	// DeviceObjectを生成し、下位ドライバに取り付ける
    if ( NT_SUCCESS(status) ) {
        //
        // Create dispatch points for create/open, close, unload.ディスパッチ関数を定義する
        //
        kPrint("Create dispatch points %d\n", IRP_MJ_MAXIMUM_FUNCTION);
        for(i=0; i<IRP_MJ_MAXIMUM_FUNCTION; ++i){
			DriverObject->MajorFunction[i]				= AnalysisCommon;
		}
			DriverObject->DriverExtension->AddDevice	= AnalysisAddDevice;
        	DriverObject->DriverUnload					= AnalysisUnload;
    }

    RtlZeroMemory(FunctionCount, sizeof(ULONG)*IRP_MJ_MAXIMUM_FUNCTION);// 稼働回数のクリア
    kPrint("DriverEntry-OUT %x\n", status);								// 終了メッセージ
    return status;
}

NTSTATUS
AcDeviceCreate(							// called by DriverEntry
    IN PDRIVER_OBJECT DriverObject		// DeviceObjectを生成し、下位ドライバに取り付ける
    )
{
    NTSTATUS status=STATUS_DEVICE_DOES_NOT_EXIST;
    PDEVICE_OBJECT deviceObject = NULL;
    PDEVICE_OBJECT previousObject = NULL;
    PDEVICE_OBJECT targetObject = NULL;
    PDEVICE_EXTENSION deviceExtension = NULL;
    UNICODE_STRING DosNameString;
    UNICODE_STRING uniNameString;
    UCHAR NameBuffer[256];

	kPrint("AcDeviceCreate\n");

    //
    // Attach to the existing DeviceObjects
    // 
		RtlInitUnicodeString(&uniNameString, DrvName);	// 取り付けのドライバ

        kPrint("uniNameString:%wZ\n", &uniNameString);
        //
        // Create the device object and log it in DeviceExtension.
        //
        status = IoCreateDevice(		  // デバイスオブジェクトを生成する
                 DriverObject,			  // DriverEntry関数で渡されたﾄﾞﾗｲﾊﾞｵﾌﾞｼﾞｪｸﾄへのﾎﾟｲﾝﾀ
                 sizeof(DEVICE_EXTENSION),// specify the device extension size
                 NULL,
				 FILE_DEVICE_UNKNOWN,
				 0,
                 FALSE,                   // This isn't an exclusive device
                 &deviceObject			  // 作成されたﾃﾞﾊﾞｲｽｵﾌﾞｼﾞｪｸﾄへのﾎﾟｲﾝﾀを受け取るﾎﾟｲﾝﾀ
                 );

        kPrint("uniNameString=%wZ %x\n", &uniNameString, status);
        if ( !NT_SUCCESS(status) ) {
            kPrint( "%s: Cannot create %s DeviceObject.\n",NAME_BASE,NameBuffer );
        	goto proc_skip;
        }

        		// ﾌｨﾙﾀﾄﾞﾗｲﾊﾞをﾃﾞﾊﾞｲｽｵﾌﾞｼﾞｪｸﾄにｱﾀｯﾁする
        status = IoAttachDevice(deviceObject,	// ﾌｨﾙﾀﾄﾞﾗｲﾊﾞのﾃﾞﾊﾞｲｽｵﾌﾞｼﾞｪｸﾄへのﾎﾟｲﾝﾀ
                                &uniNameString,	// ｱﾀｯﾁされるﾃﾞﾊﾞｲｽ名のUnicode文字列
                                &targetObject);	// ｱﾀｯﾁされるﾃﾞﾊﾞｲｽｵﾌﾞｼﾞｪｸﾄへのﾎﾟｲﾝﾀ
        kPrint( "%s: IoAttachDevice returns %08lX\n",NAME_BASE,status );
        if (!NT_SUCCESS(status)) {
            IoDeleteDevice(deviceObject);		// 下位ﾄﾞﾗｲﾊﾞから断続させる
            kPrint( "ERROR %s: IoAttachDevice returns %08lX\n",NAME_BASE,status );
        	goto proc_skip;
        }
        deviceObject->Flags |= DO_DIRECT_IO;
        deviceObject->AlignmentRequirement = targetObject->AlignmentRequirement;
        deviceExtension = deviceObject->DeviceExtension;
        deviceExtension->TargetDeviceObject = targetObject;
        deviceExtension->DeviceObject = deviceObject;
        deviceExtension->VerifyDeviceObject = NULL;
        deviceExtension->bVirtualFloppy = FALSE;
        deviceExtension->bVerifyRequired = FALSE;
        deviceExtension->Count = 0;

		RtlInitUnicodeString(&DosNameString, DOS_DEVICE_NAME);		// ﾃﾞﾊﾞｲｽｵﾌﾞｼﾞｪｸﾄをWin32
		status=IoCreateSymbolicLink(&DosNameString, &uniNameString);// から見えるようにする
		kPrint("IoCreateSymbolicLink %wZ, %wZ\n", &DosNameString, &uniNameString);

proc_skip:
    kPrint("AcDeviceCreate-OUT, %s %x\n", NAME_BASE, status);
    return status;
}

NTSTATUS
AnalysisAddDevice(	// AddDevice関数
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT pdo
    )
{
    NTSTATUS status = STATUS_SUCCESS;
    kPrint("AnalysisAddDevice-IN\n");
    // TODO
    kPrint("AnalysisAddDevice-OUT %x %x\n", status, pdo);
    return status;
}

NTSTATUS
AnalysisCommon(						// MajorFunctionに対する共通処理
    IN PDEVICE_OBJECT DeviceObject,	// IRP_MJ_DEVICE_CONTROLのときだけ、特別な処理をする。
    IN PIRP Irp						// その他の場合は、下位ドライバへ制御を渡す。
    )
{
    NTSTATUS status;
    PDEVICE_EXTENSION deviceExtension = DeviceObject->DeviceExtension;
    PIO_STACK_LOCATION currentIrpStack = IoGetCurrentIrpStackLocation(Irp);
    PIO_STACK_LOCATION nextIrpStack = IoGetNextIrpStackLocation(Irp);
	ULONG	nInBufferSize = currentIrpStack->Parameters.DeviceIoControl.InputBufferLength;
	ULONG	nOutBufferSize= currentIrpStack->Parameters.DeviceIoControl.OutputBufferLength;

	kPrint("# %2d %5d %s\n", currentIrpStack->MajorFunction,
	FunctionCount[currentIrpStack->MajorFunction], MJTable[currentIrpStack->MajorFunction]);

	++ FunctionCount[currentIrpStack->MajorFunction];			// ファンクション発行回数
	if(currentIrpStack->MajorFunction == IRP_MJ_DEVICE_CONTROL)
    {		// IF-IRP_MJ_DEVICE_CONTROL
    	switch ( currentIrpStack->Parameters.DeviceIoControl.IoControlCode ) {
    	case 8192:	// アプリケーションからの指令－１
		    {		// CASE 8192
    kPrint("アプリケーションからの指令－１ %x %d %d",			// 入力長    出力長
    currentIrpStack->Parameters.DeviceIoControl.IoControlCode, nInBufferSize, nOutBufferSize);
	DbgDump(FunctionCount, nOutBufferSize/2);
	RtlMoveMemory(Irp->AssociatedIrp.SystemBuffer, FunctionCount, nOutBufferSize/2);
		    	{	// addr
    PDEVICE_EXTENSION	deviceExtension		= DeviceObject->DeviceExtension;
    PDEVICE_OBJECT		TargetDeviceObject	= deviceExtension->TargetDeviceObject;
    PDRIVER_OBJECT		DriverObject		= TargetDeviceObject->DriverObject;
    PULONG				addr = Irp->AssociatedIrp.SystemBuffer;

    kPrint("\nアプリケーションからの指令－２ %x %x", addr, &addr[IRP_MJ_MAXIMUM_FUNCTION+1]);
	DbgDump(&DriverObject->MajorFunction[0], nOutBufferSize/2);
	RtlMoveMemory(&addr[IRP_MJ_MAXIMUM_FUNCTION+1], &DriverObject->MajorFunction[0], nOutBufferSize/2);
		    	}	// addr
	Irp->IoStatus.Information = sizeof(ULONG) * (IRP_MJ_MAXIMUM_FUNCTION+1) * 2;
	Irp->IoStatus.Status      = STATUS_SUCCESS;
#if 1				// 下位ﾄﾞﾗｲﾊﾞをｽｷｯﾌﾟしてI/O操作の完了をI/Oﾏﾈｰｼﾞｬに返す場合、ここを有効にする
			IoCompleteRequest( Irp, IO_NO_INCREMENT );
			return STATUS_SUCCESS;
#else
			break;
#endif
		    }		// CASE 8192

    	default:
        	break;
	    }	// SWITCH
    }		// IF-IRP_MJ_DEVICE_CONTROL

    *nextIrpStack = *currentIrpStack;	// Copy current stack to next stack.
    //
    // 下層ﾄﾞﾗｲﾊﾞからの戻り時に介入できるよう、I/O完了ﾙｰﾁﾝを下位ﾄﾞﾗｲﾊﾞのI/Oｽﾀｯｸｽﾛｯﾄにｱﾀｯﾁする
    //
    IoSetCompletionRoutine(Irp,	// Ask to be called back during request completion.
                           AcIoCompletionRoutine,	// I/O完了ルーチン
                           (PVOID)deviceExtension,	// 引数として渡されるﾄﾞﾗｲﾊﾞが定義した値
                           TRUE,					// TRUEを指定した場合、IoCompletionRoutine
                           TRUE,					// が呼び出される条件
                           TRUE);					// 

    status = IoCallDriver(deviceExtension->TargetDeviceObject, Irp);// 下位ﾄﾞﾗｲﾊﾞへ受け渡す
    return status;
}

VOID
AnalysisUnload(						// Unload関数
    IN PDRIVER_OBJECT DriverObject
    )
{
	NTSTATUS		status;
    PDEVICE_OBJECT	deviceObject = NULL;
	UNICODE_STRING	Win32DeviceName;

	kPrint("Unload-IN\n");
	RtlInitUnicodeString( &Win32DeviceName, DOS_DEVICE_NAME);		// ﾃﾞﾊﾞｲｽｵﾌﾞｼﾞｪｸﾄをWin32
    status = IoDeleteSymbolicLink( &Win32DeviceName );
	kPrint("DriverUnload: DriverObject (%wZ) %x\n", &Win32DeviceName, DriverObject);
	if ( NT_SUCCESS(status) ) {
		deviceObject = DriverObject->DeviceObject;
		kPrint("DRIVEROBJECT %x\n", &DriverObject->DeviceObject);
		kPrint("<< DriverUnload deviceObject >> %x\n", deviceObject);
		if(deviceObject)	IoDeleteDevice( deviceObject );
	}
	kPrint("Unload-OUT %x\n", status);
}

NTSTATUS				// 下位ドライバから戻ってきたときに動作する完了ルーチン
AcIoCompletionRoutine(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp,
    IN PVOID          Context
    )
{

#if 0			// 拡張処理
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);

    if ( NT_SUCCESS(Irp->IoStatus.Status) ) {
        switch (irpStack->MajorFunction) {
//		case xxx:		// ファンクション別の処理
//		break;
        default:
            break;
        }
    }
#endif

    //
    // Irp pending marking 
    //
    if (Irp->PendingReturned) {
        IoMarkIrpPending(Irp);			// "さらに処理が必要"とﾏｰｸする
    }
    return Irp->IoStatus.Status;
}

