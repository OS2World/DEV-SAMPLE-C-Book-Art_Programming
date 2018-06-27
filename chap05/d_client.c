#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <conio.h>
#include <fcntl.h>
#include "dcommon.h"
int main(unsigned short usNumArgs,char *apchArgs[])

{
   unsigned short   *usHiCh;
   unsigned short   *usLoCh;
   unsigned long    ulHiLoCh;
   unsigned short   arReturn;

   achPipeName[0] = 0;
   chToken = 0;
   ulHiLoCh = 0;
   usLoCh = (unsigned short *)&ulHiLoCh;
   usHiCh = usLoCh+1;
   chToken = CLIENT_MODE;

/*   ANSI.SYS must be loaded for the COLOR string to work      */
/*   printf (CLIENT_COLOR);                                    */

   printf("\n\n Hit F3 to exit client program. \n\n");
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
   if (arReturn != EOF)
   {
      printf(" You can start typing in this CLIENT window\n");
      printf(
       " and watch for your keystrokes in the SERVER window\n\n")
         ;
   } 

   while ((arReturn != EOF) && (chToken != DISCON_MODE))
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
   close(hpPipe);

/*   ANSI.SYS must be loaded for the COLOR string to work      */
/*   printf (NORMAL_COLOR);                                    */

   return  arReturn;
} 

unsigned short ConnToServer(void)
{
   char             achInitBuf[HAND_SHAKE_LEN+1];
   unsigned long    ulOpenFlag;
   unsigned long    ulOpenMode;
   unsigned long    ulActionTaken;
   int              arReturn = 0;
   unsigned long    ulBytesDone;

   memset(achInitBuf, 
          0, 
          sizeof(achInitBuf));

   ulOpenFlag = DEFAULT_OPEN_FLAG;
   ulOpenMode = DEFAULT_OPEN_MODE;

   arReturn = hpPipe = open(achPipeName, 
                            O_RDWR|O_BINARY);

   if (arReturn != EOF)
   {
      arReturn = write(hpPipe, 
                       HAND_SHAKE_INP, 
                       strlen(HAND_SHAKE_INP));

      if (arReturn != EOF)
      {

         arReturn = read(hpPipe, 
                         achInitBuf, 
                         HAND_SHAKE_LEN);

         if (strcmp(achInitBuf, 
                    HAND_SHAKE_OUT))
         {
            arReturn = HAND_SHAKE_ERROR;
         }                             /* endif                */
      }                                /* endif                */
   }                                   /* endif                */
   if (arReturn == EOF)
   {
      printf("\n  The Pipe Open / Connection API "
         "returned rc = %02d\n", 
             arReturn);
      printf("\n  Make sure the Server is running.\n\n");
   }                                   /* endif                */
   return  arReturn;
} 

unsigned short SendToServer(unsigned long ulHiLoCh)
{
   unsigned long    ulBytesDone;

   return (write(hpPipe, 
                 &ulHiLoCh, 
                 sizeof(ulHiLoCh)));
} 

