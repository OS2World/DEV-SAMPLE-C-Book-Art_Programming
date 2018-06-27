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
APIRET InitClientQueEnv(VOID);

APIRET WriteToQue(ULONG ulHiLoCh);

INT main(USHORT usNumArgs,PCHAR apchArgs[])
{
   USHORT           *usHiCh;
   USHORT           *usLoCh;
   ULONG            ulHiLoCh;
   APIRET           arReturn;

   ulHiLoCh = 0;
   usLoCh = (USHORT *)&ulHiLoCh;
   usHiCh = usLoCh+1;
   chToken = CLIENT_MODE;

   strcpy(aachColors[0], 
          CLIENT0_COLOR);
   strcpy(aachColors[1], 
          CLIENT1_COLOR);
   strcpy(aachColors[2], 
          CLIENT2_COLOR);
   strcpy(aachColors[3], 
          CLIENT3_COLOR);
   strcpy(aachColors[4], 
          CLIENT4_COLOR);

   printf(WHITE_COLOR);
   printf(" Client process is initializing");
   printf(" and connecting to the Queue...\n");
   arReturn = InitClientQueEnv();

   if (!arReturn)
      printf("\n Press F3 to exit\n\n");

   while (!arReturn && (chToken != DISCON_MODE))
   {
      *usHiCh = getch();
      if ((*usHiCh == FUNC_KEYS_CHAR) || (*usHiCh == 
         EXTD_KEYS_CHAR))
      {
         *usLoCh = getch();
      } 
      else
      {
         *usLoCh = *usHiCh;
      }                                /* endif                */
      arReturn = WriteToQue(ulHiLoCh);

      if (ulHiLoCh == TOKEN_F3_DISCON)
      {
         chToken = DISCON_MODE;
         pmqsClient[usClientIndex].szColor[0] = '\0';
         pmqsClient[usClientIndex].ulPid = 0;
         break;
      }                                /* endif                */
      if (*usLoCh == *usHiCh)
         putch(*usHiCh);
      else
         putch('*');

      if (*usLoCh == RETURN_CHAR)
      {
         putch(LINE_FEED_CHAR);
      }                                /* endif                */
   } 

   if (arReturn == 0)
   {
      arReturn = DosCloseQueue(hqQueue);
   }                                   /* endif                */
   printf("\n[0;37;40m");
   return  arReturn;
} 

APIRET InitClientQueEnv(VOID)
{
   APIRET           arReturn;
   SHORT            sIndex;
   PID              pidOwner;

   arReturn = DosGetNamedSharedMem((PVOID)&pmqsClient, 
                                   DEFAULT_SEG_NAME, 
                                   PAG_WRITE|PAG_READ);

   if (!arReturn)
   {
      for (sIndex = 0; sIndex <= MAX_CLIENTS; sIndex++)
      {
         if ((pmqsClient[sIndex].szColor[0] == 0) && (sIndex < 
            MAX_CLIENTS))
         {
            strcpy(pmqsClient[sIndex].szColor, 
                   aachColors[sIndex]);
            usClientIndex = sIndex;
            break;
         }                             /* endif                */
      }                                /* endfor               */
      if (sIndex > MAX_CLIENTS)
      {
         arReturn = PROGRAM_ERROR;
         printf("\n\n  Maximum number of clients is FIVE !\n");
      }                                /* endif                */
      if (!arReturn)
      {
         arReturn = DosOpenQueue(&pidOwner, 
                                 &hqQueue, 
                                 DEFAULT_QUE_NAME);
         if (!arReturn)
         {
            printf(" %s", 
                   aachColors[usClientIndex]);
            printf("\n Client #%d has connected to the Queue\n", 
                   usClientIndex);
         }                             /* endif                */
      }                                /* endif                */
   }                                   /* endif                */
   return  arReturn;
} 

APIRET WriteToQue(ULONG ulHiLoCh)
{
   return  DosWriteQueue(hqQueue, 

                         (ULONG)usClientIndex, 
                         ulHiLoCh, 
                         pvData, 
                         ULONG_NULL);
} 
