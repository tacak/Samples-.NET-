#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdarg.h>

#include "debug.h"

#define BUFF_SIZE  (511)


///////////////////////////////////////////////////////////////////////////////


VOID DebugPrint (LPTSTR   szFormat, ... ) {

     TCHAR    szBuffer[BUFF_SIZE + 1];
     INT     nWritten;
     va_list args;

     ::ZeroMemory(szBuffer, sizeof(szBuffer));

     // Format error message like printf()
     
     va_start( args, szFormat );

     nWritten = _vsntprintf( szBuffer, BUFF_SIZE, szFormat, args );

     va_end( args );

     // Output debug string

     ::OutputDebugString( szBuffer );

     
}
