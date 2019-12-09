// FDLAUNCH.H
#define IDM_DIALOG1  100
#define IDM_DIALOG2  101
#define IDM_EXIT     102
#define IDM_HELP     103

#define IDD_RED      104
#define IDD_GREEN    105
#define IDD_SELFRUIT 106
#define ID_LB1       107

#define ID_EB1       108

#ifndef _H_INSTDRV_
#define _H_INSTDRV_

BOOL InstallDriver( const char * Name, const char * Path);
BOOL UnInstallDriver( const char * Name );

#endif // _H_INSTDRV_
