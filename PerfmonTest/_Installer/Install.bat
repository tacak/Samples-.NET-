copy PerfTestDLL.dll %SYSTEMROOT%\system32
reg import PerfTestInstall.reg
lodctr perfdll_test.ini