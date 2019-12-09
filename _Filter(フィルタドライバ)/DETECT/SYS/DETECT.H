/******************************************************************************
*
*       DETECT.H - Windows XP Filter Floppy Module Definition
*
*               Copyright (c) 2002 AIdesign
*
*       PURPOSE: Define the driver I/O control module prototype.
*
******************************************************************************/
#ifndef _DETECT_H_
#define _DETECT_H_
#include "sysname.h"

#pragma pack(1)

typedef struct  _BOOT_SECTOR
{
    UCHAR       bsJump[3];          // x86 jmp instruction, checked by FS
    CCHAR       bsOemName[8];       // OEM name of formatter
    USHORT      bsBytesPerSec;      // Bytes per Sector
    UCHAR       bsSecPerClus;       // Sectors per Cluster
    USHORT      bsResSectors;       // Reserved Sectors
    UCHAR       bsFATs;             // Number of FATs - we always use 1
    USHORT      bsRootDirEnts;      // Number of Root Dir Entries
    USHORT      bsSectors;          // Number of Sectors
    UCHAR       bsMedia;            // Media type
    USHORT      bsFATsecs;          // Number of FAT sectors
    USHORT      bsSecPerTrack;      // Sectors per Track - we use 63
    USHORT      bsHeads;            // Number of Heads - we use 32
    ULONG       bsHiddenSecs;       // Hidden Sectors - we set to 63
    ULONG       bsHugeSectors;      // Number of Sectors if > 32 MB size
    UCHAR       bsDriveNumber;      // Drive Number - not used
    UCHAR       bsReserved1;        // Reserved
    UCHAR       bsBootSignature;    // New Format Boot Signature - 0x29
    ULONG       bsVolumeID;         // VolumeID - 
    CCHAR       bsLabel[11];        // Label - 
    CCHAR       bsFileSystemType[8];// File System Type - FAT12 or FAT16
    CCHAR       bsReserved2[448];   // Reserved
    UCHAR       bsSig2[2];          // Boot Signature - 0x55, 0xAA
}   BOOT_SECTOR, *PBOOT_SECTOR;

#pragma pack()

PULONG GetFloppyCount();

#endif // _DETECT_H_

NTSTATUS
DeviceCreate(
    IN PDRIVER_OBJECT DriverObject
    );

NTSTATUS
DetectRead(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
DetectWrite(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
DetectDeviceCtl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
DetectOpenClose(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

NTSTATUS
DetectAddDevice(
    IN PDRIVER_OBJECT DriverObject,
    IN PDEVICE_OBJECT pdo
    );

NTSTATUS
DetectUnload(
    IN PDRIVER_OBJECT DriverObject
    );

//////////////////////////////////////////////////////////////////
//
// Spinlock Macros
//

#define MUTEX_TYPE      KSPIN_LOCK
#define MUTEX_INIT(v)   KeInitializeSpinLock( &v );
#define MUTEX_P(v, o)   KeAcquireSpinLock( &v, o );   
#define MUTEX_V(v, n)   KeReleaseSpinLock( &v, n );   

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

/*++

Routine Description:

    Debug print the major function code.

Arguments:

    Major function code
    I/O control code (optional)

Return Value:

    None

--*/

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
