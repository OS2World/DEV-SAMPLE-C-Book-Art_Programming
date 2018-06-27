#define  INCL_DOSPROCESS
#define  INCL_DOSSEMAPHORES
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#define  DEFAULT_THREAD_FLAGS 0
#define  DEFAULT_THREAD_STACK 0x4000
#define  MY_BEGIN_SEMAPHORE "\\SEM32\\BEGIN"
#define  MAX_SEM_WAIT  - 1L
#define  DEFAULT_NUM_THREADS 10
#define  MAX_NUM_THREADS 255

HEV hevKillThread;


VOID APIENTRY MyThreadOne(void);

INT main(USHORT usNumArgs,PCHAR apchArgs[])
{
   USHORT           usNumThreads;
   TID              tidThread;
   INT              iCharRead;
   USHORT           usReturn;

   printf("\n STHREAD.EXE demonstrates the superior pocessor");
   printf("\n time management by allowing a user to start a ");
   printf("\n number of threads (1-255) and have all of them");
   printf("\n wait on one semaphore, while allowing keystrokes");
   printf("\n to be entered at the keyboard.");
   printf("\n (see FTHREAD.EXE, for speed comparison)");
   printf("\n\n      STHREAD.EXE [X], where X is 1-255\n\n");
   printf("\n\n lower case 'x' exits ...\n\n");

   if (usNumArgs > 1)
   {

     //----------------------------------------------------------
      // Insure that usNumThreads is in the range 1<<x<<MAX
     //----------------------------------------------------------
      usNumThreads = max(min(atoi(apchArgs[1]),
                             MAX_NUM_THREADS),
                         1);

   }
   else
   {
      usNumThreads = DEFAULT_NUM_THREADS;
   }                                   /* endif                */
   usReturn = DosCreateEventSem(MY_BEGIN_SEMAPHORE,
                                &hevKillThread,
                                NULLHANDLE,
                                FALSE);

   while (usNumThreads-- && !usReturn)
   {
      usReturn = DosCreateThread(&tidThread,
                                 (PFNTHREAD)MyThreadOne,
                                 (ULONG)NULL,
                                 DEFAULT_THREAD_FLAGS,
                                 DEFAULT_THREAD_STACK);
      if (!usReturn)
         printf(" Started Thread #%2d\n",
                tidThread-1);
      else
         printf(" DosCreateThread returned %2d\n\n",
                usReturn);
   }                                   /* endfor               */
   if (!usReturn)
   {
      printf("\n Start typing and experience ");
      printf("the speed of semaphores for yourself...");
      printf("\n >> lower case 'x' exits << \n\n");

      fflush(stdout);

      iCharRead = getche();

      while (iCharRead != 'x')
      {
         iCharRead = getche();
      }                                /* endwhile             */
   }
   printf("\n\n Exiting, please wait...\n\n");

   usReturn = DosPostEventSem(hevKillThread);
   DosSleep(2000L);
   usReturn = DosCloseEventSem(hevKillThread);

   return  usReturn;
}

VOID APIENTRY MyThreadOne()
{
   DosWaitEventSem(hevKillThread,
                   MAX_SEM_WAIT);
}
