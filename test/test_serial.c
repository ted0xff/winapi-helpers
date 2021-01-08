
#include<stdio.h>
#include<stdlib.h>

#include"serial.h"
#include"time_measure.h"

int main(int argc, char *argv[]) {
    HANDLE hComm;
    char serialPort[32];

    printf("Version : %s %s\n", __DATE__, __TIME__);

    printf("\nList all found serial ports:\n");
    for (int i = 1; i < 255; ++i){
        if (serial_exist( i)) {
            printf( " COM%d exists\n", i);
        }
    }
    
    
    if(argc == 1) {
        strncpy(serialPort,"\\\\.\\COM1", 32);
    } else {
        int portnum;
        portnum = strtol(argv[1],NULL,10);
        /* in case of letter instead of number, return 0 */
        if( portnum == 0 ) {
            printf("Error COMPORT number, put a number as argument : 8 for COM8.\n");
            return -1;
        }else {
            snprintf(serialPort, 32, "\\\\.\\COM%ld", strtol(argv[1],NULL,10) );
        }
    }

    printf("PC: open port %s\n", serialPort);


    // data to send
    char lpBuffer[1024] = "123456789abcdefghijklmnopqrstuvwxyzFFX ";
    DWORD dNoOFBytestoWrite;         // No of bytes to write into the port
    DWORD dNoOfBytesWritten = 0;     // No of bytes written to the port

    strcat(lpBuffer, __TIME__);
    strcat(lpBuffer, "\n");
    dNoOFBytestoWrite = strlen(lpBuffer);

    // data read
    char ReadBuffer[1024];
    DWORD dNoOfBytesReallyRead = 0;

    DWORD inqueue=0, outqueue=0;


    printf("***Open***\n");
    hComm = serial_open( serialPort);
    if(NULL == hComm ) {
        printf("Failed open %s\n", serialPort);
        return -1;
    }

    printf("***Config***\n");
    if(FALSE == serial_config(hComm, CBR_115200 /*CBR_9600*/, NOPARITY)) {
        printf("Failed config %s %d %d\n", serialPort, CBR_115200, NOPARITY);
        return -1;
    }

    printf("***Initial status***\n");
    /* check fifo */{
        inqueue=0, outqueue=0;
        if( FALSE == serial_check_error(hComm, &inqueue, &outqueue, NULL) ) {
            printf("There were errors on serial_check_error()\n");
        }
        printf("FIFO status: input %d, output %d\n", inqueue, outqueue);
    }


    printf("***Write***\n");
    serial_write(hComm, lpBuffer, dNoOFBytestoWrite, &dNoOfBytesWritten);
    printf("Bytes written: %d / %d\n", dNoOfBytesWritten, dNoOFBytestoWrite);

    /* check fifo */{
        inqueue=0, outqueue=0;
        if( FALSE == serial_check_error(hComm, &inqueue, &outqueue, NULL) ) {
            printf("There were errors on serial_check_error()\n");
        }
        printf("FIFO status: input %d, output %d\n", inqueue, outqueue);
    }

    printf("***Read***\n");
    memset(ReadBuffer, '\0', 1024);
    serial_read(hComm, ReadBuffer, inqueue, &dNoOfBytesReallyRead);
    printf("Bytes read: %d / %d\n", dNoOfBytesReallyRead, inqueue);
    printf(" data --%s--\n", ReadBuffer);

    /* check fifo */{
        inqueue=0, outqueue=0;
        if( FALSE == serial_check_error(hComm, &inqueue, &outqueue, NULL) ) {
            printf("There were errors on serial_check_error()\n");
        }
        printf("FIFO status: input %d, output %d\n", inqueue, outqueue);
    }

    printf("***Status***\n");
    /* check fifo */{
        inqueue=0, outqueue=0;
        if( FALSE == serial_check_error(hComm, &inqueue, &outqueue, NULL) ) {
            printf("There were errors on serial_check_error()\n");
        }
        printf("FIFO status: input %d, output %d\n", inqueue, outqueue);
    }

    printf("***Flush***\n");
    serial_flush( hComm ); 

    printf("***Status***\n");
    /* check fifo */{
        inqueue=0, outqueue=0;
        if( FALSE == serial_check_error(hComm, &inqueue, &outqueue, NULL) ) {
            printf("There were errors on serial_check_error()\n");
        }
        printf("FIFO status: input %d, output %d\n", inqueue, outqueue);
    }

    printf("***Read***\n");
    memset(ReadBuffer, '\0', 1024);
    serial_read(hComm, ReadBuffer, inqueue, &dNoOfBytesReallyRead);
    printf("Bytes read: %d / %d\n", dNoOfBytesReallyRead, inqueue);
    printf("data --%s--\n", ReadBuffer);

    /* check fifo */{
        inqueue=0, outqueue=0;
        if( FALSE == serial_check_error(hComm, &inqueue, &outqueue, NULL) ) {
            printf("There were errors on serial_check_error()\n");
        }
        printf("FIFO status: input %d, output %d\n", inqueue, outqueue);
    }


    CounterStruct cnt;
    float speed, baudrate_estimate;

    CounterInit();
    printf("***Write***\n");
    dNoOFBytestoWrite=1024;
    CounterStart(&cnt);
    serial_write(hComm, lpBuffer, dNoOFBytestoWrite, &dNoOfBytesWritten);
    CounterStop(&cnt);

    /* ko/s */
    speed = 1024.0/CounterGet(&cnt);
    baudrate_estimate = speed * (8+2) * 1000 ; /* start, 8bit, stop(, interframe) */

    printf("Bytes written: %d / %d en %.3fms\n", dNoOfBytesWritten, dNoOFBytestoWrite, CounterGet(&cnt) );
    printf(" -> %.3fko/s -- baud  estimated: %.1f\n", speed, baudrate_estimate );

    /* check fifo */{
        inqueue=0, outqueue=0;
        if( FALSE == serial_check_error(hComm, &inqueue, &outqueue, NULL) ) {
            printf("There were errors on serial_check_error()\n");
        }
        printf("FIFO status: input %d, output %d\n", inqueue, outqueue);
    }


    printf("***Close***\n");
    serial_close( hComm);

    return 0;
}

