#ifndef __M_SERIAL_H_
#define __M_SERIAL_H_

#include<windows.h>

/**
 * @brief serial_exists
 *
 * Check if a serial port exist
 *
 * @param[in] port number of port: 7 for "COM7"
 *
 * @return TRUE if serial port found, FALSE when not found
*/
BOOL serial_exist( int port);

/**
 * @brief serial_open
 * 
 * Open a serial port by name, like "\\.\COM9" or "COM1"
 * 
 * @return a Handle on Serial port opened, on error return NULL
*/
HANDLE serial_open( char *serialPort);

/**
 * @brief serial_config
 * 
 * Configure an opened serial port
 *
 * @param[in] hComm Handle to opened serial port
 * @param[in] speed baudrate, use CBR_9600,CBR_115200 like value
 * @param[in] parity use NOPARITY, EVENPARITY, MARKPARITY ...
 * @return FALSE on error
*/
BOOL serial_config(HANDLE hComm, int speed, int parity);

/**
 * @brief serial_write
 * 
 * @param[in] hComm Handle to opened serial port
 * @param[in] lpBuffer buffer to data to write
 * @param[in] dNoOFBytestoWrite byte count to write on serial port from lpBuffer
 * @param[out] dNoOfBytesWritten byte count really written after the call
 * @return FALSE on error
*/
BOOL serial_write(HANDLE hComm, char *lpBuffer, DWORD dNoOFBytestoWrite, DWORD *dNoOfBytesWritten);

/**
 * @brief serial_read
 * 
 * @param[in] hComm Handle to opened serial port
 * @param[out] ReadBuffer buffer to retrieve data, shall be allocated by caller
 * @param[in] dNoOfBytesToRead byte count to read from serial port to ReadBuffer
 * @param[out] dNoOfBytesReallyRead byte count really read after the call
 * @return FALSE on error
*/
BOOL serial_read(HANDLE hComm, char *ReadBuffer,DWORD dNoOfBytesToRead, DWORD *dNoOfBytesReallyRead);

/**
 * @brief serial_flush
 * 
 * Flush serial port (only RX ?)
 * 
 * @param[in] hComm Handle to opened serial port
*/
void serial_flush(HANDLE hComm); 

/**
 * @brief serial_check_error
 *
 * Get error status and FIFO status
 * This function flush all buffer RX+TX on error (design choice)
 *
 * @param[in] hComm Handle to opened serial port
 * @param[out] inqueue return current input queue size in bytes, can be NULL if not needed
 * @param[out] outqueue return current output queue size in bytes, can be NULL if not needed
 * @param[out] error return current error when case, 0 when ok, can be NULL if not needed
 * @return FALSE on error
 * @return TRUE  when OK
*/
BOOL serial_check_error(HANDLE hComm, DWORD *inqueue, DWORD *outqueue, DWORD *error);


/**
 * @brief serial_close
 * @param[in] hComm Handle to opened serial port
*/
void serial_close(HANDLE hComm);


#endif /* __M_SERIAL_H_ */

