////////////////////////////////////////////////////////////
//
//  kFILTER.h - definition of KeyFilter
//  Copyright (c) 2002, AiDesign, M.Takiguchi
//
////////////////////////////////////////////////////////////

#define HEADER_					"kFILTER:"			// 8Œ…
#define KFILTER_DEVICE_TYPE		FILE_DEVICE_KEYBOARD

#define PHD_DEVICE_NAME	L"\\Device\\KFILTER"		// kFILTERÄÞ×²ÊÞ‚ÌÃÞÊÞ²½–¼
#define WIN32_PATH		L"\\DosDevices\\kCOUNT"		// WIN32 PATH–¼

    // resource assignment types
typedef enum _WINDK_RESOURCE_ASSIGN_TYPE 
{
    RESOURCE_ASSIGN_NONE, 
    RESOURCE_ASSIGN_IOASSIGN, 
    RESOURCE_ASSIGN_IOREPORT
} WINDK_RESOURCE_ASSIGN_TYPE, *PWINDK_RESOURCE_ASSIGN_TYPE;

    // information structure for WinDK device objects
typedef struct _WKDEV_INFORMATION {
#if defined(WINDK_ENV_PNP) 
    PDEVICE_OBJECT      pPhysicalDeviceObject;// pointer to the physical device object
    PDEVICE_OBJECT      pStackDeviceObject; // pointer to the device object below us
    WKPNP_INFORMATION   PnpInfo;            // pnp information
#endif
    ULONG               OpenCount;          // number of handles open to driver
    BOOLEAN             DeviceExclusive;    // the device has been opened 
                                            //  exclusively
    PDRIVER_OBJECT      pDriverObject;      // the NT driver object for this device
    PDEVICE_OBJECT      pDeviceObject;      // the NT device object for this device
    UNICODE_STRING      DeviceName;         // name of this device
    UNICODE_STRING      SymbolicLink;       // symbolic link name
    UNICODE_STRING      DeviceClassName;    // device Class name for assign of 
                                            //  resources
    UNICODE_STRING      RegPathPrepend;     // key prepended to device name for
                                            //  assign resources
    UNICODE_STRING      RegPathPostpend;    // key postpended to device name for
                                            //  assign resources
    KIRQL               InterruptIrql;      // HalGetInterruptVector values: interrupt
                                            //  IRQL
    KAFFINITY           InterruptAffinity;  // processor affinity
    ULONG               InterruptVector;    // assigned vector
    PKINTERRUPT         pInterruptObject;   // the interrupt object
    KSPIN_LOCK          ClassSpinLock;      // class device extension protection 
    KIRQL               OldIrql;            // old IRQL from acquiring spinlock
    UNICODE_STRING      ResourceRegistryPath; // path used for IoAssignResource
    WINDK_RESOURCE_ASSIGN_TYPE ResourceTypeAssign; // method used to assign resources
    BOOLEAN             Initialized;        // indicated if the structure is initialized
} WKDEV_INFORMATION, *PWKDEV_INFORMATION;

    // keyfilt device extension
typedef struct _DEVICE_EXTENSION {
    WKDEV_INFORMATION   Information;				// WinDK device information
    PDEVICE_OBJECT      pKeyboardClassDeviceObject;	// keyboard class device object
    BOOLEAN             Whitespace;					// white space
    ULONG               Counter;					// count
	IO_REMOVE_LOCK		RemoveLock;					// removal control locking structure
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

    // the increment applied to completed requests
#define KFILTER_IO_INCREMENT            IO_NO_INCREMENT

    // DriverEntry routine
NTSTATUS DriverEntry(PDRIVER_OBJECT   pDriverObject,
                              PUNICODE_STRING  pRegistryPath);

    // create a single keyfilt device
NTSTATUS CreateKeyFilterDevice(   IN  PDRIVER_OBJECT      pDriverObject,
                                OUT PDEVICE_OBJECT      *ppDeviceObject);

    // delete a singe keyfilt device
VOID DeleteKeyFilterDevice(IN PDEVICE_OBJECT pDeviceObject);
    
    // dispatch IRP_MJ_READ requests
NTSTATUS KeyFilterReadDispatch(PDEVICE_OBJECT pDeviceObject,
                                      PIRP           pIrp);

    // complete IRP_MJ_READ requests
NTSTATUS ReadCompletionRoutine(PDEVICE_OBJECT  pDeviceObject, 
                               PIRP            pIrp, 
                               PVOID           pContext);

    // complete IRP_MJ_READ requests
NTSTATUS DefaultCompletionRoutine(PDEVICE_OBJECT  pDeviceObject, 
                                  PIRP            pIrp, 
                                  PVOID           pContext);

    // dispatch IRP_MJ_DEVICE_CONTROL requests
NTSTATUS KeyFilterIoctlDispatch(PDEVICE_OBJECT pDeviceObject,
                                       PIRP           pIrp);

    // dispatch IRP_MJ_CLEANUP, IRP_MJ_FLUSH_BUFFERS, IRP_MJ_CREATE, and 
    // IRP_MJ_CLOSE requests
NTSTATUS KeyFilterDefaultDispatch(PDEVICE_OBJECT pDeviceObject,
                                         PIRP           pIrp);
