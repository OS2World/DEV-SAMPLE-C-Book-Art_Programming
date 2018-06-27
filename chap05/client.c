#define  INCL_DOSNMPIPES
#define  INCL_DOSPROCESS
#include <os2.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <conio.h>
#include "common.h"

int TestPipeClient(int raz);

INT main(USHORT usNumArgs,PCHAR apchArgs[])

{
   USHORT           *usHiCh;
   USHORT           *usLoCh;
   ULONG            ulHiLoCh;
   APIRET           arReturn;

   arReturn = DosExitList(EXLST_ADD,
                          CleanUp);

   achPipeName[0] = 0;
   chToken = 0;
   ulHiLoCh = 0;
   usLoCh = (USHORT *)&ulHiLoCh;
   usHiCh = usLoCh+1;
   chToken = CLIENT_MODE;

   printf(CLIENT_COLOR);
   printf(" Hit F3 to exit client program, \"T\" to start fast test. \n\n");
   printf(" Starting the program in Client Mode...\n\n");

   if (usNumArgs == REMOTE_PIPE)
   {
      sprintf(achPipeName,
              "\\\\%s",
              apchArgs[1]);
   }

   strcat(achPipeName,
          DEFAULT_PIPE_NAME);
   printf(" Connecting to pipe : %s\n\n",
          achPipeName);

   arReturn = ConnToServer();
   if (!arReturn)
   {
      printf(" You can start typing in this CLIENT window\n");
      printf(
       " and watch for your keystrokes in the SERVER window\n\n")
         ;
   }
   while (!arReturn && (chToken != DISCON_MODE))
   {

      *usHiCh = getch();
      if(*usHiCh == 'T')
      {  *usLoCh = *usHiCh;
         printf("\npipe spead test\n");
         arReturn = SendToServer(ulHiLoCh);
         TestPipeClient(1000000);
         continue;
      }
      if ((*usHiCh == FUNC_KEYS_CHAR) || (*usHiCh ==
         EXTD_KEYS_CHAR))
      {
         *usLoCh = getch();
      }
      else
      {
         *usLoCh = *usHiCh;
      }                                /* endif                */
      arReturn = SendToServer(ulHiLoCh);

      if (ulHiLoCh == TOKEN_F3_DISCON)
      {
         chToken = DISCON_MODE;
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
   }                                   /* endwhile             */
   arReturn = DosClose(hpPipe);
   printf(NORMAL_COLOR);
   return  arReturn;
}

int TestPipeClient(int raz)
{
   APIRET  arReturn;
   int i,nb,buffer[1024];
   ULONG ulBytesDone;

   SendToServer(raz);
   for(i=0,nb = 0;i<raz;i++)
   {  buffer[nb++] = i;
      if(nb == 1024)
      {
         arReturn = DosWrite(hpPipe,
                    &buffer,
                    nb * sizeof(int),
                    &ulBytesDone);
         if(arReturn) break;
         nb = 0;
         if(!((i+1)%102400)) printf("\n%ik",(i+1)/1024);
      }
   }
   if(nb)
   {  arReturn = DosWrite(hpPipe,
                    &buffer,
                    sizeof(buffer),
                    &ulBytesDone);
         nb = 0;
         printf("\n%i",i);
   }


   return arReturn;
}

APIRET ConnToServer(VOID)
{
   CHAR             achInitBuf[HAND_SHAKE_LEN+1];
   ULONG            ulOpenFlag;
   ULONG            ulOpenMode;
   ULONG            ulActionTaken;
   INT              arReturn;
   ULONG            ulBytesDone;

   memset(achInitBuf,
          0,
          sizeof(achInitBuf));

   ulOpenFlag = DEFAULT_OPEN_FLAG;
   ulOpenMode = DEFAULT_OPEN_MODE;

   arReturn = DosOpen(achPipeName,
                      &hpPipe,
                      &ulActionTaken,
                      0,
                      0,
                      ulOpenFlag,
                      ulOpenMode,
                      0);

   if (!arReturn)
   {
      arReturn = DosWrite(hpPipe,
                          HAND_SHAKE_INP,
                          strlen(HAND_SHAKE_INP),
                          &ulBytesDone);

      if (!arReturn)
      {
         arReturn = DosRead(hpPipe,
                            achInitBuf,
                            (ULONG)HAND_SHAKE_LEN,
                            &ulBytesDone);

         if (strcmp(achInitBuf,
                    HAND_SHAKE_OUT))
         {
            arReturn = HAND_SHAKE_ERROR;
         }                             /* endif                */
      }                                /* endif                */
   }                                   /* endif                */
   if (arReturn)
   {
      printf("\n  The Pipe Open / Connection API "
         "returned rc = %02x\n",
             arReturn);
      printf("\n  Make sure the Server is running.\n\n");
   }                                   /* endif                */
   return  arReturn;
}

APIRET SendToServer(ULONG ulHiLoCh)
{
   ULONG            ulBytesDone;

   return  DosWrite(hpPipe,
                    &ulHiLoCh,

                    sizeof(ulHiLoCh),
                    &ulBytesDone);
}

VOID APIENTRY CleanUp(ULONG ulTermCode)
{
#define  MY_STDOUT     1
   ULONG            ulBytesDone;

   DosClose(hpPipe);
   DosWrite(MY_STDOUT,
            NORMAL_COLOR,
            strlen(NORMAL_COLOR),
            &ulBytesDone);

   DosExitList(EXLST_EXIT,
               0);

}
