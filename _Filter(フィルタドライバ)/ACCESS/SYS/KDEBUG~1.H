//////////////////////////////////////////////////////////////////////////////
//	DebugPrint code header to add to a test driver
//
/////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C"
{
#endif

#define xprintf	kPrint
#define	dumpx	DbgDump

void kPrint(const char* format, ... );
void DbgDump( void *addr, unsigned int byte);
NTSTATUS ReadDebugPort(PUNICODE_STRING RegistryPath);
extern unsigned short OutputPort;

#ifdef __cplusplus
}
#endif

#define DBGPRINT( Component, Level, Fmt )	\
	{										\
		kPrint Fmt;							\
	}

/////////////////////////////////////////////////////////////////////////////
