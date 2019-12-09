/******************************************************************************
*
*       DEVEXT.H - Windows XP Driver Device Extension Definition
*
*               Copyright (c) 2002 AIdesign
*
*       PURPOSE: Define the XP driver device extension.
*
******************************************************************************/
#ifndef _DEVEXT_H_
#define _DEVEXT_H_

//
// Device Extension
//

typedef struct _DEVICE_EXTENSION {

    //
    // Back pointer to device object
    //

    PDEVICE_OBJECT  DeviceObject;

    //
    // Previous Device Object
    //

    PDEVICE_OBJECT  PreviousDeviceObject;

    //
    // Target Device Object
    //

    PDEVICE_OBJECT  TargetDeviceObject;

    //
    // Verify Device Object
    //

    PDEVICE_OBJECT  VerifyDeviceObject;

    //
    // Floppy Disk Counter
    //

    ULONG   Count;

    //
    // Virtual Floppy Disk Flag
    //

    BOOLEAN bVirtualFloppy;

    //
    // Verify Request Flag/SpinLock
    //

    BOOLEAN bVerifyRequired;

    KSPIN_LOCK VerifySpinLock;

} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

#define DEVICE_EXTENSION_SIZE sizeof(DEVICE_EXTENSION)

#endif // _DEVEXT_H_

