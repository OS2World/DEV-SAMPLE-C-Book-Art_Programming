#define  INCL_DOSQUEUES
#define  INCL_DOSMEMMGR
#define  INCL_DOSPROCESS
#define  INCL_DOSERRORS
#define  INCL_DOSSEMAPHORES
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include "qcommon.h"
APIRET InitServerQueEnv(VOID);

APIRET ReadFromQue(PULONG pulHiLoCh);
APIRET ToScreen(VOID *buffer,ULONG length);

INT main(USHORT usNumArgs,PCHAR apchArgs[])
{
   USHORT           *usHiCh;
   USHORT           *usLoCh;
   ULONG            ulHiLoCh;
   APIRET           arReturn = 0;

   arReturn = DosExitList(EXLST_ADD, 
                          CleanUp);

   ulHiLoCh = 0;
   usLoCh = (USHORT *)&ulHiLoCh;
   usHiCh = usLoCh+1;

   printf(SERVER_COLOR);
   printf(" Server process is creating");
   printf(" and initializing the Queue...\n");
   arReturn = InitServerQueEnv();
   if (!arReturn)
      printf("\n Press Ctrl-C, or Ctrl-Break to exit\n\n");

   while (!arReturn)
   {
      arReturn = ReadFromQue(&ulHiLoCh);

      if (!arReturn)
      {

         if (*usLoCh == *usHiCh)
            putch(*usHiCh);
         else
            if (ulHiLoCh == TOKEN_F3_DISCON)
            {
               ToScreen(WHITE_COLOR, 
                        strlen(WHITE_COLOR));
               ToScreen("\n\r client exited\n\n\r", 
                        strlen("\n\r client exited\n\n\r"));
            } 
            else
               putch('*');
         if (*usLoCh == RETURN_CHAR)
            putch(LINE_FEED_CHAR);
      } 
      else
         if (arReturn == ERROR_QUE_EMPTY)

            arReturn = (SHORT)NULL;
   } 

   arReturn = DosCloseQueue(hqQueue);
   printf(WHITE_COLOR);
   return  arReturn;
} 

APIRET InitServerQueEnv(VOID)
{
   APIRET           arReturn;
   SHORT            sIndex;

   arReturn = DosAllocSharedMem((PVOID)&pmqsClient, 
                                DEFAULT_SEG_NAME, 
                                DEFAULT_PAGE_SIZE, 
                                DEFAULT_SEG_FLAG);

   if (!arReturn)
   {
      arReturn = DosCreateQueue(&hqQueue, 
                                DEFAULT_QUE_FLAG, 
                                DEFAULT_QUE_NAME);

      if (!arReturn)
      {
         printf("\n Queue created successfully \n");
         for (sIndex = 0; sIndex < MAX_CLIENTS; sIndex++)
         {
            pmqsClient[sIndex].szColor[0] = (BYTE)NULL;
            pmqsClient[sIndex].ulPid = (PID)NULL;
         }                             /* endfor               */
         arReturn = DosCreateEventSem(DEFAULT_SEM_NAME, 
                                      &hsmSem, 
                                      ULONG_NULL, 
                                      TRUE);
         if (arReturn)
            printf("\n DosCreateEventSem returned ""%02d\n", 
                   arReturn);
      } 
      else
      {
         printf("\n  DosCreateQueue API returned ""%02d\n", 
                arReturn);
      }                                /* endif                */
   } 
   else
   {
      printf(" \n  Could not allocate "
         "Shared Memory ( %02d ) \n", 
             arReturn);
   }                                   /* endif                */
   return  arReturn;
} 

APIRET ReadFromQue(PULONG pulHiLoCh)
{
   APIRET           arReturn;
   REQUESTDATA      rdRequest;
   ULONG            ulSzData;
   BYTE             bPriority;

   arReturn = DosReadQueue(hqQueue, 
                           &rdRequest, 
                           &ulSzData, 
                           &pvData, 
                           0, 
                           DCWW_NOWAIT, 
                           &bPriority, 
                           hsmSem);

   if (!arReturn)
   {
      pmqsClient[rdRequest.ulData].ulPid = rdRequest.pid;
      *pulHiLoCh = ulSzData;

      ToScreen(pmqsClient[rdRequest.ulData].szColor, 
               strlen(pmqsClient[rdRequest.ulData].szColor));

   }                                   /* endif                */
   return  arReturn;
} 

APIRET ToScreen(VOID *buffer,ULONG length)
{
#define  MY_STDOUT     1
   ULONG            ulBytesDone;

   return (DosWrite(MY_STDOUT, 
                    buffer, 
                    length, 
                    &ulBytesDone));

} 

VOID APIENTRY CleanUp(ULONG ulTermCode)
{

   DosCloseQueue(hqQueue);

   ToScreen(WHITE_COLOR, 
            strlen(WHITE_COLOR));

   DosExitList(EXLST_EXIT, 
               0);
} 
