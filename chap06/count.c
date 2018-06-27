#define  INCL_DOSFILEMGR
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
ULONG _Far16 _Pascal HowMany(USHORT *_Seg16 usCheckSum,char 
                              *_Seg16);

ULONG _Far16 _Pascal HowMany(USHORT *_Seg16 usCheckSum,char 
                              *_Seg16 szFileName)
{

#define  MY_MAX_COUNT  10000
   APIRET           rc = 0;
   FILESTATUS3      fsts3Info;
   ULONG            ulCount = 0;
   UCHAR            *pBigBuffer = 0;
   HFILE            hFileHandle;
   ULONG            ulAction = 0L;
   ULONG            ulBytes = 0L;

   printf("\n Now inside 32-bit %s", 
          __FILE__);

   printf("\n size of usCheckSum (USHORT * _Seg16) = %d", 
          sizeof(USHORT *_Seg16));
   printf("\n size of szFileName (char * _Seg16  ) = %d", 
          sizeof(char *_Seg16));

   rc = DosQueryPathInfo(szFileName, 
                         FIL_STANDARD, 
                         &fsts3Info, 
                         sizeof(FILESTATUS3));
   if (rc)
      return (0);
   else
   {
      pBigBuffer = malloc(fsts3Info.cbFileAlloc);
      if (pBigBuffer)
      {
         rc = DosOpen((PSZ)szFileName, 
                      &hFileHandle, 
                      &ulAction, 
                      0L, 
                      FILE_READONLY, 
                      OPEN_ACTION_OPEN_IF_EXISTS, 
                      OPEN_FLAGS_SEQUENTIAL|
                         OPEN_SHARE_DENYREADWRITE|
                         OPEN_ACCESS_READONLY, 
                      0L);
         if (rc)
         {
            printf("\n\n Could not open <BIGFILE>, rc=%d\n\n", 
                   rc);
            return (0);
         } 
         else
         {
            rc = DosRead(hFileHandle, 
                         pBigBuffer, 
                         fsts3Info.cbFile, 
                         &ulBytes);
            if (rc || fsts3Info.cbFile != ulBytes)
            {
               printf("\n\n Could not read the data             \
     rc=%d, FileSize=%d, BytesRead=%d\n", 
                      rc, 
                      fsts3Info.cbFile, 
                      ulBytes);
               return (0);
            } 
            else
            {
               *usCheckSum = 0;
               do
               {
                  *usCheckSum += (USHORT)(*pBigBuffer++);
                  ulCount++;
                  fsts3Info.cbFile--;
               }  while (fsts3Info.cbFile);

            } 
         } 

      } 
      else
      {
         printf(
      "\n\n Could not allocate enough space to read <BIGFILE>\n")
            ;
         return (0);
      } 
      free(pBigBuffer);
      rc = DosClose(hFileHandle);
      printf("\n About to leave the 32-bit %s.\n", 
             __FILE__);
      return (ulCount);
   } 

} 
