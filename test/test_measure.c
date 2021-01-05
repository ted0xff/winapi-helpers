#include<stdio.h>
#include"time_measure.h"

int main( void ) {

    CounterStruct counter1;
    CounterStruct counter2;

    if( FALSE == CounterInit() ){
        printf("Failed Counter Init \n");
        return -1;
    }

    CounterStart(&counter2);
    CounterStart(&counter1);

    Sleep( 222 );

    printf("Counter1: %.3f\n", CounterStop(&counter1) );
    printf("Counter2: %.3f\n", CounterStop(&counter2) );

    return 0;
}

