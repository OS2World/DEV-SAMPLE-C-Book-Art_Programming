#define  INCL_DOS
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#define  THREAD_SUSPEND 1L
#define  STDOUT        (HFILE) 1
VOID APIENTRY MyThread(ULONG ulThreadArgs);

INT main(VOID)
{
   APIRET           arReturn;
   TID              tidThreadID[15];
   USHORT           usIndex;
   ULONG            ulThreadPriorities[] = 
   {
      PRTYC_FOREGROUNDSERVER,PRTYC_TIMECRITICAL,PRTYC_REGULAR,
      PRTYC_NOCHANGE,PRTYC_IDLETIME
   } 
   ;

   for (usIndex = 0; usIndex < 15; usIndex++)
   {
      arReturn = DosCreateThread(&tidThreadID[usIndex], 
                                 MyThread, 
                                 ulThreadPriorities[usIndex%5], 
                                 THREAD_SUSPEND, 
                                 4096);

      arReturn = DosSetPriority(PRTYS_THREAD, 
                                ulThreadPriorities[usIndex%5], 
                                (LONG)0, 
                                tidThreadID[usIndex]);

      if (arReturn)
      {
         printf("\narReturn = %d", 
                arReturn);
      }                                /* endif                */
      DosResumeThread(tidThreadID[usIndex]);
   }                                   /* endfor               */
   DosSleep(20000);
   return 0;
} 

VOID APIENTRY MyThread(ULONG ulThreadArgs)
{
   USHORT           usIndex;
   CHAR             cChar;
   ULONG            ulBytesWritten;

   for (usIndex = 0; usIndex < 2000; usIndex++)
   {
      cChar = (CHAR)ulThreadArgs+'0';

      DosWrite(STDOUT, 
               (PVOID)&cChar, 
               1, 
               &ulBytesWritten);
   }                                   /* endfor               */
   return ;
} 
