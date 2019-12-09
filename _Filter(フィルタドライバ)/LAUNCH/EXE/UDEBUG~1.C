//////////////////////////////////////////////////////////////////////////////
//
//  UdebugView.c		2002-11-20
//
//	Copyright(C) 2002 M.Takiguchi, AIdesign.
//
//  <<< 書式付きデバッグ関数（uPrint)でデバッグ情報を表示するツール >>>
//
//  http://www.systeminternals.comから入手できるMark Russinovich氏が作成した
//  DebugViewを使用してprintf感覚でデバッグ画面に表示できる。uPrintは、フォー
//  マット変換後、OutputDebugStringにリンクする。
//
//  type : user-mode
//
//////////////////////////////////////////////////////////////////////////////

#include	<windows.h>
#include	<winbase.h>			// OutputDebugString
#include	<stdarg.h>			// va_list
#include	<stdio.h>			// sprintf

//////////////////////////////////////////////////////////////////////////////

void uPrint(const char* format, ... )
{
	char	buf[256];
	va_list marker;

	va_start(marker, format);
	_vsnprintf(buf, 256, format, marker);
	va_end(marker);
	OutputDebugString(buf);
}

void DbgDump( void *addr, unsigned int byte)	// DISPLAY １６進,アスキ表示
{
	unsigned char	Buf[49], Asc[17];
	unsigned int	i, j, m=0;
	unsigned char	*kt, *pt, *wt, c;

	if((addr==0) || (byte==0))	return;
	for(i=0; i<16; i+=3)	Buf[i]=0x20;
	Buf[0]=0; Buf[48]=0; Asc[0]=0;
	pt = Buf;

	uPrint("\n ADDRESS  0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F  ASCII\n", addr);
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
			uPrint("%08X %s %s\n", wt, Buf, Asc);
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
		uPrint("%08X %s %s\n", wt, Buf, Asc);
	}
}
