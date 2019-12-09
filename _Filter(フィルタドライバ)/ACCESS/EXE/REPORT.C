// 2002-11-29B
#include <windows.h>
#include <stdio.h>
#define	IRP_MJ_MAXIMUM_FUNCTION	0x1b

ULONG	Function[IRP_MJ_MAXIMUM_FUNCTION+1];	// 稼動状況格納領域

ULONG	Tanka[IRP_MJ_MAXIMUM_FUNCTION+1]={		// 単価テーブル
        100,// "IRP_MJ_CREATE",                   // 0x00
        100,// "IRP_MJ_CREATE_NAMED_PIPE",        // 0x01
        100,// "IRP_MJ_CLOSE",                    // 0x02
        100,// "IRP_MJ_READ",                     // 0x03
        140,// "IRP_MJ_WRITE",                    // 0x04
        140,// "IRP_MJ_QUERY_INFORMATION",        // 0x05
        400,// "IRP_MJ_SET_INFORMATION",          // 0x06
        500,// "IRP_MJ_QUERY_EA",                 // 0x07
        160,// "IRP_MJ_SET_EA",                   // 0x08
        200,// "IRP_MJ_FLUSH_BUFFERS",            // 0x09
        200,// "IRP_MJ_QUERY_VOLUME_INFORMATION", // 0x0a
        120,// "IRP_MJ_SET_VOLUME_INFORMATION",   // 0x0b
        120,// "IRP_MJ_DIRECTORY_CONTROL",        // 0x0c
        120,// "IRP_MJ_FILE_SYSTEM_CONTROL",      // 0x0d
        120,// "IRP_MJ_DEVICE_CONTROL",           // 0x0e
        300,// "IRP_MJ_INTERNAL_DEVICE_CONTROL",  // 0x0f
        300,// "IRP_MJ_SHUTDOWN",                 // 0x10
        460,// "IRP_MJ_LOCK_CONTROL",             // 0x11
        450,// "IRP_MJ_CLEANUP",                  // 0x12
        350,// "IRP_MJ_CREATE_MAILSLOT",          // 0x13
        180,// "IRP_MJ_QUERY_SECURITY",           // 0x14
        220,// "IRP_MJ_SET_SECURITY",             // 0x15
        400,// "IRP_MJ_QUERY_POWER",              // 0x16
        390,// "IRP_MJ_SET_POWER",                // 0x17
        440,// "IRP_MJ_DEVICE_CHANGE",            // 0x18
        200,// "IRP_MJ_QUERY_QUOTA",              // 0x19
        270,// "IRP_MJ_SET_QUOTA",                // 0x1a
        170 // "IRP_MJ_PNP_POWER"                 // 0x1b
};

void _cdecl main(int argc, char *argv[])	// Dynamically load the supporting WDM
{
	unsigned int	i=8, j, k=4, l, Yen=0;
	HANDLE			Handle;
			// デバイスオブジェクトを開き、利用可能なハンドルを取得する
	Handle=CreateFile("\\\\.\\A:",GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|
		FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	printf("課金システム Ver1.00\n");
	if(Handle == INVALID_HANDLE_VALUE){
		printf("ERROR-100 %x\n", GetLastError());
		return;
	}
		// 指定ﾃﾞﾊﾞｲｽﾄﾞﾗｲﾊﾞへ制御ｺｰﾄﾞを送信し、対応するﾃﾞﾊﾞｲｽに特定動作をさせる
	l = DeviceIoControl(Handle, 8192, &k, 4, Function, 4*(IRP_MJ_MAXIMUM_FUNCTION+1), &j, NULL);
	//printf("I=%d, J=(%d), L=%d\n", i, j, l);
	printf(" # Function 単価   金額\n");
	for(i=0; i<=IRP_MJ_MAXIMUM_FUNCTION; ++i){	// 項目別課金額一覧表を作成
		k = Function[i] * Tanka[i];
		Yen += k;
		printf("%2d %5d %6d%8d\n", i, Function[i], Tanka[i], k);
	}
	printf("合計金額 %d\n", Yen);
	CloseHandle(Handle);						// 開いているｵﾌﾞｼﾞｪｸﾄﾊﾝﾄﾞﾙを閉じる
	//printf("\n処理終了\n");
}
