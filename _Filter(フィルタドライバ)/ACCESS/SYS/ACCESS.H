/******************************************************************************
*
*       ACCESS.H - Windows XP Filter Floppy Module Definition
*
*               Copyright (c) 2002 AIdesign
*
*       PURPOSE: Define the driver I/O control module prototype.
*
******************************************************************************/
#ifndef _ACCESS_H_
#define _ACCESS_H_
#include "sysname.h"

#endif // _ACCESS_H_

NTSTATUS
AccessCommon(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
AccessAddDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT pdo
    );

VOID
AccessUnload(
    IN PDRIVER_OBJECT DriverObject
    );

NTSTATUS
AcDeviceCreate(
    IN PDRIVER_OBJECT DriverObject
    );

NTSTATUS
AcIoCompletionRoutine(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP           Irp,
    IN PVOID          Context
    );

//////////////////////////////////////////////////////////////////

//
// GetFunctionCode Macro
//
#define GetFunctionCode(x) ((x>>2)&0xFFF)

#if 1	// DBG
    UCHAR *MJTable[IRP_MJ_MAXIMUM_FUNCTION+1] = {
        "IRP_MJ_CREATE",                   // 0x00
        "IRP_MJ_CREATE_NAMED_PIPE",        // 0x01
        "IRP_MJ_CLOSE",                    // 0x02
        "IRP_MJ_READ",                     // 0x03
        "IRP_MJ_WRITE",                    // 0x04
        "IRP_MJ_QUERY_INFORMATION",        // 0x05
        "IRP_MJ_SET_INFORMATION",          // 0x06
        "IRP_MJ_QUERY_EA",                 // 0x07
        "IRP_MJ_SET_EA",                   // 0x08
        "IRP_MJ_FLUSH_BUFFERS",            // 0x09
        "IRP_MJ_QUERY_VOLUME_INFORMATION", // 0x0a
        "IRP_MJ_SET_VOLUME_INFORMATION",   // 0x0b
        "IRP_MJ_DIRECTORY_CONTROL",        // 0x0c
        "IRP_MJ_FILE_SYSTEM_CONTROL",      // 0x0d
        "IRP_MJ_DEVICE_CONTROL",           // 0x0e
        "IRP_MJ_INTERNAL_DEVICE_CONTROL",  // 0x0f
        "IRP_MJ_SHUTDOWN",                 // 0x10
        "IRP_MJ_LOCK_CONTROL",             // 0x11
        "IRP_MJ_CLEANUP",                  // 0x12
        "IRP_MJ_CREATE_MAILSLOT",          // 0x13
        "IRP_MJ_QUERY_SECURITY",           // 0x14
        "IRP_MJ_SET_SECURITY",             // 0x15
        "IRP_MJ_QUERY_POWER",              // 0x16
        "IRP_MJ_SET_POWER",                // 0x17
        "IRP_MJ_DEVICE_CHANGE",            // 0x18
        "IRP_MJ_QUERY_QUOTA",              // 0x19
        "IRP_MJ_SET_QUOTA",                // 0x1a
        "IRP_MJ_PNP_POWER"                 // 0x1b
    };

PUCHAR
GetMajorFunctionString(
    ULONG MajorFunction,
    ULONG IoControlCode
    )
{
    static UCHAR Buffer[128];

    sprintf(Buffer,"IRP_MJ_UNKNOWN(%X)",MajorFunction);
    if (MajorFunction<=IRP_MJ_MAXIMUM_FUNCTION) {
        if (MajorFunction==IRP_MJ_DEVICE_CONTROL) {
            sprintf(Buffer,"%s(Code=%Xh)",
                MJTable[MajorFunction], 
                GetFunctionCode(IoControlCode));
        } else
        if (MajorFunction==IRP_MJ_INTERNAL_DEVICE_CONTROL) {
            sprintf(Buffer,"%s(InternalCode=%Xh)",
                MJTable[MajorFunction], 
                GetFunctionCode(IoControlCode));
        } else
        sprintf(Buffer,"%s",MJTable[MajorFunction]);
    }
    return Buffer;

}
#endif
