/******************************************************************************
*
*	DETECT.C - Windows XP Filter Floppy Module 
*	標準のﾌﾛｯﾋﾟｰﾄﾞﾗｲﾊﾞに接続しアクセス状況を把握するとともに、合わせてﾌﾛｯﾋﾟｰﾃﾞｨ
*	ｽｸのｼｽﾃﾑ領域をｷｬｯｼｭ化することによりｱｸｾｽｽﾋﾟｰﾄﾞを向上させるﾌｨﾙﾀﾄﾞﾗｲﾊﾞである。
*   メディアチェンジは、動作確認のため意識的にサポートしない。
*
*	Copyright (c) 2002 AIdesign
*
*	PROGRAM: DETECT.C
*
*	PURPOSE: Filter Floppy Driver
*
******************************************************************************/

#include <ntddk.h>
#include <ntdddisk.h>
#include <memory.h>
#include <stdio.h>
#include "DETECT.h"
#include "devext.h"
#include "KdebugView.h"		// kernel-mode tiny debugger

//#define	CACHE	1
#define	SYS_SIZE	19

ULONG			BootFlag=0, SystemRecord=SYS_SIZE, DataRecord=33, NumSector=0;
PBOOT_SECTOR	BootPt;

ULONG			Previous=0, WayLength=0, AccessCount=0, CacheFlag;
ULONG			SysRecFlag[SYS_SIZE];			// システム領域読み込みフラグ
ULONG			SysArea[SYS_SIZE][512];			// システム領域読み込みバッファ

void CalcLength(ULONG record, ULONG DebugCode)	//① ヘッドの移動距離を計算する
{
ULONG	n;
UCHAR	c;
	if(DebugCode==IRP_MJ_READ)	c='R';
	else						c='W';
	if(record >= Previous)	n = record - Previous;	//② 前回とのレコード番号差を算出
	else					n = Previous - record;
	WayLength += n;									//③ 移動距離を加算する
	++ AccessCount;
	kPrint("WayLength(%c)= %d (%d) %d %d\n", c, WayLength, record, n, AccessCount);
	Previous = record;
}

NTSTATUS
DriverEntry(			// Windowsからの唯一の入り口
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    )
{
    NTSTATUS status;

	CacheFlag = ReadDebugPort(RegistryPath);	//④ キャッシュ有効フラグ設定
    kPrint("DETECT,DriverEntry-IN,V1.02  OutputPort=%x %x\n", OutputPort, CacheFlag);
    status = DeviceCreate(DriverObject);		//⑤ DeviceObjectを生成する
    if ( NT_SUCCESS(status) ) {
        //
        //⑥ Create dispatch points for create/open, close, unload.
        //
        kPrint("Create dispatch points\n");
        DriverObject->MajorFunction[IRP_MJ_CREATE]			= DetectOpenClose;
        DriverObject->MajorFunction[IRP_MJ_CLOSE]			= DetectOpenClose;
        DriverObject->MajorFunction[IRP_MJ_READ]			= DetectRead;
        DriverObject->MajorFunction[IRP_MJ_WRITE]			= DetectWrite;
        DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL]	= DetectDeviceCtl;
		DriverObject->DriverExtension->AddDevice			= DetectAddDevice;
        DriverObject->DriverUnload							= DetectUnload;
    }
	RtlZeroMemory(SysRecFlag, sizeof(ULONG)*SYS_SIZE);
    kPrint("DriverEntry-OUT %x\n", status);
    return status;
}

NTSTATUS
DetectAddDevice(		//⑦ set in DriverEntry
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT pdo
    )
{
    NTSTATUS status = STATUS_SUCCESS;
    kPrint("DetectAddDevice-IN\n");
    // TODO
    kPrint("DetectAddDevice-OUT %x %x\n", status, pdo);
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
DetectIoCompletionRoutine(	//⑧ 下位ドライバからの戻り時に介入する関数
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp,
    IN PVOID          Context
    )
{
    PDEVICE_EXTENSION  deviceExtension = Context;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
																  // BLOCK AAA
    if ( NT_SUCCESS(Irp->IoStatus.Status) && (CacheFlag==0x48) ) {// DebugViewPort=CACHE
        switch (irpStack->MajorFunction) {
        case IRP_MJ_READ:	//⑨ 下位ドライバで読み込み処理後に動作する
		{
		    PIO_STACK_LOCATION currentIrpStack = IoGetCurrentIrpStackLocation(Irp);
			ULONG	k = currentIrpStack->Parameters.Read.ByteOffset.LowPart;
			ULONG	l = currentIrpStack->Parameters.Read.Length;
			PUCHAR	addr = MmGetSystemAddressForMdl(Irp->MdlAddress);
			if((k == 0) && (BootFlag == 0)){	//⑩ ボリューム情報を確保する
				BootFlag = 1;					// ブートセクター読み込み処理
				BootPt = (PBOOT_SECTOR)addr;	// 簡便式、一回だけ
				//DbgDump(BootPt, 512);
				SystemRecord = BootPt->bsResSectors + BootPt->bsFATs * BootPt->bsFATsecs;
				DataRecord   = SystemRecord + BootPt->bsRootDirEnts/16;	// Add root_dir
				NumSector    = BootPt->bsSectors;						// Number of Sectors
				kPrint("BOOT %d %d %d %d %d %d\n", DataRecord, SystemRecord, NumSector,
					BootPt->bsResSectors, BootPt->bsFATs, BootPt->bsFATsecs);
			}
			k /= 512;		// record
			l /= 512;		// number
			//kPrint("Kr %d %d %d\n", k, l, SystemRecord);
			if( (k+l) < SystemRecord ){	// データをシステム領域に確保する
				ULONG	n;
				kPrint("SystemAreaへ(R) %d %x\n", k, addr);
				for(n=0; n<l; ++n){
					RtlCopyMemory(&SysArea[k+n][0], addr, 512);
					SysRecFlag[k+n] = 1;
					//kPrint("SystemAreaへ(R) %d %d %x\n", n, k+n, addr);
					//DbgDump(&SysArea[k+n][0], 512);
					addr += 512;
				}
			}
										// ルートディレクトリ、データ領域
		}
            break;
        case IRP_MJ_WRITE:
		{
		    PIO_STACK_LOCATION currentIrpStack = IoGetCurrentIrpStackLocation(Irp);
			ULONG	k = currentIrpStack->Parameters.Read.ByteOffset.LowPart;
			ULONG	l = currentIrpStack->Parameters.Read.Length;
			PUCHAR	addr = MmGetSystemAddressForMdl(Irp->MdlAddress);
			k /= 512;		// record
			l /= 512;		// number
			//kPrint("Wr %d %d %d\n", k, l, SystemRecord);
    if ( CacheFlag==0x48 ){	// DebugViewPort=CACHE
			if( (k+l) < SystemRecord ){	// システム領域		//⑪
				ULONG	n;
				kPrint("SystemAreaへ転送する(W) %d %d %x\n", k, l, addr);
				for(n=0; n<l; ++n){		// ディスク書き込みデータをメモリ上にも置く
					RtlCopyMemory(&SysArea[k+n][0], addr, 512);
					SysRecFlag[k+n] = 1;
					//kPrint("CopyMemory N=%d %d %x\n", n, k+n, addr);
					//DbgDump(&SysArea[k+n][0], 512);
					addr += 512;
				}
			}
	}
		}
            break;
        default:
            break;
        }
    }		// BLOCK AAA

    //
    // Irp pending marking 
    //
    if (Irp->PendingReturned) {
        IoMarkIrpPending(Irp);
    }
    return Irp->IoStatus.Status;
}

NTSTATUS				// set in DriverEntry
DetectRead(				// 読み込み関数、やがて下位ドライバに制御を渡す
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{
    NTSTATUS status;
    PDEVICE_EXTENSION  deviceExtension = DeviceObject->DeviceExtension;
    PIO_STACK_LOCATION currentIrpStack = IoGetCurrentIrpStackLocation(Irp);
    PIO_STACK_LOCATION nextIrpStack    = IoGetNextIrpStackLocation(Irp);
	ULONG	k = currentIrpStack->Parameters.Read.ByteOffset.LowPart;

    if ( CacheFlag==0x48 )	// DebugViewPort=CACHE
		{		// BLOCK BBB
			ULONG	l = currentIrpStack->Parameters.Read.Length;
			PUCHAR	addr = MmGetSystemAddressForMdl(Irp->MdlAddress);

			k /= 512;		// record
			l /= 512;		// number
			//kPrint("Rr %d %d %d\n", k, l, SystemRecord);
			if( (k+l) < SystemRecord ){	// システム領域		//⑫
				ULONG	n;
				for(n=0; n<l; ++n){
					if(SysRecFlag[k+n]==0)	goto normal60;
				}
				// すべてメモリにすでに存在している,メモリ上のデータをアプリに返す
				RtlCopyMemory(addr, &SysArea[k][0], 512*l);
				Irp->IoStatus.Status = STATUS_SUCCESS;
				Irp->IoStatus.Information = l*512;
				IoCompleteRequest(Irp, IO_NO_INCREMENT);	//⑬ 下層ドライバの呼び出しをスキップ
				kPrint("SystemAreaから転送(R) %d %d 下層ドライバの呼び出しをスキップ\n", k, l);
				//DbgDump(&SysArea[k][0], 512);
				return STATUS_SUCCESS;
			}
normal60:	;	// ルートディレクトリ、データ領域,システム領域でも未読み込みを含む
		}		// BLOCK BBB
    //
    // Copy current stack to next stack.
    //

    *nextIrpStack = *currentIrpStack;

    //
    // Ask to be called back during request completion.
    //

    IoSetCompletionRoutine(Irp,							//⑭ I/O完了ルーチンを取り付ける
                           DetectIoCompletionRoutine,	//⑮ DetectIoCompletionRoutineを指定
                           (PVOID)deviceExtension,
                           TRUE,
                           TRUE,
                           TRUE);

	//kPrint("RR LowLebel DRV\n");
	CalcLength(k, currentIrpStack->MajorFunction);	//⑯ ヘッドの移動距離を計算する
    status = IoCallDriver(deviceExtension->TargetDeviceObject, Irp);	//⑰
    return status;									// 下層ドライバを呼び出す
}

NTSTATUS				// set in DriverEntry
DetectWrite(			// 書き込み関数、そっくり下位ドライバに制御を渡す
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{
    NTSTATUS status;
    PDEVICE_EXTENSION  deviceExtension = DeviceObject->DeviceExtension;
    PIO_STACK_LOCATION currentIrpStack = IoGetCurrentIrpStackLocation(Irp);
    PIO_STACK_LOCATION nextIrpStack    = IoGetNextIrpStackLocation(Irp);

	ULONG	k;
//	ULONG	l;
//	PUCHAR	addr;

			k = currentIrpStack->Parameters.Read.ByteOffset.LowPart;
//			l = currentIrpStack->Parameters.Read.Length;
//			addr = MmGetSystemAddressForMdl(Irp->MdlAddress);

			k /= 512;		// record
//			l /= 512;		// number
			//kPrint("Ww %d %d %d\n", k, l, SystemRecord);

    //
    // Copy current stack to next stack.
    //

    *nextIrpStack = *currentIrpStack;

    //
    // Ask to be called back during request completion.
    //

    IoSetCompletionRoutine(Irp,
                           DetectIoCompletionRoutine,
                           (PVOID)deviceExtension,
                           TRUE,
                           TRUE,
                           TRUE);

	CalcLength(k, currentIrpStack->MajorFunction);	//⑯ ヘッドの移動距離を計算する
    status = IoCallDriver(deviceExtension->TargetDeviceObject, Irp);
    return status;									//⑱ 下層ドライバを呼び出す
}

NTSTATUS				//⑲ set in DriverEntry
DetectDeviceCtl(
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
                           DetectIoCompletionRoutine,		// I/O完了ﾙｰﾁﾝ
                           (PVOID)deviceExtension,
                           TRUE,
                           TRUE,
                           TRUE);

    status = IoCallDriver(deviceExtension->TargetDeviceObject, Irp);
    return status;
}

NTSTATUS				// set in DriverEntry
DetectOpenClose(
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
                           DetectIoCompletionRoutine,
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
DeviceCreate(							//⑳ called by DriverEntry
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
	kPrint("DetectDeviceCreate,FloppyCount = %d\n", *pFloppyCount);

    //
    // Attach to the existing DeviceObjects
    // 
    if(*pFloppyCount) {
        previousObject = deviceObject;

        sprintf( NameBuffer, "\\Device\\Floppy%d", Count);
        status = StringZ2UnicodeString(&uniNameString, NameBuffer);
        kPrint("uniNameString=%wZ %x\n", &uniNameString, status);
        if ( !NT_SUCCESS(status) ) goto proc_skip;

        //
        // Create the device object and log it in DeviceExtension.
        //
        status = IoCreateDevice(		//21 デバイスオブジェクトを生成する
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

        status = IoAttachDevice(deviceObject,	//22 フィルタドライバを取り付ける
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
    kPrint("%s: Current *pFloppyCount ==> %d\n", NAME_BASE, *pFloppyCount);
    return status;
}

NTSTATUS				// set in DriverEntry
DetectUnload(			//23 Unload時にDeviceを削除する
    IN PDRIVER_OBJECT DriverObject
    )
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
    while (deviceObject) {						//24 デバイスオブジェクトを削除する関数
        deviceExtension = deviceObject->DeviceExtension;
        Count = deviceExtension->Count;
        if (deviceExtension->bVirtualFloppy) {
            sprintf(
                NameBuffer,
                "\\DosDevices\\%c:",(UCHAR)('a'+(UCHAR)Count));
            kPrint("%s: Deleting SymbolicLink %s...\n",NAME_BASE,NameBuffer);
            status = StringZ2UnicodeString(&uniLinkNameString, NameBuffer);
			kPrint("uniLinkNameString=%wZ %x\n", &uniLinkNameString, status);
            if ( NT_SUCCESS(status) ) {
                status = IoDeleteSymbolicLink( &uniLinkNameString );// Win32空間から削除する
                RtlFreeUnicodeString( &uniLinkNameString );
                if (*pFloppyCount>1) (*pFloppyCount)--;
            }
        } else {
            IoDetachDevice( deviceExtension->TargetDeviceObject );// ドライバからデタッチする
        }
        if (deviceObject) IoDeleteDevice( deviceObject );	//25 デバイスオブジェクトを削除する
        deviceObject = deviceExtension->PreviousDeviceObject;
    }

    return status;
}
