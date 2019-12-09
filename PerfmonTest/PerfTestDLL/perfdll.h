#pragma once 

#include <windows.h>
#include <WinPerf.h>

typedef struct _MYPERF_DATA {
     PERF_OBJECT_TYPE pot;
     PERF_COUNTER_DEFINITION pcd1;
     PERF_COUNTER_DEFINITION pcd2;
     PERF_COUNTER_BLOCK pcb;
     LONG lInstanceCount;
     LONG lPerf1;
} MYPERF_DATA, *PMYPERF_DATA;
