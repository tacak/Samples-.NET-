/******************************************************************************
*
*	ACCESS.C - Windows XP Filter Driver Module 
*	機器の稼働状況に対する課金を管理するため、機能別稼働回数を把握するドライバ
*
*	Copyright (c) 2002 AIdesign
*
*	PROGRAM: ACCESS.C
*
*	PURPOSE: Filter Driver 
*
******************************************************************************/

#include <wdm.h>
#include <stdio.h>
#include "access.h"
#include "devext.h"
#include "KdebugView.h"

#define	DOS_DEVICE_NAME	L"\\DosDevices\\aCount"	// ｱﾌﾟﾘｹｰｼｮﾝからのﾃﾞﾊﾞｲｽ名

LONGLONG	StartTime;
LONGLONG	PassTime[IRP_MJ_MAXIMUM_FUNCTION+1];
ULONG		FunctionCount[IRP_MJ_MAXIMUM_FUNCTION+1];

NTSTATUS
DriverEntry(			// Windowsからの唯一の入り口
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    )
{
    NTSTATUS	status;
    ULONG		i;

	ReadDebugPort(RegistryPath);
	kPrint("DriverEntry-IN,ACCESS Version 1.02 <<< 2002-08-29 >>> %x\n", OutputPort);
    status = AcDeviceCreate(DriverObject);
    if ( NT_SUCCESS(status) ) {
        //
        // Create dispatch points for create/open, close, unload.
        //
        xprintf("Create dispatch points %d\n", IRP_MJ_MAXIMUM_FUNCTION);
        for(i=0; i<IRP_MJ_MAXIMUM_FUNCTION; ++i){
			DriverObject->MajorFunction[i]				= AccessCommon;
		}
			DriverObject->DriverExtension->AddDevice	= AccessAddDevice;
        	DriverObject->DriverUnload					= AccessUnload;
    }

    RtlZeroMemory(PassTime, sizeof(LONGLONG)*IRP_MJ_MAXIMUM_FUNCTION);	// 経過時間のクリア
    RtlZeroMemory(FunctionCount, sizeof(ULONG)*IRP_MJ_MAXIMUM_FUNCTION);// 稼働回数のクリア
    kPrint("DriverEntry-OUT %x\n", status);
    return status;
}

NTSTATUS
AccessAddDevice(	// AddDevice
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT pdo
    )
{
    NTSTATUS status = STATUS_SUCCESS;
    xprintf("AccessAddDevice-IN\n");
    // TODO
    xprintf("AccessAddDevice-OUT %x %x\n", status, pdo);
    return status;
}

NTSTATUS
AccessCommon(		// MajorFunctionに対する共通処理
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{
    NTSTATUS status;
    PDEVICE_EXTENSION deviceExtension = DeviceObject->DeviceExtension;
    PIO_STACK_LOCATION currentIrpStack = IoGetCurrentIrpStackLocation(Irp);
    PIO_STACK_LOCATION nextIrpStack = IoGetNextIrpStackLocation(Irp);

	KeQuerySystemTime( (PLARGE_INTEGER)&StartTime );		// 100ns
	kPrint("# %2d %s ", currentIrpStack->MajorFunction,
							MJTable[currentIrpStack->MajorFunction]);

	if(currentIrpStack->MajorFunction == IRP_MJ_DEVICE_CONTROL){
    	xprintf("CTL %x ", currentIrpStack->Parameters.DeviceIoControl.IoControlCode);
    	switch ( currentIrpStack->Parameters.DeviceIoControl.IoControlCode ) {
    	case 8192:		// 課金システムからの指令
    {
ULONG	nInBufferSize = currentIrpStack->Parameters.DeviceIoControl.InputBufferLength;
ULONG	nOutBufferSize= currentIrpStack->Parameters.DeviceIoControl.OutputBufferLength;

	dumpx(FunctionCount, IRP_MJ_MAXIMUM_FUNCTION * 4);
	RtlMoveMemory(Irp->AssociatedIrp.SystemBuffer, FunctionCount,
									sizeof(ULONG) * IRP_MJ_MAXIMUM_FUNCTION);
	Irp->IoStatus.Information = sizeof(ULONG) * IRP_MJ_MAXIMUM_FUNCTION;
	Irp->IoStatus.Status      = STATUS_SUCCESS;

	xprintf("%x %x ", nInBufferSize, nOutBufferSize);
						// 入力長　　出力長
		IoCompleteRequest( Irp, IO_NO_INCREMENT );
						// 下位ﾄﾞﾗｲﾊﾞをｽｷｯﾌﾟしてI/O操作の完了をI/Oﾏﾈｰｼﾞｬに返す
		return STATUS_SUCCESS;
    }
    	default:
        	break;
	    }
    }

    *nextIrpStack = *currentIrpStack;	// Copy current stack to next stack.
    //
    // 下層ﾄﾞﾗｲﾊﾞからの戻り時に介入できるよう、I/O完了ﾙｰﾁﾝを下位ﾄﾞﾗｲﾊﾞのI/Oｽﾀｯｸｽﾛｯﾄにｱﾀｯﾁする
    //
    IoSetCompletionRoutine(Irp,	// Ask to be called back during request completion.
                           AcIoCompletionRoutine,
                           (PVOID)deviceExtension,
                           TRUE,
                           TRUE,
                           TRUE);

    status = IoCallDriver(deviceExtension->TargetDeviceObject, Irp);
    return status;
}

VOID
AccessUnload(
    IN PDRIVER_OBJECT DriverObject
    )
{
	xprintf("Unload\n");
}

NTSTATUS				// 下位ドライバから戻ってきたときに動作する完了ルーチン
AcIoCompletionRoutine(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp,
    IN PVOID          Context
    )
{
    PDEVICE_EXTENSION  deviceExtension = Context;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);

    LONGLONG	CurrentTime;

	KeQuerySystemTime( (PLARGE_INTEGER)&CurrentTime );			// 100ns
	CurrentTime -= StartTime;			// 消費時間(100ns単位)
	CurrentTime /= 10000;				// マイクロ -> ミリ
	xprintf("%I64ums ", CurrentTime);

	PassTime[irpStack->MajorFunction] += CurrentTime;	// 合計時間
	++ FunctionCount[irpStack->MajorFunction];			// ファンクション発行回数
	kPrint("%d\n", FunctionCount[irpStack->MajorFunction]);

#if 0	// ファンクション別の処理
    if ( NT_SUCCESS(Irp->IoStatus.Status) ) {
        switch (irpStack->MajorFunction) {

        default:
            break;
        }
    }
#endif

    //
    // Irp pending marking 
    //
    if (Irp->PendingReturned) {
        IoMarkIrpPending(Irp);
    }
    return Irp->IoStatus.Status;
}

NTSTATUS
StringZ2UnicodeString(
    IN PUNICODE_STRING uniNameString,
    IN PUCHAR StringZ
    )
{
    NTSTATUS status;
    STRING ansiNameString;
    RtlInitAnsiString( &ansiNameString, StringZ );
    status = RtlAnsiStringToUnicodeString(
        uniNameString,
        &ansiNameString,
        TRUE );
    return status;
}

NTSTATUS
AcDeviceCreate(							// called by DriverEntry
    IN PDRIVER_OBJECT DriverObject
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
    ULONG Count=0;

    //
    // Create counted string version of our device name.
    //
	xprintf("AcDeviceCreate,FloppyCount\n");

    //
    // Attach to the existing DeviceObjects
    // 
    if(1) {
        previousObject = deviceObject;

        sprintf( NameBuffer, "\\Device\\Floppy%d", Count);
        status = StringZ2UnicodeString(&uniNameString, NameBuffer);
        xprintf("uniNameString=%wZ %x\n", &uniNameString, status);
        //xprintf("uniNameString= %x\n", status);
        if ( !NT_SUCCESS(status) ) goto proc_skip;

        //
        // Create the device object and log it in DeviceExtension.
        //
        status = IoCreateDevice(
                 DriverObject,
                 sizeof(DEVICE_EXTENSION),// specify the device extension size
                 NULL,
                 FILE_DEVICE_DISK,
                 FILE_REMOVABLE_MEDIA | FILE_FLOPPY_DISKETTE,
                 FALSE,                   // This isn't an exclusive device
                 &deviceObject
                 );

        if ( !NT_SUCCESS(status) ) {
            RtlFreeUnicodeString( &uniNameString );
            kPrint("%s: Cannot create %s DeviceObject.\n",NAME_BASE,NameBuffer);
        	goto proc_skip;
        }

        status = IoAttachDevice(deviceObject,
                                &uniNameString,
                                &targetObject);
        if (!NT_SUCCESS(status)) {
            IoDeleteDevice(deviceObject);
            kPrint("%s: IoAttachDevice returns %08lX\n",NAME_BASE,status);
        	goto proc_skip;
        }
        deviceObject->Flags |= DO_DIRECT_IO;
        deviceObject->AlignmentRequirement = targetObject->AlignmentRequirement;
        deviceExtension = deviceObject->DeviceExtension;
        deviceExtension->TargetDeviceObject = targetObject;
        deviceExtension->DeviceObject = deviceObject;
        deviceExtension->PreviousDeviceObject = previousObject;
        deviceExtension->VerifyDeviceObject = NULL;
        deviceExtension->bVirtualFloppy = FALSE;
        deviceExtension->bVerifyRequired = FALSE;
        deviceExtension->Count = Count;

		RtlInitUnicodeString(&DosNameString, DOS_DEVICE_NAME);
		status=IoCreateSymbolicLink(&DosNameString, &uniNameString);
		kPrint("IoCreateSymbolicLink %wZ: %x\n", &DosNameString, status);
        //MUTEX_INIT( deviceExtension->VerifySpinLock );
    }

proc_skip:
	RtlFreeUnicodeString( &uniNameString );
    kPrint("%s: Current *pFloppyCount ==>\n", NAME_BASE);

    return status;
}

