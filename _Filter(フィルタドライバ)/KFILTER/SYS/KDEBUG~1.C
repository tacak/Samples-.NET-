//////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002-08-20 AIdesign, M.Takiguchi
//
//  http://www.systeminternals.comから入手できるMark Russinovich氏が作成した
//  DebugViewを使用してデバッグ関数（kPrint)を表示するためのデバッグツール。
//  Remote DebugをTCP/IPでなく、シリアルポートを使って可能なように改良した。
//
//  kPrintは、DbgPrintまたはStringOutにリンクする。
//
//////////////////////////////////////////////////////////////////////////////
//

#define	INP		READ_PORT_UCHAR		// HAL関数
#define	OUTP	WRITE_PORT_UCHAR	// HAL関数

#include	<wdm.h>
#include	<stdarg.h>				// va_list
#include	<stdio.h>				// sprintf

#define HEADER_	"AIdesign:"			// 8桁

unsigned short OutputPort=0;		// 0x3f8/0x2f8:Serial
unsigned char  last_moji=0xff;

void kPrint(const char* format, ... );
void DbgDump( void *addr, unsigned int byte);
void StringOut(char* Msg);
//void ExSystemTimeToLocalTime(PLARGE_INTEGER, PLARGE_INTEGER);

NTSTATUS ReadDebugPort(PUNICODE_STRING RegistryPath)
{							// レジストリを探索してデバッグ出力先を決める関数
	NTSTATUS			Status;
	HANDLE				hKey;
	OBJECT_ATTRIBUTES	attributes;
	UNICODE_STRING		valname;
    ULONG           	length=0, ValueKey=0;
	PKEY_VALUE_FULL_INFORMATION vp;
	wchar_t				*pt;

    InitializeObjectAttributes(&attributes,				// ZwOpenKey関数が使う
                               RegistryPath,			// OBJECT_ATTRIBUTES
                               OBJ_CASE_INSENSITIVE,	// 構造体を初期化する
                               NULL,
                               NULL
                               );

    Status = ZwOpenKey(&hKey,					// 既存のレジストリキーを開く
                       KEY_READ,
                       &attributes
                       );

    if (!NT_SUCCESS(Status))
    {
    	ZwClose(hKey);							// 名前付きオブジェクトを閉じる
    	return Status;
    }											// DebugViewPort = COM1

	RtlInitUnicodeString(&valname, L"DebugViewPort");
	Status = ZwQueryValueKey(hKey, &valname, KeyValueFullInformation,
							NULL,
							0,					// ハンドルで指定されたキーの
							&length);			// 値を取得する

	if(Status==STATUS_BUFFER_TOO_SMALL){
		vp = (PKEY_VALUE_FULL_INFORMATION)ExAllocatePool(PagedPool, length);
		Status = ZwQueryValueKey(hKey, &valname, KeyValueFullInformation,
							vp,
							length,				// ハンドルで指定されたキーの
							&length);			// 値を取得する

		pt = (wchar_t *)((PUCHAR)vp + vp->DataOffset);
		if(*(pt+3) == 0x0031)		ValueKey = 0x3F8;	// COM1
		else{
			if(*(pt+3) == 0x0032)	ValueKey = 0x2F8;	// COM2
		}												// SELF
    	OutputPort = (unsigned short)ValueKey;
    	ExFreePool(vp);
	}
   	ZwClose(hKey);								// 名前付きオブジェクトを閉じる
    return Status;
}

void bput1(unsigned char moji)					// キャラクタをシリアルポートに
{												// 出力する
int	i;

	for(i=0;i<800;++i){
		if(INP((unsigned char *)(OutputPort+5)) & 0x20)	break;
		KeStallExecutionProcessor(50);			// 50 microseconds wait
	}
	OUTP((unsigned char *)OutputPort, moji);
}

unsigned int bputc(unsigned char moji)			// LFを出力するとき、その前に
{												// CRを出力する
	if(moji == '\n')	bput1(0x0d);
	bput1(moji);
	return((unsigned int)moji);
}

void StringOut(CHAR *p)							// 文字列を出力する
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

void DebugPrintMsg(char* Msg)					// 指定先に文字列を出力する
{
	if(OutputPort)	StringOut(Msg);				// シリアルポートへ
	else			DbgPrint(Msg);				// Windowsデバッグ機構へ
}

//////////////////////////////////////////////////////////////////////////////
//	DebugPrint:	Formatted print to DebugPrint, allocating own print buffer
//
//	IRQL <= DISPATCH_LEVEL

void kPrint(const char* format, ... )		// 書式付き出力
{
	char* Msg;
	va_list marker;
    LARGE_INTEGER	CurrentTime;
    TIME_FIELDS		TimeFields;
    char*			DbgPt;
    size_t			leng=256;

	Msg = (char*)ExAllocatePool(NonPagedPool, 256);		// 領域確保
	if( Msg==NULL)
	{
		StringOut("DebugPrint: Could not allocate buffer");
		return;
	}
	if(last_moji==0xff){								// 開始メッセージ
		last_moji=0xfe;
		DebugPrintMsg("\n<<< Kernel-mode tiny-Debugger >>>, Copyright(c) 2002 AIdesign, M.Takiguchi\n");
	}

  if(OutputPort){				  			// 現在時刻を作成する
		KeQuerySystemTime( &CurrentTime );	// 100ns
		//                       GMT    --->   LocalTime,+9*60*60*10000000;9H
		ExSystemTimeToLocalTime(&CurrentTime, &CurrentTime);//W98,not support
		RtlTimeToTimeFields( &CurrentTime, &TimeFields );
		if(last_moji==0xfe){
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
	_vsnprintf(DbgPt, leng, format, marker);// フォーマット変換
	va_end(marker);

	DebugPrintMsg(Msg);
	ExFreePool(Msg);						// 領域開放
}

void MsgOut(const char* format, ... )		// DbgDumpで使う書式付き出力
{
	char* Msg;
	va_list marker;

	Msg = (char*)ExAllocatePool(NonPagedPool, 256);
	if(Msg==NULL){StringOut("DebugPrint: Could not allocate buffer");return;}
	va_start(marker, format);
	_vsnprintf(Msg, 256, format, marker);	// フォーマット変換
	va_end(marker);
	if(OutputPort)	StringOut(Msg);			// 出力先を決める
	else			DbgPrint(Msg);
	ExFreePool(Msg);
}

void DbgDump( void *addr, unsigned int byte) // DISPLAY １６進数とアスキー表示
{											 // １行に１６バイト出力する
	unsigned int	j, m=0;
	unsigned char	*kt, *pt, *wt, c;
	unsigned char	Buf[49], Asc[17];

	if((addr==0) || (byte==0))	return;
	for(j=0; j<16; j+=3)	Buf[j]=0x20;
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