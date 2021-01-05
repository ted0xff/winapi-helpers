#ifndef __TIME_MEASURE_H_
#define __TIME_MEASURE_H_


#include <windows.h>

/** Storage for Counter data */
typedef struct {
    __int64 CounterStart;
    __int64 CounterEnd;
    double result_ms;
}CounterStruct;



/**
 * @brief CounterInit Initialize counter reference
 *
 * @return FALSE when counter may not work
 * @return TRUE  when init counter is ok
 */
BOOL CounterInit(void);

/**
 * @brief CounterStart Start counter and store ref
 *
 * @param[in] cnt time structure to store data
 */
void CounterStart(CounterStruct *cnt);

/**
 * @brief CounterGet return time measured
 *
 * Shall be called after CounterStop()
 * 
 * @param[in] cnt time structure to store data
 * 
 * @return measured time in [ms]
 */
double CounterGet(CounterStruct *cnt);

/**
 * @brief CounterStop Stop counter and return time measured
 *
 * @param[in] cnt time structure to store data
 * 
 * @return measured time in [ms]
 */
double CounterStop(CounterStruct *cnt);

#endif /* __TIME_MEASURE_H_ */
