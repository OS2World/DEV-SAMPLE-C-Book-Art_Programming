#define  INCL_DOSFILEMGR
#define  INCL_DOSMISC
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define  CONFIG_FILE   "?:\\CONFIG.SYS"
#define  LIBPATH       "LIBPATH"
#define  CR            13
#define  LF            10

INT main(VOID)
{
   APIRET           arReturn;
   ULONG            ulDrive;
   PCHAR            pchFile;
   HFILE            hfFile;
   ULONG            ulAction;
   ULONG            ulBytesRead;
   PCHAR            pchBuffer;
   PCHAR            pchLibpath;
   USHORT           usIndex;
   FILESTATUS3      fsStatus;

   pchBuffer = NULL;

/* The number of SysInfo Queries */
   arReturn = DosQuerySysInfo(QSV_TIMER_INTERVAL, QSV_TIMER_INTERVAL,
                              &ulDrive, sizeof(ulDrive));
   printf("QSV_TIMER_INTERVAL = %i",ulDrive);

   arReturn = DosQuerySysInfo(QSV_MIN_SLICE, QSV_MIN_SLICE,
                              &ulDrive, sizeof(ulDrive));
   printf("QSV_MIN_SLICE = %i",ulDrive);

   arReturn = DosQuerySysInfo(QSV_MAX_SLICE,QSV_MAX_SLICE,
                              &ulDrive, sizeof(ulDrive));
   printf(" QSV_MAX_SLICE = %i\n",ulDrive);

   arReturn = DosQuerySysInfo(QSV_VERSION_MAJOR, QSV_VERSION_MAJOR,
                              &ulDrive,sizeof(ulDrive));
   printf("QSV_VERSION_MAJOR = %i",ulDrive);

   arReturn = DosQuerySysInfo(QSV_VERSION_MINOR,QSV_VERSION_MINOR,
                              &ulDrive, sizeof(ulDrive));
   printf(" QSV_VERSION_MINOR = %i",ulDrive);

   arReturn = DosQuerySysInfo(QSV_VERSION_REVISION,QSV_VERSION_REVISION,
                              &ulDrive, sizeof(ulDrive));
   printf(" QSV_VERSION_ REVISION = %i\n",ulDrive);

   arReturn = DosQuerySysInfo(QSV_MS_COUNT,QSV_MS_COUNT,
                              &ulDrive, sizeof(ulDrive));
   printf("QSV_MS_COUNT = %i\n",ulDrive);

   arReturn = DosQuerySysInfo(QSV_TOTPHYSMEM,QSV_TOTPHYSMEM,
                              &ulDrive, sizeof(ulDrive));
   printf("QSV_TOTPHYSMEM = %ikb",ulDrive/1024);

   arReturn = DosQuerySysInfo(QSV_TOTRESMEM,QSV_TOTRESMEM,
                              &ulDrive, sizeof(ulDrive));
   printf(" QSV_TOTRESMEM = %ikb",ulDrive/1024);

   arReturn = DosQuerySysInfo(QSV_TOTAVAILMEM,QSV_TOTAVAILMEM,
                              &ulDrive, sizeof(ulDrive));
   printf(" QSV_TOTAVAILMEM = %ikb\n",ulDrive/1024);

   arReturn = DosQuerySysInfo(QSV_MAXPRMEM,QSV_MAXPRMEM,
                              &ulDrive, sizeof(ulDrive));
   printf("QSV_MAXPRMEM = %ikb",ulDrive/1024);

   arReturn = DosQuerySysInfo(QSV_MAXSHMEM,QSV_MAXSHMEM,
                              &ulDrive, sizeof(ulDrive));
   printf(" QSV_MAXSHMEM = %ikb\n",ulDrive/1024);

   arReturn = DosQuerySysInfo(QSV_FOREGROUND_FS_SESSION,QSV_FOREGROUND_FS_SESSION,
                              &ulDrive, sizeof(ulDrive));
   printf("QSV_FOREGROUND_FS_SESSION = %i",ulDrive);

   arReturn = DosQuerySysInfo(QSV_FOREGROUND_PROCESS,QSV_FOREGROUND_PROCESS,
                              &ulDrive, sizeof(ulDrive));
   printf("  QSV_FOREGROUND_PROCESS = %i\n",ulDrive);

   arReturn = DosQuerySysInfo(26,26,
                              &ulDrive, sizeof(ulDrive));
   printf("QSV_NUMPROCESSORS = %i (rc = %i)\n",ulDrive,arReturn);

   arReturn = DosQuerySysInfo(27,27,
                              &ulDrive, sizeof(ulDrive));
   printf("QSV_27 = %i (rc = %i)\n",ulDrive,arReturn);

   /************************************************************/
   /* find the boot drive                                      */
   /************************************************************/

   arReturn = DosQuerySysInfo(QSV_BOOT_DRIVE,
                              QSV_BOOT_DRIVE,
                              &ulDrive,
                              sizeof(ulDrive));

   pchFile = CONFIG_FILE;
   pchFile[0] = ulDrive+'A'-1;

   /************************************************************/
   /* get the size of the CONFIG.SYS                           */
   /************************************************************/


   arReturn = DosQueryPathInfo(pchFile,
                               FIL_STANDARD,
                               &fsStatus,
                               sizeof(fsStatus));

    printf("%s cbFileAlloc = %i\n",pchFile,fsStatus.cbFileAlloc);
   /************************************************************/
   /* allocate buffer size + 1 for NULL                        */
   /************************************************************/

   pchBuffer = malloc(fsStatus.cbFile+1);

   arReturn = DosOpen(pchFile,
                      &hfFile,
                      &ulAction,
                      0,
                      FILE_NORMAL,
                      FILE_OPEN,
             OPEN_FLAGS_FAIL_ON_ERROR |
             OPEN_FLAGS_SEQUENTIAL |
             OPEN_SHARE_DENYNONE |
             OPEN_ACCESS_READONLY ,
                      NULL);

   arReturn = DosRead(hfFile,
                      pchBuffer,
                      fsStatus.cbFile,
                      &ulBytesRead);

   arReturn = DosClose(hfFile);

   pchBuffer[fsStatus.cbFile] = 0;

   /************************************************************/
   /* seach buffer for LIBPATH variable                        */
   /************************************************************/

   pchLibpath = strstr(pchBuffer,
                       LIBPATH);

   if (pchLibpath == NULL)
   {

      /*********************************************************/
      /* will only execute this section of code if LIBPATH is  */
      /* NOT all caps                                          */
      /*********************************************************/

      for (usIndex = 0; usIndex < strlen(pchBuffer); usIndex++)
      {
         if (toupper(pchBuffer[usIndex]) == 'L')
            if (toupper(pchBuffer[usIndex+1]) == 'I' && toupper
               (pchBuffer[usIndex+2]) == 'B' && toupper(pchBuffer
               [usIndex+3]) == 'P' && toupper(pchBuffer[usIndex+4
               ]) == 'A' && toupper(pchBuffer[usIndex+5]) == 'T'
               && toupper(pchBuffer[usIndex+6]) == 'H')
            {
               pchLibpath = pchBuffer+usIndex;
               break;
            }
      }
   }

   /************************************************************/
   /* read to the line feed                                    */
   /************************************************************/

   for (usIndex = 0; usIndex < CCHMAXPATHCOMP; usIndex++)
   {
      if (pchLibpath[usIndex] == CR)
      {
         if (pchLibpath[usIndex+1] == LF)
            break;
      }                                /* endif                */
   }                                   /* endfor               */
   pchLibpath[usIndex] = 0;

   /************************************************************/
   /* print out the LIBPATH                                    */
   /************************************************************/

   printf("\n%s\n",
          pchLibpath);
   free(pchBuffer);
   return  arReturn;
}
