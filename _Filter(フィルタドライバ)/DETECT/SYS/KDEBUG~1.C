//////////////////////////////////////////////////////////////////////////////
//
//  http://www.systeminternals.comから入手できるMark Russinovich氏が作成した
//  DebugViewを使用してデバッグ関数（kPrint)を表示するためのデバッグツール。
//  Remote DebugをTCP/IPでなく、シリアルポートを使って可能なように改良した。
//
//  kPrintは、DbgPrintまたはDebugPrintMsgにリンクする。
//
//////////////////////////////////////////////////////////////////////////////
//

#define	INP		READ_PORT_UCHAR
#define	OUTP	WRITE_PORT_UCHAR

#include	<wdm.h>
#include	<stdarg.h>		// va_list
#include	<stdio.h>		// sprintf

#ifndef	HEADER_
#define HEADER_	"RAMDISK: "			// 8桁
#endif

unsigned short OutputPort=0;		// 0x3f8/0x2f8:Serial
unsigned char  last_moji=0xff;

void kPrint(const char* format, ... );
void DbgDump( void *addr, unsigned int byte);
void DebugPrintMsg(char* Msg);

ULONG ReadDebugPort(PUNICODE_STRING RegistryPath)
{							// レジストリを探索してデバッグ出力先を決める関数
	NTSTATUS			Status;
	HANDLE				hKey;
	OBJECT_ATTRIBUTES	attributes;
	UNICODE_STRING		valname;
    ULONG           	length=0, ValueKey=0x00000046;
	PKEY_VALUE_FULL_INFORMATION vp;
	wchar_t				*pt;

    InitializeObjectAttributes(&attributes,
                               RegistryPath,
                               OBJ_CASE_INSENSITIVE,
                               NULL,
                               NULL
                               );

    Status = ZwOpenKey(&hKey,
                       KEY_READ,
                       &attributes
                       );

    if (!NT_SUCCESS(Status))
    {
    	ZwClose(hKey);
    	return ValueKey;
    }							// DebugViewPort = COM1

	RtlInitUnicodeString(&valname, L"DebugViewPort");
	Status = ZwQueryValueKey(hKey, &valname, KeyValueFullInformation,
							NULL,
							0,
							&length);

	if(Status==STATUS_BUFFER_TOO_SMALL){
		vp = (PKEY_VALUE_FULL_INFORMATION)ExAllocatePool(PagedPool, length);
		Status = ZwQueryValueKey(hKey, &valname, KeyValueFullInformation,
							vp,
							length,
							&length);

		pt = (wchar_t *)((PUCHAR)vp + vp->DataOffset);
		ValueKey = *(pt+3);
		if(ValueKey == 0x0031)		OutputPort = 0x3F8;	// COM1
		else{
			if(*(pt+3) == 0x0032)	OutputPort = 0x2F8;	// COM2
		}												// SELF
		//DbgDump(vp, 64);	// sizeof(KEY_VALUE_PARTIAL_INFORMATION));
		//DbgDump( pt, 16);
    	ExFreePool(vp);
	}
    ZwClose(hKey);
    //kPrint("OutputPort : %x %x\n", OutputPort, ValueKey);
    return ValueKey;
}

void bput1(unsigned char moji)
{
int	i;

	for(i=0;i<800;++i){
		if(INP((unsigned char *)(OutputPort+5)) & 0x20)	break;
		KeStallExecutionProcessor(50);			// 50 microseconds wait
	}
	OUTP((unsigned char *)OutputPort, moji);
}

unsigned int bputc(unsigned char moji)
{
	if(moji == '\n')	bput1(0x0d);
	bput1(moji);
	return((unsigned int)moji);
}

void StringOut(CHAR *p)
{
int	i;
unsigned char	c;

	for(i=0; i<256; ++i){
		c=*(p++);
		if(c==0)	break;
		bputc(c);
	}
}

//////////////////////////////////////////////////////////////////////////////
//	DebugPrint:	Send message event to DebugPrint
//
//	IRQL <= DISPATCH_LEVEL

void DebugPrintMsg(char* Msg)
{
	StringOut(Msg);
///	StringOut("\n");
}

//////////////////////////////////////////////////////////////////////////////
//	DebugPrint:	Formatted print to DebugPrint, allocating own print buffer
//
//	IRQL <= DISPATCH_LEVEL

void kPrint(const char* format, ... )
{
	char* Msg;
	va_list marker;
    LARGE_INTEGER	CurrentTime;
    TIME_FIELDS		TimeFields;
    char*			DbgPt;
    size_t			leng=256;

	Msg = (char*)ExAllocatePool(NonPagedPool, 256);
	if( Msg==NULL)
	{
		DebugPrintMsg("DebugPrint: Could not allocate buffer");
		return;
	}
  if(OutputPort){				  			// 現在時刻を作成する
		KeQuerySystemTime( &CurrentTime );	// 100ns
		//                       GMT    --->   LocalTime,+9*60*60*10000000;9H
		ExSystemTimeToLocalTime(&CurrentTime, &CurrentTime);//W98,not support
		RtlTimeToTimeFields( &CurrentTime, &TimeFields );
		if(last_moji==0xff){
			last_moji=1;
			DbgPt=&Msg[25];					// 25
			sprintf( Msg, "\n%4d-%02d-%02d %02d:%02d:%02d-%03d ",
					TimeFields.Year,
					TimeFields.Month,
					TimeFields.Day,
					TimeFields.Hour,
					TimeFields.Minute,
					TimeFields.Second,
					TimeFields.Milliseconds );
			leng = 256-25;
		}
		else{
			DbgPt=&Msg[10];					// 10
			sprintf( Msg, "%02d:%02d-%03d ",
					TimeFields.Minute,
					TimeFields.Second,
					TimeFields.Milliseconds );
			leng = 256-10;
		}
  }
  else	DbgPt=&Msg[0];						// 0
	va_start(marker, format);
	_vsnprintf(DbgPt, leng, format, marker);
	va_end(marker);
	if(OutputPort)	DebugPrintMsg(Msg);
	else			DbgPrint(Msg);
	ExFreePool(Msg);
}

void MsgOut(const char* format, ... )
{
	char* Msg;
	va_list marker;

	Msg = (char*)ExAllocatePool(NonPagedPool, 256);
	if(Msg==NULL){DebugPrintMsg("DebugPrint: Could not allocate buffer");return;}
	va_start(marker, format);
	_vsnprintf(Msg, 256, format, marker);
	va_end(marker);
	if(OutputPort)	DebugPrintMsg(Msg);
	else			DbgPrint(Msg);
	ExFreePool(Msg);
}

unsigned char	Buf[49], Asc[17];

void DbgDump( void *addr, unsigned int byte) //32ﾋﾞｯﾄ版：DISPLAY １６進アスキ表示
{
	unsigned int	i, j, m=0;
	unsigned char	*kt, *pt, *wt, c;

	if((addr==0) || (byte==0))	return;
	for(i=0; i<16; i+=3)	Buf[i]=0x20;
	Buf[0]=0; Buf[48]=0; Asc[0]=0;
	pt = Buf;

	MsgOut("\n ADDRESS  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  ASCII\n", addr);
	wt = kt = (unsigned char *)addr;

	while(m < byte)
	{
		c = *(kt++);
		sprintf((char *)pt, "%02x ", c);
		j = m % 16;
		if( (c<0x20) || (c>0x7e) )	Asc[j]=0x2e;
		else						Asc[j]=c;
		Asc[j+1]=0;
		pt += 3;
		m++;
		if((m%16)==0){
			Buf[23]='-';
			MsgOut("%08X %s %s\n", wt, Buf, Asc);
			pt = Buf;
			wt = kt;
		}
	}
	j = m % 16;
	if(j!=0){
		if(j<8)	Buf[23]=' ';
		else	Buf[23]='-';
		for(;j<16;++j){
			*(pt++)=0x20;*(pt++)=0x20;++pt;
			Asc[j]=0x20;
		}
		MsgOut("%08X %s %s\n", wt, Buf, Asc);
	}
}
