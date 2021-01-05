
#include "time_measure.h"

/* avoid stdio dependencie if not needed */
#ifdef DEBUG
#include <stdio.h>
#endif

double PCFreq = 0.0;


BOOL CounterInit(void) {
    LARGE_INTEGER li;
    if(!QueryPerformanceFrequency(&li)) {
#ifdef DEBUG
        printf("QueryPerformanceFrequency failed!\n");
#endif
        return FALSE;
    }

    // result is in milliseconds
    PCFreq = (li.QuadPart)/1000.0;
    return TRUE;
}

double CounterGet(CounterStruct *cnt) {
    return cnt->result_ms;
}

void CounterStart(CounterStruct *cnt) {
    LARGE_INTEGER li;
    cnt->result_ms = 0.0;
    QueryPerformanceCounter(&li);
    cnt->CounterStart = li.QuadPart;
}

double CounterStop(CounterStruct *cnt) {
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    cnt->CounterEnd = li.QuadPart;
    cnt->result_ms = (double)(cnt->CounterEnd - cnt->CounterStart) / PCFreq;
    return CounterGet(cnt);
}

