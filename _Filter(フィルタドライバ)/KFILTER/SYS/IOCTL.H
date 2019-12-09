//	DeviceIoControl IOCTL codes supported by FDCjudge

#define IOCTL_KEYCODE_COUNT CTL_CODE(	\
			FILE_DEVICE_UNKNOWN,		\
			0x800,						\
			METHOD_BUFFERED,			\
			FILE_ANY_ACCESS)

#define IOCTL_KEYCODE_DEBUG CTL_CODE(	\
			FILE_DEVICE_UNKNOWN,		\
			0x80F,						\
			METHOD_BUFFERED,			\
			FILE_ANY_ACCESS)
