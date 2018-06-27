#define  INCL_OS2
#define  INCL_KBD
#define  INCL_VIO
#define  INCL_DOSPROCESS
#define  INCL_DOSDEVICES
#define  INCL_DOSDEVIOCTL
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#define  STACK_SIZE    8192
#define  BPS           9600
#define  KBD_HANDLE    0
#define  VIO_HANDLE    0
struct
{
   BYTE             dataBits;

   BYTE             parity;
   BYTE             stopBits;
} lineCtrl =
{
   8,
   0,
   0
} ;
                                       // 8,N,1

DCBINFO          dcbInfo;

HFILE            hCom;                 // COM handle
unsigned char    inBuffer[256];        // input buffer

void ComThread(void);

/***************************************************************/
/*  main                                                       */
/***************************************************************/

int main(void)
{

   APIRET           ulAction,rc = 0;
   ULONG            ulBaudRate = BPS;
   ULONG            ulParmLen = 0;
   ULONG            ulBytesWritten;
   TID              ComThreadId = 0;
   ULONG            ulKbdChar = 0;

   printf("\n\n Each keystroke is echoed to COM1, 9600,8,N,1");
   printf("\n Ctrl-C or Ctrl-Brk to exit...\n\n");

/*Open and initialize COM1                                     */

   if (DosOpen((PUCHAR)"COM1",
               &hCom,
               &ulAction,
               0L,
               0,
               1,
               0x12,
               0L))
   {
      printf("COM1 not available or COM0x.SYS not loaded\n");
      exit(1);
   }

/*Set data rate to 9600bps and line format to N81              */

   ulParmLen = sizeof(ulBaudRate);

   rc = DosDevIOCtl(hCom,
                    IOCTL_ASYNC,
                    ASYNC_SETBAUDRATE,
                    &ulBaudRate,
                    ulParmLen,
                    &ulParmLen,
                    0,
                    0,
                    0);

   ulParmLen = sizeof(lineCtrl);

   rc = DosDevIOCtl(hCom,
                    IOCTL_ASYNC,
                    ASYNC_SETLINECTRL,
                    &lineCtrl,
                    ulParmLen,
                    &ulParmLen,
                    0,
                    0,
                    0);

/*Set device control block parameters                          */

   ulParmLen = sizeof(DCBINFO);

   rc = DosDevIOCtl(hCom,
                    IOCTL_ASYNC,
                    ASYNC_GETDCBINFO,
                    0,
                    0,
                    0,
                    &dcbInfo,
                    ulParmLen,
                    &ulParmLen);

   dcbInfo.usWriteTimeout = 6000;

   /************************************************************/
   /* 60 second write timeout                                  */
   /************************************************************/

   dcbInfo.usReadTimeout = 6000;

   /************************************************************/
   /* 60 second readtimeout                                    */
   /************************************************************/

   dcbInfo.fbCtlHndShake = MODE_DTR_CONTROL;

   /************************************************************/
   /* enable DTR                                               */
   /************************************************************/

   dcbInfo.fbFlowReplace = MODE_RTS_CONTROL;

   /************************************************************/
   /* enable RTS                                               */
   /************************************************************/

   dcbInfo.fbTimeout = MODE_WAIT_READ_TIMEOUT;

   /************************************************************/
   /* wait-for-something reads                                 */
   /************************************************************/

   ulParmLen = sizeof(DCBINFO);

   rc = DosDevIOCtl(hCom,
                    IOCTL_ASYNC,
                    ASYNC_GETDCBINFO,
                    &dcbInfo,
                    ulParmLen,
                    &ulParmLen,
                    0,
                    0,
                    0);

/*Create a thread to monitor the serial port                   */

   rc = DosCreateThread(&ComThreadId,
                        (PFNTHREAD)&ComThread,
                        0,
                        CREATE_READY,
                        STACK_SIZE);

/*Monitor the keyboard and output typed characters
Hit Ctrl-C to exit (primitive termination)                     */

   while (!rc)
   {
      if (kbhit())
      {
         ulKbdChar = (ULONG)getche();
         rc = DosWrite(hCom,
                       &ulKbdChar,
                       1,
                       &ulBytesWritten);
      }
   }

   printf(
    "\n\n Could not write to COM1, killing the MAIN thread.\n\n")
      ;

   return (rc);
}

/***************************************************************/
/*  Thread to read characters from COM1 and write to screen    */
/***************************************************************/

void ComThread(void)
{

   ULONG            ulBytesRead = 0,i;
   APIRET           rc = 0;

   while (!rc)
   {
      rc = DosRead(hCom,
                   inBuffer,
                   1,
                   &ulBytesRead);

      if (ulBytesRead)
      {
         for (i = 0; i < ulBytesRead; i++)
            inBuffer[i] &= 0x7f;
         VioWrtTTY(inBuffer,
                   ulBytesRead,
                   VIO_HANDLE);
      }
   }
   printf("\n\n Could not read from COM1");
   printf("killing the LISTEN thread.\n\n");


}
