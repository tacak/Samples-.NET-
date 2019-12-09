/******************************************************************************
*
*	XFLOPPY.C - Windows XP Filter Floppy Module 
*	標準のﾌﾛｯﾋﾟｰﾄﾞﾗｲﾊﾞに接続し暗号化して書き込み、復号化して読み込むﾌｨﾙﾀﾄﾞﾗｲﾊﾞ
*
*	Copyright (c) 2002 AIdesign
*
*	PROGRAM: XFLOPPY.C
*
*	PURPOSE: Filter Floppy Driver
*
******************************************************************************/

#include <ntddk.h>
#include <ntdddisk.h>
#include <stdio.h>
#include "xfloppy.h"
#include "devext.h"
#include "KdebugView.h"

#define	FILTER_OK	2	// 1:all record,2:only data record

#if FILTER_OK
ULONG			BootFlag=0, SystemRecord=19, DataRecord=33;
PBOOT_SECTOR	BootPt;
#endif

NTSTATUS
DriverEntry(			// Windowsからの唯一の入り口
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    )
{
    NTSTATUS status;

	ReadDebugPort(RegistryPath);
    kPrint("XFLOPPY,DriverEntry-IN, Version 1.02 %d %x\n", FILTER_OK, OutputPort);
    status = xFDeviceCreate(DriverObject);
    if ( NT_SUCCESS(status) ) {
        //
        // Create dispatch points for create/open, close, unload.
        //
        kPrint("Create dispatch points\n");
        DriverObject->MajorFunction[IRP_MJ_CREATE]			= xFloppyOpenClose;
        DriverObject->MajorFunction[IRP_MJ_CLOSE]			= xFloppyOpenClose;
        DriverObject->MajorFunction[IRP_MJ_READ]			= xFloppyRead;
        DriverObject->MajorFunction[IRP_MJ_WRITE]			= xFloppyWrite;
        DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL]	= xFloppyDeviceCtl;
		DriverObject->DriverExtension->AddDevice			= xFloppyAddDevice;
        DriverObject->DriverUnload							= xFloppyUnload;
    }
    kPrint("DriverEntry-OUT %x\n", status);
    return status;
}

NTSTATUS
xFloppyAddDevice(		// set in DriverEntry
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT pdo
    )
{
    NTSTATUS status = STATUS_SUCCESS;
    kPrint("xFloppyAddDevice-IN\n");
    // TODO
    kPrint("xFloppyAddDevice-OUT %x %x\n", status, pdo);
    return status;
}

PULONG					// 機器の接続数を求める
GetFloppyCount()
{
    PCONFIGURATION_INFORMATION pConfigInfo;
    pConfigInfo = IoGetConfigurationInformation();

    if ( pConfigInfo == NULL ) {
        return 0;
    } 
    return &pConfigInfo->FloppyCount; 
}

NTSTATUS
xFIoCompletionRoutine(	// 下位ドライバからの戻り時に介入する関数
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp,
    IN PVOID          Context
    )
{
    PDEVICE_EXTENSION  deviceExtension = Context;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);

    if ( NT_SUCCESS(Irp->IoStatus.Status) ) {
        switch (irpStack->MajorFunction) {
#if	FILTER_OK							// 暗号化と復号化処理
        case IRP_MJ_READ:
		{
		    PIO_STACK_LOCATION currentIrpStack = IoGetCurrentIrpStackLocation(Irp);
			ULONG	k = currentIrpStack->Parameters.Read.ByteOffset.LowPart;
			ULONG	l = currentIrpStack->Parameters.Read.Length;
			PUCHAR	addr = MmGetSystemAddressForMdl(Irp->MdlAddress);
	#if FILTER_OK == 1					// 全領域を暗号化と復号化対象にする
			kPrint(" h %d (%d) %x\n", k, l, addr);
			while(l--){					// 指定された長さぶん、可逆可能な変換をする
				*(addr++) ^= 0xff;		// 復号化
			}
	#else								// データ領域を暗号化と復号化対象にする
			kPrint(" h %d (%d) %x %d\n", k, l, addr, DataRecord*512);
			if((k == 0) && (BootFlag == 0)){
				BootFlag = 1;
				BootPt = (PBOOT_SECTOR)addr;
				//dumpx(BootPt, 512);
				SystemRecord = BootPt->bsResSectors + BootPt->bsFATs * BootPt->bsFATsecs;
				DataRecord   = SystemRecord + BootPt->bsRootDirEnts/16;
				kPrint("%d %d %d %d %d\n", DataRecord, SystemRecord, BootPt->bsResSectors,
												BootPt->bsFATs, BootPt->bsFATsecs);
			}
			if(k >= DataRecord*512){	// データ領域以外はスキップする
				while(l--){				// 指定された長さぶん、可逆可能な変換をする
					*(addr++) ^= 0xff;	// 復号化
				}
			}
	#endif
		}
            break;
        case IRP_MJ_WRITE:
		{
		    PIO_STACK_LOCATION currentIrpStack = IoGetCurrentIrpStackLocation(Irp);
			ULONG	k = currentIrpStack->Parameters.Read.ByteOffset.LowPart;
			ULONG	l = currentIrpStack->Parameters.Read.Length;
			PUCHAR	addr = MmGetSystemAddressForMdl(Irp->MdlAddress);

	#if FILTER_OK == 1					// 全領域を暗号化と復号化対象にする
			while(l--){					// 指定された長さぶん、可逆可能な変換をする
				*(addr++) ^= 0xff;		// バッファ内容を元へ戻す
			}
	#else								// データ領域を暗号化と復号化対象にする
			if(k >= DataRecord*512){	// データ領域以外はスキップする
				while(l--){				// 指定された長さぶん、可逆可能な変換をする
					*(addr++) ^= 0xff;	// バッファ内容を元へ戻す
				}
				kPrint(" $ ");
			}
	#endif
		}
            break;
#endif

        default:
            break;
        }
    }

    //
    // Irp pending marking 
    //
    if (Irp->PendingReturned) {
        IoMarkIrpPending(Irp);
    }
    return Irp->IoStatus.Status;
}

NTSTATUS				// set in DriverEntry
xFloppyRead(			// 読み込み関数、そっくり下位ドライバに制御を渡す
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{
    NTSTATUS status;
    PDEVICE_EXTENSION deviceExtension = DeviceObject->DeviceExtension;
    PIO_STACK_LOCATION currentIrpStack = IoGetCurrentIrpStackLocation(Irp);
    PIO_STACK_LOCATION nextIrpStack = IoGetNextIrpStackLocation(Irp);

    //
    // Copy current stack to next stack.
    //

    *nextIrpStack = *currentIrpStack;

    //
    // Ask to be called back during request completion.
    //

    IoSetCompletionRoutine(Irp,
                           xFIoCompletionRoutine,
                           (PVOID)deviceExtension,
                           TRUE,
                           TRUE,
                           TRUE);

	kPrint("R");
    status = IoCallDriver(deviceExtension->TargetDeviceObject, Irp);
    return status;
}

NTSTATUS				// set in DriverEntry
xFloppyWrite(			// 書き込み関数、暗号化のあと、下位ドライバに制御を渡す
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{
    NTSTATUS status;

	ULONG	k, l;
	PUCHAR	addr;

    PDEVICE_EXTENSION deviceExtension = DeviceObject->DeviceExtension;
    PIO_STACK_LOCATION currentIrpStack = IoGetCurrentIrpStackLocation(Irp);
    PIO_STACK_LOCATION nextIrpStack = IoGetNextIrpStackLocation(Irp);

#if	FILTER_OK
	k = currentIrpStack->Parameters.Read.ByteOffset.LowPart;
	l = currentIrpStack->Parameters.Read.Length;
	addr = MmGetSystemAddressForMdl(Irp->MdlAddress);
	kPrint(" H %d (%d) %x %d\n", k, l, addr, DataRecord*512);

	#if FILTER_OK == 1
	while(l--){					// 指定された長さぶん、可逆可能な変換をする
		*(addr++) ^= 0xff;		// 暗号化
	}
	#else
	if(k >= DataRecord*512){	// データ領域以外,WRITE_FUNCTION
		while(l--){				// 指定された長さぶん、可逆可能な変換をする
			*(addr++) ^= 0xff;	// 暗号化
		}
	}
	#endif
#endif

	kPrint("W");

    //
    // Copy current stack to next stack.
    //

    *nextIrpStack = *currentIrpStack;

    //
    // Ask to be called back during request completion.
    //

    IoSetCompletionRoutine(Irp,
                           xFIoCompletionRoutine,
                           (PVOID)deviceExtension,
                           TRUE,
                           TRUE,
                           TRUE);

    status = IoCallDriver(deviceExtension->TargetDeviceObject, Irp);
    return status;
}

NTSTATUS				// set in DriverEntry
xFloppyDeviceCtl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{
    NTSTATUS status;
    PDEVICE_EXTENSION deviceExtension = DeviceObject->DeviceExtension;
    PIO_STACK_LOCATION currentIrpStack = IoGetCurrentIrpStackLocation(Irp);
    PIO_STACK_LOCATION nextIrpStack = IoGetNextIrpStackLocation(Irp);

    //kPrint("CTL %x\n", currentIrpStack->Parameters.DeviceIoControl.IoControlCode);
    switch ( currentIrpStack->Parameters.DeviceIoControl.IoControlCode ) {
    case IOCTL_DISK_CHECK_VERIFY:		// 74800
        break;

    case IOCTL_DISK_IS_WRITABLE:
    case IOCTL_DISK_GET_DRIVE_GEOMETRY:
    case IOCTL_DISK_GET_PARTITION_INFO:
        break;

    default:
        break;

    }

    //
    // Copy current stack to next stack.
    //

    *nextIrpStack = *currentIrpStack;

    //
    // 下層ﾄﾞﾗｲﾊﾞからの戻り時に介入できるよう、I/O完了ﾙｰﾁﾝを下位ﾄﾞﾗｲﾊﾞのI/Oｽﾀｯｸｽﾛｯﾄにｱﾀｯﾁする
    //

    IoSetCompletionRoutine(Irp,
                           xFIoCompletionRoutine,		// I/O完了ﾙｰﾁﾝ
                           (PVOID)deviceExtension,
                           TRUE,
                           TRUE,
                           TRUE);

    status = IoCallDriver(deviceExtension->TargetDeviceObject, Irp);
    return status;
}

NTSTATUS				// set in DriverEntry
xFloppyOpenClose(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{
    NTSTATUS status;
    PDEVICE_EXTENSION deviceExtension	= DeviceObject->DeviceExtension;
    PIO_STACK_LOCATION currentIrpStack	= IoGetCurrentIrpStackLocation(Irp);
    PIO_STACK_LOCATION nextIrpStack		= IoGetNextIrpStackLocation(Irp);

	//kPrint("%s: %s\n",NAME_BASE,MJTable[currentIrpStack->MajorFunction]);

    *nextIrpStack = *currentIrpStack;	// Copy current stack to next stack.

    IoSetCompletionRoutine(Irp,	// Ask to be called back during request completion.
                           xFIoCompletionRoutine,
                           (PVOID)deviceExtension,
                           TRUE,
                           TRUE,
                           TRUE);

    status = IoCallDriver(deviceExtension->TargetDeviceObject, Irp);
    return status;
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
xFDeviceCreate(							// called by DriverEntry
    IN PDRIVER_OBJECT DriverObject
    )
{
    NTSTATUS status=STATUS_DEVICE_DOES_NOT_EXIST;
    PDEVICE_OBJECT deviceObject			= NULL;
    PDEVICE_OBJECT deviceObject0		= NULL;
    PDEVICE_OBJECT previousObject		= NULL;
    PDEVICE_OBJECT targetObject			= NULL;
    PDEVICE_OBJECT targetObject0		= NULL;
    PDEVICE_EXTENSION deviceExtension	= NULL;
    UNICODE_STRING uniLinkNameString;
    UNICODE_STRING uniNameString;
    UCHAR NameBuffer[256];
    PULONG pFloppyCount;
    ULONG Count=0;

    //
    // Create counted string version of our device name.
    //
    pFloppyCount = GetFloppyCount();
	kPrint("xFDeviceCreate,FloppyCount = %d\n", *pFloppyCount);

    //
    // Attach to the existing DeviceObjects
    // 
    if(*pFloppyCount) {
        previousObject = deviceObject;

        sprintf( NameBuffer, "\\Device\\Floppy%d", Count);
        status = StringZ2UnicodeString(&uniNameString, NameBuffer);
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
        RtlFreeUnicodeString( &uniNameString );

        if (!NT_SUCCESS(status)) {
            IoDeleteDevice(deviceObject);
            kPrint("%s: IoAttachDevice returns %08lX\n",NAME_BASE,status);
        	goto proc_skip;
        }
        deviceObject->Flags					   |= DO_DIRECT_IO;
        deviceObject->AlignmentRequirement		= targetObject->AlignmentRequirement;
        deviceExtension							= deviceObject->DeviceExtension;
        deviceExtension->TargetDeviceObject		= targetObject;
        deviceExtension->DeviceObject			= deviceObject;
        deviceExtension->PreviousDeviceObject	= previousObject;
        deviceExtension->VerifyDeviceObject		= NULL;
        deviceExtension->bVirtualFloppy			= FALSE;
        deviceExtension->bVerifyRequired		= FALSE;
        deviceExtension->Count = Count;

        MUTEX_INIT( deviceExtension->VerifySpinLock );
    }

proc_skip:
    kPrint("%s: Current *pFloppyCount= %d %x\n", NAME_BASE, *pFloppyCount, status);
    return status;
}

VOID					// set in DriverEntry
xFloppyUnload(			// Unload時にDeviceを削除する
    IN PDRIVER_OBJECT DriverObject)
{
    NTSTATUS status;
    PDEVICE_OBJECT deviceObject = NULL;
    PDEVICE_EXTENSION deviceExtension = NULL;
    UNICODE_STRING uniLinkNameString;
    UCHAR NameBuffer[256];
    PULONG pFloppyCount;
    ULONG Count;

    //
    // Create counted string version of our device name.
    //
    pFloppyCount = GetFloppyCount();

    deviceObject = DriverObject->DeviceObject;
	kPrint("xFloppyUnload-IN %x\n", deviceObject);
    while (deviceObject) {
        deviceExtension = deviceObject->DeviceExtension;
        Count = deviceExtension->Count;
        if (deviceExtension->bVirtualFloppy) {
            sprintf(
                NameBuffer,
                "\\DosDevices\\%c:",(UCHAR)('a'+(UCHAR)Count));
            kPrint("%s: Deleting SymbolicLink %s...\n",NAME_BASE,NameBuffer);
            status = StringZ2UnicodeString(&uniLinkNameString, NameBuffer);
            if ( NT_SUCCESS(status) ) {
                status = IoDeleteSymbolicLink( &uniLinkNameString );
                RtlFreeUnicodeString( &uniLinkNameString );
                if (*pFloppyCount>1) (*pFloppyCount)--;
            }
        } else {
            kPrint("%s: Detaching \\Devices\\Floppy%d...\n",NAME_BASE,Count);
            IoDetachDevice( deviceExtension->TargetDeviceObject );
        }
        if (deviceObject) IoDeleteDevice( deviceObject );
        deviceObject = deviceExtension->PreviousDeviceObject;
    }
    kPrint("xFloppyUnload-OUT\n");
}
