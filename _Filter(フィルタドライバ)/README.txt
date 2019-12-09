付属CD-ROMについて


（１）免責事項
　収録したソースコードは筆者の開発環境で動作を確認してありますが、動作を保証するものではありません。筆者および技術評論社は、使用にあたって生じたトラブル等について、一切の責任を負いません。
　また、プログラムリストは、著作権法により保護されています。個人で使用する目的以外には使用することができません。ネットワークへのアップロードなどは著者の許可なく行うことはできません。


（２）収録ファイル一覧
　ルートフォルダには、6つのサブフォルダと2つのテキストファイルおよびREADME.txt（このファイル）があります。
　サブフォルダは、2つのフォルダSYS（ANALYSIS、KFILTER、XFLOPPY、DETECT、ACCESS）とEXE（ANALYSIS、KFILTER、ACCESS、LAUNCH）からなっています。SYSはドライバ、EXEは動作を確認するアプリケーションです。
　SYSではBUILDユーティリティ、EXEでは統合環境コンパイラを使うことを前提にしています。統合環境コンパイラを用意できない場合は、本文の巻末付録「CL.exeによるアプリケーション作成例」が参考になります。
　なお、第6章で作成している「簡易カーネルモードデバッガ」は「kdebgView.c」のファイル名で各SYSフォルダの中に入っています。


○README.txt	：説明書（このファイル）

○NTSTATUS.TXT	：巻末付録「NTステータスコード一覧」

○WINERROR.TXT	：巻末付録「Win32エラーコード一覧」

○［ANALYSIS］	：第7章「既存ドライバ動作の解析用フィルタドライバ」
SYS<フォルダ>
　　SOURCES
　　MAKEFILE
　　Make.bat
　　ANALYSIS.C
　　KdebugView.c
　　ANALYSIS.RC
　　ANALYSIS.H
　　DEVEXT.H
　　SYSNAME.H
　　KdebugView.h
　　ANALYSIS.INF
　　ANALYSIS.REG
EXE<フォルダ>
　　ACOUNT.DSP
　　ACOUNT.DSW 
　　ACOUNT.C 
　　ACOUNT.H 
　　ACOUNT.RC
　　ACOUNT.EXE

○［KFILTER］	：第8章「システムで発生している特定現象を解析するフィルタドライバ」
　SYS<フォルダ>
　　SOURCES
　　MAKEFILE
　　MAKE.BAT
　　kFILTER.c
　　KdebugView.c
　　Ioctl.h
　　KdebugView.h
　　kFILTER.h
　　NTDDKBD.H
　　kFILTER.reg
　　kFILTER.sys
　EXE<フォルダ>
　　kCOUNT.dsp
　　kCOUNT.dsw
　　kCOUNT.c
　　kCOUNT.rc
　　kCOUNT.h
　　DLLCOUNT.lib
　　DLLCOUNT.DLL
　　kCOUNT.exe

○ [XFLOPPY］	：第9章「下位ドライバ動作を変更する暗号化・復号化システム」
　SYS<フォルダ>
　　SOURCES
　　MAKEFILE
　　Make.bat
　　XFLOPPY.C
　　KdebugView.c
　　XFLOPPY.RC
　　DEVEXT.H
　　KdebugView.h
　　SYSNAME.H
　　XFLOPPY.H
　　XFLOPPY.INF
　　XFLOPPY.REG
　　XFLOPPY.SYS

○［DETECT］	：第10章「FDDの機能を複合的に向上させるフィルタドライバ」
　SYS<フォルダ>
　　SOURCES
　　MAKEFILE
　　Make.bat
　　DETECT.C
　　KdebugView.c
　　DETECT.RC
　　DETECT.H
　　DEVEXT.H
　　KdebugView.h
　　SYSNAME.H
　　DETECT.INF
　　DETECT.REG
　　DETECT.SYS

○［ACCESS］	：第11章「機器利用課金システムへの応用例」
　SYS<フォルダ>
　　ACCESS.C
　　KdebugView.c
　　ACCESS.RC
　　ACCESS.H
　　KdebugView.h
　　DEVEXT.H
　　SYSNAME.H
　　MAKEFILE
　　SOURCES
　　Make.bat
　　ACCESS.INF
　　ACCESS.SYS
　EXE<フォルダ>
　　README.TXT
　　Report.c
　　MACWIN32.H
　　MAKE.BAT
　　MAKEFILE
　　SOURCES
　　Report.exe

○［LAUNCH］	：巻末付録「INFファイルを使わないドライバインストーラの作成方法」
　EXE<フォルダ>
　　LAUNCH.c
　　INSTALL.c
　　UdebugView.c
　　LAUNCH.RC
　　LAUNCH.h
　　UdebugView.h
　　LAUNCH.dsp
　　LAUNCH.dsw
　　LAUNCH.exe

（３）開発環境と実行環境

◆コンパイラ
　　○ドライバ　　　　：\WINDDK\2600\X86\CL.exe　Version 13.00.9176
　　○アプリケーション：Visual C++6.0　Professional Edition

◆DDK
　　Microsoft Windows XP DDK

