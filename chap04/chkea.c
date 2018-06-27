#define  INCL_DOSERRORS
#define  INCL_DOSFILEMGR
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define  MAXEATYPE     3
CHAR *pszKeyEAName[] =
{

   ".LONGNAME",
   ".ICONPOS",
   ".TYPE"
} ;

typedef struct   _EAINFO
{
   USHORT           usEAType;

   USHORT           usEALength;
   BYTE             bEAData[1];
} EAINFO,*PEAINFO;

INT DumpEA(CHAR *pszFile,PFEA2 pszAttributeName);

INT main(USHORT usNumArgs,PCHAR apchArgs[])
{

   CHAR             achPath[CCHMAXPATHCOMP];
   PCHAR            pchPath;
   ULONG            ulCount;
   HDIR             hdFile;
   APIRET           arReturn;
   FILEFINDBUF4     ffbFile;
   CHAR             achFile[CCHMAXPATHCOMP];
   PBYTE            pbBuffer;
   PFEA2            pdAttribute;
   int i,NumEA;

   if (usNumArgs != 2)
   {

      puts("Syntax:  CHKEA [filename]");
      puts("");
      puts("where \'filename\' is the name of a ");
      puts("file/directory and can contain wildcards.");
      return 1;

   }                                   /* endif                */

   /************************************************************/
   /* get full path name                                       */
   /************************************************************/

   DosQueryPathInfo(apchArgs[1],
                    FIL_QUERYFULLNAME,
                    achPath,
                    CCHMAXPATHCOMP);

   pchPath = strrchr(achPath,
                     '\\');

   if (pchPath != NULL)
   {

      pchPath++;
      *pchPath = 0;

   }                                   /* endif                */
   ulCount = 1;
   hdFile = HDIR_SYSTEM;

   arReturn = DosFindFirst(apchArgs[1],
                           &hdFile,
                           FILE_DIRECTORY,
                           &ffbFile,
                           sizeof(FILEFINDBUF4),
                           &ulCount,
                           FIL_QUERYEASIZE);

   while (arReturn == 0)
   {

      /*********************************************************/
      /* print out full path name                              */
      /*********************************************************/

      sprintf(achFile,
              "%s%s",
              achPath,
              ffbFile.achName);

      printf("\n-----------------------------");
      printf("\nFile name: %s\n",
             achFile);
      printf("\nTotal bytes allocated for EAs: %ld bytes.",
             ffbFile.cbList);

      /*********************************************************/
      /* allocate memory for ea buffer                         */
      /*********************************************************/

      pbBuffer = malloc(ffbFile.cbList +256 );
/*      i = _msize(pbBuffer);               */
      for(i=0;i<256;i++) pbBuffer[i] = i;

      ulCount = -1;
      i = 1;
      arReturn = DosEnumAttribute(ENUMEA_REFTYPE_PATH,
                                  achFile,
                                  i,
                                  pbBuffer,
                                  ffbFile.cbList+128,
                                  &ulCount,
                                  ENUMEA_LEVEL_NO_VALUE);
      NumEA = ulCount;
      printf("\nThis object contains %ld EAs., return code %i",
             NumEA,arReturn);

      pdAttribute = (PFEA2)pbBuffer;

  if(!arReturn)
      while (ulCount != 0)
      {

         printf("\nFound EA with name (Name lenth=%i)\"%s\"",
                (int)(pdAttribute->cbName),pdAttribute->szName);

         DumpEA(achFile,
                pdAttribute);

         ulCount--;
         pdAttribute = (PFEA2)(((PBYTE)pdAttribute)+
            pdAttribute->oNextEntryOffset);

      }                                /* endwhile             */
      free(pbBuffer);
      if(NumEA)
      {  printf("\npress any key");
         fflush(stdout);
         getchar();
      }
      ulCount = 1;
      arReturn = DosFindNext(hdFile,
                             &ffbFile,
                             sizeof(ffbFile),
                             &ulCount);

   }                                   /* endwhile             */

   if ((arReturn != 0) && (arReturn != ERROR_NO_MORE_FILES))
   {

      printf("\nError %ld encountered\n",
             arReturn);

   }                                   /* endif                */
   return  arReturn;
}

int DumpEA(CHAR *pszFile,PFEA2 pdAttribute)
{
   APIRET           rc;
   USHORT           i;
   ULONG            ulGBufLen,ulFBufLen,ulEBufLen;
   PFEA2            pFEA2;
   EAOP2            eaopGet;
   PGEA2LIST        pGEA2List;
   ULONG            ulSize;
   ULONG            ulDataStart;
   PEAINFO          ptrEAData,ptrEADataHolder;

//   for (i = 0; i < MAXEATYPE; i++)
   {

      /*********************************************************/
      /* does EA name match one of the EA's we're interested in*/
      /*********************************************************/

//      if (!strcmp(pdAttribute->szName,
//                  pszKeyEAName[i]))
      {

         /******************************************************/
         /* build input/output data buffer first build         */
         /* fpFEA2List structure                               */
         /******************************************************/

         ulFBufLen = sizeof(FEA2LIST)+pdAttribute->cbName+1+/*
                                          actual name          */
            pdAttribute->cbValue;      /* actual value         */
         pFEA2 = (PFEA2)calloc(1,
                               ulFBufLen);
         if (!pFEA2)
            return  FALSE;

         /******************************************************/
         /* only one pFEA2 attribute in this list              */
         /******************************************************/

         eaopGet.fpFEA2List = (FEA2LIST *)pFEA2;
         eaopGet.fpFEA2List->cbList = ulFBufLen;

         /******************************************************/
         /* next build fpGEA2List structure                    */
         /******************************************************/

         ulGBufLen = sizeof(GEA2LIST)+pdAttribute->cbName+1;

         pGEA2List = (GEA2LIST *)calloc(1,
                                        ulGBufLen);
         if (!pGEA2List)
         {
            free(pFEA2);
            return  FALSE;
         }

         /******************************************************/
         /* initialize fpGEA2List                              */
         /******************************************************/

         pGEA2List->cbList = ulGBufLen;
         pGEA2List->list[0].oNextEntryOffset = 0;
         pGEA2List->list[0].cbName = pdAttribute->cbName;
         strcpy(pGEA2List->list[0].szName,
                pdAttribute->szName);
         eaopGet.fpGEA2List = (GEA2LIST *)pGEA2List;

         /******************************************************/
         /* get EA's                                           */
         /******************************************************/

         ulEBufLen = ulFBufLen+ulGBufLen;
         rc = DosQueryPathInfo(pszFile,
                               FIL_QUERYEASFROMLIST,
                               (PVOID)&eaopGet,
                               ulEBufLen);

         if (!rc)
         {
            printf("\nEA Data for EA %s  ",
                   pdAttribute->szName);

            ulSize = sizeof(FEA2LIST);

            /***************************************************/
            /* get the first list                              */
            /***************************************************/

/* A-            pFEA2 = (PFEA2)eaopGet.fpFEA2List->list; */

            /***************************************************/
            /* offset to start of EAdata                       */
            /***************************************************/

/* A-            ulDataStart = ulSize+pFEA2->cbName;         */
            ulDataStart = ulSize+  ((PFEA2)eaopGet.fpFEA2List->list)->cbName;
            ptrEAData = (PEAINFO)((PBYTE)eaopGet.fpFEA2List+
               ulDataStart);

            /***************************************************/
            /* allocate memory with space for null             */
            /***************************************************/

            ptrEADataHolder = calloc(1,
                                     sizeof(EAINFO)+
                                        ptrEAData->usEALength+1);
            printf("Type = 0x%x  ",
                   ptrEAData->usEAType);
            printf("Length = 0x%x",
                   ptrEAData->usEALength);

            if(ptrEAData->usEALength)
            {  int ii,isstr;
               unsigned char *s;
            /* move Data into placeholder memory */
            memcpy(ptrEADataHolder, ptrEAData->bEAData, ptrEAData->usEALength);
            /* check for ASCII text data */
            s = (char *) ptrEADataHolder;
            isstr = 1;
            for(ii=0;s[ii];ii++)
            {  if(s[ii] >= 32 && s[ii] <= 127) continue;
               if(s[ii] == '\t' || s[ii] == '\n') continue;
               isstr = 0;
               break;
            }
            printf("\nData = ");
            if(isstr)
               printf("%s", ptrEADataHolder);
            else
               {  for(ii=0;ii<ptrEAData->usEALength;ii++)
                  {  printf("%3x",s[ii]);
                  }
               }
            }
            free(ptrEADataHolder);
         }

         free(pFEA2);
         free(pGEA2List);

         if (rc)
         {
            printf("\nDosQueryPathInfo failed, rc = %d",
                   rc);
            return  FALSE;
         }
      }                                /* if EA is one of the
                                          key types            */
   }                                   /* loop through all EA
                                          key types            */
   return  TRUE;
}
