
#include<stdio.h>
#include<stdlib.h>

#include"serial.h"

BOOL serial_exist( int port) {
    HANDLE hComm;
    char buffer[16];
    BOOL ret;

    if (! (1 <= port && port <= 255)) {
        return FALSE;
    }

    snprintf( buffer, sizeof(buffer), "\\\\.\\COM%d", port);

    hComm = CreateFileA(buffer,                //port name
                        GENERIC_READ | GENERIC_WRITE, //Read/Write
                        0,                            // No Sharing
                        NULL,                         // No Security
                        OPEN_EXISTING,// Open existing port only
                        0,            // Non Overlapped I/O
                        NULL);        // Null for Comm Devices

    if (hComm == INVALID_HANDLE_VALUE) {
        //printf("Error in opening serial port");
        ret = FALSE;
        if( ERROR_SHARING_VIOLATION == GetLastError()) {
            // ne peut pas ouvrir car déjà ouvert, donc le port existe
            printf(" COM%d already in use!\n", port);
            ret = TRUE;
        }
    }else{
        //printf("opening serial port successful");
        ret = TRUE;
    }


    CloseHandle(hComm);//Closing the Serial Port
    return ret;
}

HANDLE serial_open( char *serialPort){
    HANDLE hComm;

    hComm = CreateFileA(serialPort,   //port name
        GENERIC_READ | GENERIC_WRITE, //Read/Write
        0,                            // No Sharing
        NULL,                         // No Security
        OPEN_EXISTING,                // Open existing port only
        FILE_ATTRIBUTE_NORMAL,        // Non Overlapped I/O
        NULL);                        // Null for Comm Devices

    if (hComm == INVALID_HANDLE_VALUE) {
        printf("Error in opening serial port");
        hComm = NULL;
    }

    return hComm;
}


BOOL serial_config(HANDLE hComm, int speed, int parity){
    BOOL Status = TRUE;
    DCB dcbSerialParams = { 0 }; // Initializing DCB structure
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    COMMTIMEOUTS timeouts = { 0 }; // pour read() avoid block ...

    /* set fifo to manage data, en fait j'ai 4096 ... */
    Status = SetupComm(hComm, 1024 /*dwInQueue*/,1024/*dwOutQueue*/);
    if(Status == FALSE)
    {
        printf("Failed SetupComm() for fifo size (not so harmful), err 0x%lx\n",GetLastError());
        Status = FALSE;
    }

    // get config
    Status = GetCommState(hComm, &dcbSerialParams);
    if(Status == FALSE)
    {
        printf("Failed GetCommState(), err 0x%lx\n",GetLastError());
    }

    // set/update config
    dcbSerialParams.BaudRate = speed;  // Setting  CBR_115200 BaudRate = 115200
    dcbSerialParams.ByteSize = 8;         // Setting ByteSize = 8, can be between 5 and 8
    dcbSerialParams.StopBits = ONESTOPBIT;// Setting StopBits = 1; TWOSTOPBITS ...
    dcbSerialParams.Parity   = parity;  // Setting Parity = None, EVENPARITY ...

    dcbSerialParams.fBinary = TRUE; // shall always be 1
    if( dcbSerialParams.Parity != NOPARITY) dcbSerialParams.fParity = 1;

    // il y  a aussi des champs  pour le controle de flux matériel
    // https://docs.microsoft.com/en-us/windows/win32/api/winbase/ns-winbase-dcb

    // apply
    SetCommState(hComm, &dcbSerialParams);

    //Setting Timeouts (que pour read() en fait, en write() pourrait lever des CommEvents)
    timeouts.ReadIntervalTimeout = 200;       // attente max entre 2 caracteres en [ms], si plus que cette valeur la fonction read() retourne
    timeouts.ReadTotalTimeoutMultiplier = 50; // ce nb est multiplié par le nb de car. a lire, c'est le timeout de lecture; si veut 30 char : 30*50 -> 1.5s d'attente
    timeouts.ReadTotalTimeoutConstant = 100;  // constante d'attente de lecture d'une trame ajouté au result de ReadTotalTimeoutMultiplier
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    if ( FALSE == SetCommTimeouts(hComm, &timeouts) ) {
        printf("\nError to Setting Time outs err 0x%lx\n",GetLastError());
        Status = FALSE;
    }

    serial_flush(hComm);

    return Status;
}


BOOL serial_write(HANDLE hComm, char *lpBuffer, DWORD dNoOFBytestoWrite, DWORD *dNoOfBytesWritten){
    BOOL Status;
    Status = WriteFile(hComm,             // Handle to the Serial port
                       lpBuffer,          // Data to be written to the port
                       dNoOFBytestoWrite, //No of bytes to write
                       dNoOfBytesWritten, //Bytes written
                       NULL);

    if(Status == FALSE) {
        printf("Failed WriteFile(), err 0x%lx\n",GetLastError());
    }

    return Status;
}


BOOL serial_read(HANDLE hComm, char *ReadBuffer,DWORD dNoOfBytesToRead, DWORD *dNoOfBytesReallyRead){
    BOOL Status;
    Status = ReadFile(hComm,                 // Handle to the Serial port
                       ReadBuffer,           // Data to be read from the port
                       dNoOfBytesToRead,     // count : what is in queue !
                       dNoOfBytesReallyRead, //Bytes really read
                       NULL);

    if(Status == FALSE) {
        printf("Failed ReadFile(), err 0x%lx\n",GetLastError());
    }

    return Status;
}


void serial_flush(HANDLE hComm){
    // force data to be written
    FlushFileBuffers( hComm );
    //erase RX and TX data from FIFOs
    PurgeComm(hComm,   PURGE_RXABORT | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_TXCLEAR);
}


BOOL serial_check_error(HANDLE hComm, DWORD *inqueue, DWORD *outqueue, DWORD *error){
    DWORD lpErrors = 0;
    COMSTAT lpStat;
    BOOL Status;

    Status = ClearCommError(hComm,  &lpErrors, &lpStat);
    if(Status == FALSE) {
        /* ne doit pas arriver ici ! */
        /* error code : https://docs.microsoft.com/fr-fr/windows/win32/debug/system-error-codes */
        printf("Failed ClearCommError(%lx), err 0x%lx\n",lpErrors, GetLastError());
    } else {
        // return error value
        if( NULL != error)  *error = 0;

            // si tout a 0 c'est que c'est bon, 
        if( lpErrors & CE_BREAK    ) { printf("\tError : CE_BREAK\n"); }
        if( lpErrors & CE_FRAME    ) { printf("\tError : CE_FRAME\n");  } /* The hardware detected a framing error. : espace between data ? */
        if( lpErrors & CE_OVERRUN  ) { printf("\tError : CE_OVERRUN\n"); }
        if( lpErrors & CE_RXOVER   ) { printf("\tError : CE_RXOVER\n"); } // c'est que la fifo d'entrée de 4096 est pleine...
        if( lpErrors & CE_RXPARITY ) { printf("\tError : CE_RXPARITY\n"); }

        /* on whatever error : flush everything  */
        if( lpErrors != 0 )
            serial_flush(hComm);
        
        // Output queues status
        if( NULL != inqueue)  *inqueue  = lpStat.cbInQue;
        if( NULL != outqueue) *outqueue = lpStat.cbOutQue;
    }

    return Status;
}


void serial_close(HANDLE hComm){
    //Closing the Serial Port
    if(NULL != hComm) {
        CloseHandle(hComm);
    }
}


#if 0  /* semble pas bon : https://docs.microsoft.com/en-us/windows/win32/devio/monitoring-communications-events effectivement le dernier param a init !!!!*/
            /* wait for end tx ? will not return in case of error ? */
            DWORD lpEvtMask = EV_TXEMPTY;
            Status= WaitCommEvent(hComm,  &lpEvtMask, NULL);
            if(Status == FALSE)
            {
                    // err 0x57: invalid parameter
                printf("Failed WaitCommEvent(%lx), err 0x%lx\n",lpEvtMask, GetLastError()); // error code : https://docs.microsoft.com/fr-fr/windows/win32/debug/system-error-codes
            }
#endif

