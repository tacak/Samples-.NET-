/////////////////////////////////////////////////////////////////
//
//  kFILTER.c   - kFILTER driver entry point
//  Purpose     - キーコード(0x00-0xFF)別入力回数を計測する
//  Environment - Kernel Mode Only
//  Copyright (c) 2002, AiDesign, M.Takiguchi
//  WindowsXP,Windows2000,WindowsNTに対応
//
/////////////////////////////////////////////////////////////////

#include <wdm.h>

#include "ntddkbd.h"	// driver defines and class definition
#include "kFILTER.h"	// driver defines and class definition
#include "IoCtl.h"		// driver defines and class definition
#include "KdebugView.h"	// for debug

#ifdef ALLOC_PRAGMA		// routines which can be discarded after initialization
#pragma alloc_text(INIT,DriverEntry)
#pragma alloc_text(INIT,CreateKeyFilterDevice)
#endif

// スタティック変数
	ULONG		KeyCount[256];		// キーコード別Readカウンタ
	int			OsVersion;
	LONGLONG	StartTime;
	LONGLONG	CurrentTime;

///////////////////////////////////////////////////////////////////
//
//  DriverEntry - initial driver entry point for KeyFilter driver
//  Input:  DriverObject - NT driver object
//          RegistryPath - path to this drivers registry entries
//  Output: return()     - NTSTATUS value
//  Notes:
//
///////////////////////////////////////////////////////////////////

NTSTATUS DriverEntry(PDRIVER_OBJECT   DriverObject,
                              PUNICODE_STRING  RegistryPath)
{
    NTSTATUS        status = STATUS_SUCCESS;	// return status
    PDEVICE_OBJECT  pDeviceObject;				// current NT device object

	OutputPort=0x3f8;			// デフォルトのデバッグ出力先
	ReadDebugPort(RegistryPath);// レジストリを探索してデバッグ出力先を決める

	OsVersion = 0x0120;			// OSバージョンを確認する
	if(!IoIsWdmVersionAvailable(1, 0x20)){				// !WindowsXP
		OsVersion = 0x0110;
		if(!IoIsWdmVersionAvailable(1, 0x10)){			// !Windows2000
			kPrint("not support OperatingSystem!\n");
			return STATUS_DRIVER_UNABLE_TO_LOAD;		// 0xC000026CL
		}
	}													// Windows XP,2000

	kPrint("DriverEntry-IN,<%s> V1.02[2002-10-04], OS=%x\n", HEADER_, OsVersion);

        // メジャーファンクションに対応する処理関数を取り付ける
    DriverObject->MajorFunction[IRP_MJ_READ]           = KeyFilterReadDispatch;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = KeyFilterIoctlDispatch;
    DriverObject->MajorFunction[IRP_MJ_CLEANUP]        = KeyFilterDefaultDispatch;
    DriverObject->MajorFunction[IRP_MJ_FLUSH_BUFFERS]  = KeyFilterDefaultDispatch;
    DriverObject->MajorFunction[IRP_MJ_CREATE]         = KeyFilterDefaultDispatch;
    DriverObject->MajorFunction[IRP_MJ_CLOSE]          = KeyFilterDefaultDispatch;
    DriverObject->DriverUnload                         = NULL;

        // デバイスオブジェクトを生成し、フィルタドライバを下位ドライバに接続する
	status = CreateKeyFilterDevice(DriverObject, &pDeviceObject);
	kPrint("STATUS %x\n", status);
	if (!NT_SUCCESS(status)) {	// failed to create the device, cleanup
		kPrint("kFILTER: Failed to Create Device number 0, status(0x%X)\n", status);
		// do not create any more devices
		// failed to create the device (this message will print if an
		//  error on creation occurs or after the last device is found)
		DeleteKeyFilterDevice(pDeviceObject);
	}
	else{
	    RtlZeroMemory(KeyCount, sizeof(ULONG)*256);		// キーコード別Readカウンタのクリア
		KeQuerySystemTime( (PLARGE_INTEGER)&StartTime );// 調査開始時間を記憶する、単位は100ns
		status = STATUS_SUCCESS;
    }
    return status;
}

/////////////////////////////////////////////////////////////////////
//
//  CreateKeyFilterDevice   - Creates a single KeyFilter Device
//  Input:  pDriverObject   - NT driver object
//  Output: ppDeviceObject  - NT device object for this instance of the device
//  Notes:  throws exception on error
//
//////////////////////////////////////////////////////////////////////

NTSTATUS CreateKeyFilterDevice( IN  PDRIVER_OBJECT      pDriverObject,
                                OUT PDEVICE_OBJECT      *ppDeviceObject)
{
    NTSTATUS            status;				// return status
    PDEVICE_EXTENSION   pExtension;			// the device extension
	UNICODE_STRING		UpperDeviceName;	// the name of the device
    UNICODE_STRING      keyboardDeviceName;	// the name of the lower device
	CCHAR				NameBuffer[64];
	UNICODE_STRING		Win32DeviceName;

    kPrint("CreateKeyFiltDevice-IN\n");
    	// initialize the device name
	RtlInitUnicodeString( &UpperDeviceName, PHD_DEVICE_NAME );// L"\\Device\\KFILTER"
        // create the device object			デバイスオブジェクトを生成する
    status = IoCreateDevice (pDriverObject,
                sizeof(DEVICE_EXTENSION),
                &UpperDeviceName,
                KFILTER_DEVICE_TYPE, 
                0,
                FALSE,
                ppDeviceObject);			// fdo
    if (!NT_SUCCESS(status)) {
        *ppDeviceObject = NULL;
        kPrint("kFILTER: Unable to create device object\n");
        return status;
    }
        // get the new device extension
    pExtension = (*ppDeviceObject)->DeviceExtension;
        // zero the extension
    RtlZeroMemory(pExtension, sizeof(DEVICE_EXTENSION));
        // this device uses buffered I/O
    (*ppDeviceObject)->Flags |= DO_BUFFERED_IO;

        // initialize the keyboard device name
	RtlCopyMemory( NameBuffer, L"\\Device\\KeyboardClass0", 64);
	RtlInitUnicodeString( &keyboardDeviceName, (PCWSTR)NameBuffer);

        // attach to the lower level driver	フィルタデバイスをターゲットデバイスに接続
    status = IoAttachDevice(*ppDeviceObject,
                            &keyboardDeviceName,
                            &(pExtension->pKeyboardClassDeviceObject));

	kPrint("IoAttachDevice %x %wZ\n", status, &keyboardDeviceName);
    if (!NT_SUCCESS(status)) {
        kPrint("kFILTER: Unable to attach to KeyboardClass device object\n");
        return status;
    }

        // create a Win32 accessable name for the driver
        // in the form of kFILTER,このデバイスをアプリケーションから参照できるようにする
	RtlCopyMemory( NameBuffer, WIN32_PATH, 64);	// WIN32_PATH	L"\\DosDevices\\kCOUNT"
	RtlInitUnicodeString( &Win32DeviceName, (PCWSTR)NameBuffer);
	status=IoCreateSymbolicLink(&Win32DeviceName, &UpperDeviceName);
	kPrint("Win32=%wZ, UpperDevice=%wZ\n", &Win32DeviceName, &UpperDeviceName);
	kPrint("STATUS= %x\n", status);
    if (!NT_SUCCESS(status)) {
        kPrint("kFILTER: CreateKeyFilterDevice: Failed to create Win32 name\n");
        return status;
    }

    kPrint("CreateKeyFiltDevice-OUT %x\n", status);
    return status;
}

////////////////////////////////////////////////////////////////////
//
//  DeleteKeyFilterDevice - unmaps all resources and deletes the device
//  Input:  pDeviceObject - the device object to delete
//  Output: none
//  Notes:  
//
//////////////////////////////////////////////////////////////////// 

VOID DeleteKeyFilterDevice(IN PDEVICE_OBJECT pDeviceObject) 
{
    PDEVICE_EXTENSION   pExtension;     // the device extension

        // make sure the device object is non-null
    if (pDeviceObject == NULL) {
        return;
    }
        // get the extension
    pExtension = pDeviceObject->DeviceExtension;

        // if this device is attached, unattach it
    if (NULL != pExtension->pKeyboardClassDeviceObject) {
        IoDetachDevice(pExtension->pKeyboardClassDeviceObject);
    }

    IoDeleteDevice(pDeviceObject);
}

///////////////////////////////////////////////////////////////////
//
//  KeyFilterReadDispatch - Read file dispatch entry point
//  Input:  DeviceObject  - NT device object
//          Irp           - IRP being dispatched
//  Output: return        - NT status value
//  Notes:  handles only IRP_MJ_READ
//
///////////////////////////////////////////////////////////////////

NTSTATUS KeyFilterReadDispatch(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{		// Read関数
        // get our device extension
    PDEVICE_EXTENSION Extension = (PDEVICE_EXTENSION)(DeviceObject->DeviceExtension);

        // get the our Irp stack location
    PIO_STACK_LOCATION IrpStack = IoGetCurrentIrpStackLocation(Irp);

        // get the next Irp stack location
    PIO_STACK_LOCATION NextIrpStack = IoGetNextIrpStackLocation(Irp);

        // copy our Irp stack location to the next Irp stack location
    RtlCopyMemory(NextIrpStack, IrpStack, sizeof(IO_STACK_LOCATION));

        // mark the irp pending
    IoMarkIrpPending(Irp);
    //
    // 下層ﾄﾞﾗｲﾊﾞからの戻り時に介入できるよう、I/O完了ﾙｰﾁﾝを下位ﾄﾞﾗｲﾊﾞのI/Oｽﾀｯｸｽﾛｯﾄにｱﾀｯﾁする
    //
    IoSetCompletionRoutine(Irp,					// set the completion routine
                        ReadCompletionRoutine, 
                        Extension, 
                        TRUE, 
                        TRUE, 
                        TRUE);

        // call the next level driver
    IoCallDriver(Extension->pKeyboardClassDeviceObject, Irp);

    return STATUS_PENDING;
}

//////////////////////////////////////////////////////////////////////
//
//  ReadCompletionRoutine - complete the irp request
//  Input:  DeviceObject  - pointer to the device object
//          Irp           - pointer to the irp sent to the lower driver
//          pContext      - pointer to device extension
//  Output: return        - NT status value
//
//////////////////////////////////////////////////////////////////////

NTSTATUS ReadCompletionRoutine(PDEVICE_OBJECT DeviceObject, PIRP Irp, PVOID pContext)
{		// 下層ﾄﾞﾗｲﾊﾞからの戻り時に介入するRead用I/O完了ﾙｰﾁﾝ
    ULONG                   i;			  // loop variable
    PKEYBOARD_INPUT_DATA    pData;		  // the data coming up
    ULONG                   count;		  // the number of keystrokes

        // get the extension
    PDEVICE_EXTENSION Extension = (PDEVICE_EXTENSION)pContext;

        // propagate the pending status
    if (Irp->PendingReturned) {
        IoMarkIrpPending(Irp);
    }

        // keep count of the number of whitespace keystrokes we've seen
    pData = (PKEYBOARD_INPUT_DATA) Irp->AssociatedIrp.SystemBuffer;
    count = (Irp->IoStatus.Information) / sizeof(KEYBOARD_INPUT_DATA);
    for (i = 0; i < count; i++) {
            // make sure this is a key down
        if ((pData[i]).Flags & KEY_BREAK) {
            continue;
        }
		++KeyCount[ pData[i].MakeCode ];
		kPrint("I: %d %02x %d\n", i, pData[i].MakeCode, KeyCount[ pData[i].MakeCode ]);
            // see if this is whitespace
            // note that the following are the scan codes for space, enter and
            // tab on most keyboards
        if (((pData[i]).MakeCode == 0x39) ||				// SPACE
            ((pData[i]).MakeCode == 0x0f) ||				// TAB
            ((pData[i]).MakeCode == 0x1c)) {				// RETURN
                // make sure we didn't just count a word
            if (!Extension->Whitespace) {
                Extension->Counter++;
                Extension->Whitespace = TRUE;
            }
        } else {
            Extension->Whitespace = FALSE;
        }
    }

    return Irp->IoStatus.Status;
}

///////////////////////////////////////////////////////////////////
//
//  KeyFilterIoctlDispatch - Device I/O control dispatch entry point
//  Input:  DeviceObject   - NT device object
//          Irp            - IRP being dispatched
//  Output: return         - NT status value
//  Notes:  handles only IRP_MJ_DEVICE_CONTROL
//
///////////////////////////////////////////////////////////////////

NTSTATUS KeyFilterIoctlDispatch(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{								// アプリケーションからの要求に応える関数
    NTSTATUS	status;         // intermediate status
    PULONG		BufferInfo;
    ULONG		InSize, OutSize;
    LONG		PassTime;

        // get our device extension
    PDEVICE_EXTENSION Extension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;

        // Get our portion of the irp
    PIO_STACK_LOCATION IrpStack = IoGetCurrentIrpStackLocation(Irp);
    
        // get the next Irp stack location
    PIO_STACK_LOCATION NextIrpStack = IoGetNextIrpStackLocation(Irp);

	kPrint("Ctl %x\n", IrpStack->Parameters.DeviceIoControl.IoControlCode);
	if(IrpStack->Parameters.DeviceIoControl.IoControlCode==IOCTL_KEYCODE_COUNT){
		kPrint("IOCTL_KEYCODE_COUNT\n");
		KeQuerySystemTime( (PLARGE_INTEGER)&CurrentTime );			// 100ns
		CurrentTime -= StartTime;			// 消費時間(100ns単位)
		CurrentTime /= 10000;				// マイクロ -> ミリ
		PassTime	 = (LONG)(CurrentTime/1000);

		BufferInfo    = Irp->AssociatedIrp.SystemBuffer;
		InSize =IrpStack->Parameters.DeviceIoControl.InputBufferLength;
		OutSize=IrpStack->Parameters.DeviceIoControl.OutputBufferLength;
		kPrint("PathTime=%dsecond %d %d\n", PassTime, InSize, OutSize);
		*(BufferInfo++) = PassTime;
		Irp->IoStatus.Information = OutSize;
		OutSize -= 4;
		kPrint("OutSize=%d\n", OutSize);
		RtlCopyMemory(BufferInfo, KeyCount, OutSize);
		//DbgDump(KeyCount, OutSize);

		Irp->IoStatus.Status = STATUS_SUCCESS;
		IoCompleteRequest(Irp, IO_NO_INCREMENT);
		return STATUS_SUCCESS;
	}
        // copy our Irp stack location to the next Irp stack location
    RtlCopyMemory(NextIrpStack, IrpStack, sizeof(IO_STACK_LOCATION));

        // mark the irp pending
    IoMarkIrpPending(Irp);

    //
    // 下層ﾄﾞﾗｲﾊﾞからの戻り時に介入できるよう、I/O完了ﾙｰﾁﾝを下位ﾄﾞﾗｲﾊﾞのI/Oｽﾀｯｸｽﾛｯﾄにｱﾀｯﾁする
    //
    IoSetCompletionRoutine(Irp,					// set the completion routine
                        DefaultCompletionRoutine, 
                        Extension, 
                        TRUE, 
                        TRUE, 
                        TRUE);

        // call the next level driver
    IoCallDriver(Extension->pKeyboardClassDeviceObject, Irp);

    return STATUS_PENDING;
}

///////////////////////////////////////////////////////////////////
//
//  KeyFilterDefaultDispatch - default dispatch entry point
//  Input:  DeviceObject     - NT device object
//          Irp              - IRP being dispatched
//  Output: return           - NT status value
//  Notes:  handles IRP_MJ_CLEANUP, IRP_MJ_FLUSH_BUFFERS, IRP_MJ_CLOSE
//          and IRP_MJ_CREATE
//
///////////////////////////////////////////////////////////////////

NTSTATUS KeyFilterDefaultDispatch(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
        // get our device extension
    PDEVICE_EXTENSION Extension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;

        // get the our Irp stack location
    PIO_STACK_LOCATION IrpStack = IoGetCurrentIrpStackLocation(Irp);

        // get the next Irp stack location
    PIO_STACK_LOCATION NextIrpStack = IoGetNextIrpStackLocation(Irp);

        // copy our Irp stack location to the next Irp stack location
    RtlCopyMemory(NextIrpStack, IrpStack, sizeof(IO_STACK_LOCATION));

        // mark the irp pending
    IoMarkIrpPending(Irp);

    //
    // 下層ﾄﾞﾗｲﾊﾞからの戻り時に介入できるよう、I/O完了ﾙｰﾁﾝを下位ﾄﾞﾗｲﾊﾞのI/Oｽﾀｯｸｽﾛｯﾄにｱﾀｯﾁする
    //
    IoSetCompletionRoutine(Irp,					// set the completion routine
                        DefaultCompletionRoutine, 
                        Extension, 
                        TRUE, 
                        TRUE, 
                        TRUE);

        // call the next level driver
    IoCallDriver(Extension->pKeyboardClassDeviceObject, Irp);

    return STATUS_PENDING;

}

//////////////////////////////////////////////////////////////////////
//
//  DefaultCompletionRoutine - complete the irp request
//  Input:  DeviceObject - pointer to the device object
//          Irp          - pointer to the irp sent to the lower driver
//          pContext     - pointer to device extension
//  Output: return - NT status value
//
//////////////////////////////////////////////////////////////////////

NTSTATUS DefaultCompletionRoutine(PDEVICE_OBJECT  DeviceObject, PIRP Irp, PVOID pContext)
{		// 下層ﾄﾞﾗｲﾊﾞからの戻り時に介入するデフォルトのI/O完了ﾙｰﾁﾝ
        // propagate the pending status
    if (Irp->PendingReturned) {
        IoMarkIrpPending(Irp);
    }

    return Irp->IoStatus.Status;
}
