#define  INCL_DOSNMPIPES
#define  INCL_DOSPROCESS
#include <os2.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <conio.h>
#include "common.h"

int TestPipeServer(void);

INT main(void)

{
   USHORT           *usHiCh;
   USHORT           *usLoCh;
   ULONG            ulHiLoCh;
   APIRET           arReturn;
   ULONG            ulBytesDone;

   arReturn = DosExitList(EXLST_ADD,
                          CleanUp);

   ulHiLoCh = 0;
   usLoCh = (USHORT *)&ulHiLoCh;
   usHiCh = usLoCh+1;
   chToken = SERVER_MODE;

   printf(SERVER_COLOR);
   printf(" Hit Ctrl+C to exit the server at any time \n");
   printf(" Starting the program in Server Mode...\n\n");

   arReturn = ConnFromClient();
   printf("\n  The Pipe Creation / Connection API "
      " returned rc = %02X\n\n",
          arReturn);

   while (!arReturn && (chToken != DISCON_MODE))
   {

      arReturn = RecvFromClient(&ulHiLoCh,
                                &ulBytesDone);
      if(*usLoCh == 'T')
      {  printf("\npipe spead test\n");
         TestPipeServer();
         continue;
      }

      if (ulHiLoCh == TOKEN_F3_DISCON || !ulBytesDone)
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

int TestPipeServer(void)
{
   APIRET  arReturn;
   int i,j,raz,val,BytesDone,buffer[1024],n;
   arReturn = RecvFromClient(&raz,&BytesDone);
   if(arReturn)
      return arReturn;
printf("\n%ibytes",raz*4);
   for(i=0; i<raz; )
   { /*  arReturn = RecvFromClient(&val,&BytesDone);     */
      arReturn =  DosRead(hpPipe,
                   buffer,
                   1024 * sizeof(int),
                   &BytesDone);

      if(arReturn == 234) arReturn = 0;
      if(arReturn)
      {  printf("\nError code %i after %ibytes",arReturn,i*4);
         break;
      }
/*  printf("\nBytesDone=%i",BytesDone); */
      for(j=0;j<BytesDone/sizeof(int) && i<raz;j++,i++)
      { val = buffer[j];
        if(val != i)
        {  printf("\n\aError fast pipes test : mast be %i, received %i",i,val);
           printf("\nBytesDone=%i,j=%i,@#$=%i",BytesDone,j,BytesDone/sizeof(int));
           exit(1);
        }
        if(!(i%102400))  printf("\n%ik",i/1024);
/*         printf("\n%i %i",i,val); */
      }
   }

   return arReturn;

/*   return  DosRead(hpPipe,
                   pulHiLoCh,

                   sizeof(pulHiLoCh),
                   pulBytesDone);
*/

}


APIRET ConnFromClient(VOID)
{
   CHAR             achInitBuf[HAND_SHAKE_LEN+1];
   ULONG            ulOpenMode;
   ULONG            ulPipeMode;
   ULONG            ulOutBufSize;
   ULONG            ulInpBufSize;
   ULONG            ulTimeOut;
   USHORT           arReturn;
   ULONG            ulBytesDone;

   memset(achInitBuf,
          0,
          sizeof(achInitBuf));

   ulOpenMode = DEFAULT_MAKE_MODE;
   ulPipeMode = DEFAULT_PIPE_MODE;
   ulOutBufSize = DEFAULT_OUTB_SIZE;
   ulInpBufSize = DEFAULT_INPB_SIZE;
   ulTimeOut = DEFAULT_TIME_OUTV;

   arReturn = DosCreateNPipe(DEFAULT_PIPE_NAME,
                             &hpPipe,
                             ulOpenMode,
                             ulPipeMode,
                             ulOutBufSize,
                             ulInpBufSize,
                             ulTimeOut);

   if (!arReturn)
   {
      printf(" You can start the CLIENT program now.\n");
      printf(" Typing in the CLIENT window will make\n");
      printf(" the keystrokes echo in this SERVER window\n\n");
      arReturn = DosConnectNPipe(hpPipe);
      if (!arReturn)
      {
         arReturn = DosRead(hpPipe,
                            achInitBuf,
                            (ULONG)HAND_SHAKE_LEN,
                            &ulBytesDone);

         if (!strcmp(achInitBuf,
                     HAND_SHAKE_INP) && !arReturn)
         {
            arReturn = DosWrite(hpPipe,
                                HAND_SHAKE_OUT,
                                strlen(HAND_SHAKE_OUT),
                                &ulBytesDone);
         }
         else
         {
            arReturn = HAND_SHAKE_ERROR;
         }                             /* endif                */
      }                                /* endif                */
   }                                   /* endif                */
   return  arReturn;
}

APIRET RecvFromClient(PULONG pulHiLoCh,PULONG pulBytesDone)
{

   return  DosRead(hpPipe,
                   pulHiLoCh,

                   sizeof(pulHiLoCh),
                   pulBytesDone);

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
