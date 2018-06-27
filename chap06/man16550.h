/* Header file for the 16-bit LIB/DLL used to perform IOPL i/o calls */
/* A. Panov 1993,1994,1995                                           */

unsigned short (*  _Far16 _Pascal my_inp )(unsigned short);
unsigned short (*  _Far16 _Pascal my_outp)(unsigned short, unsigned short);

#define MY_COM1         0x3F8
#define MY_COM2         0x2F8
#define MY_INT_ENABLE   1
#define MY_INT_ID       2
#define MY_FIFO_CTRL    2
#define MY_LINE_CTRL    3
#define MY_MODEM_CTRL   4
#define MY_LINE_STATUS  5
#define MY_MODEM_STATUS 6
#define MY_SCRATCH      7

