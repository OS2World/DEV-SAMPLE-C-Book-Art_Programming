#define  INCL_WINFRAMEMGR
#define  INCL_WINMENUS
#define  INCL_WINPOINTERS
#define  INCL_WINSTDCNR
#define  INCL_WINSYS
#define  INCL_WINWINDOWMGR
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "CONTAIN3.H"
#define  CLS_CLIENT    "SampleClass"
#define  MAX_YEARS     10
#define  MAX_MONTHS    12
#define  MAX_COLUMNS   4
#define  CX_SPLITBAR   120
//----------------------------------------------------
// For the GA 2.0 toolkit, CRA_SOURCE is not defined,
// but it should be.
//----------------------------------------------------

#ifndef  CRA_SOURCE
#define  CRA_SOURCE    0x00004000L
#endif
typedef struct _CLIENTDATA
{
   HWND             hwndCnr;

   HPOINTER         hptrIcon;
   HWND             hwndMenu;
   BOOL             bCnrSelected;
} CLIENTDATA,*PCLIENTDATA;

typedef struct   _SALESINFO
{
   MINIRECORDCORE   mrcStd;
   BOOL             bEmphasized;
   ULONG            ulNumUnits;
   float            fSales;
   PCHAR            pchSales;
} SALESINFO,*PSALESINFO;

MRESULT EXPENTRY clientWndProc(HWND hwndClient,ULONG ulMsg,MPARAM 
                                mpParm1,MPARAM mpParm2);

INT main(VOID)
{
   HAB              habAnchor;
   HMQ              hmqQueue;
   ULONG            ulFlags;
   HWND             hwndFrame;
   BOOL             bLoop;
   QMSG             qmMsg;

   habAnchor = WinInitialize(0);
   hmqQueue = WinCreateMsgQueue(habAnchor, 
                                0);

   WinRegisterClass(habAnchor, 
                    CLS_CLIENT, 
                    clientWndProc, 
                    0, 
                    sizeof(PVOID));

   ulFlags = FCF_SIZEBORDER|FCF_TITLEBAR|FCF_TASKLIST|
      FCF_SHELLPOSITION|FCF_SYSMENU;

   hwndFrame = WinCreateStdWindow(HWND_DESKTOP, 
                                  WS_VISIBLE, 
                                  &ulFlags, 
                                  CLS_CLIENT, 
                                  "Container Sample", 
                                  0, 
                                  NULLHANDLE, 
                                  RES_CLIENT, 
                                  NULL);

   if (hwndFrame != NULLHANDLE)
   {
      bLoop = WinGetMsg(habAnchor, 
                        &qmMsg, 
                        NULLHANDLE, 
                        0, 
                        0);
      while (bLoop)
      {
         WinDispatchMsg(habAnchor, 
                        &qmMsg);
         bLoop = WinGetMsg(habAnchor, 
                           &qmMsg, 
                           NULLHANDLE, 
                           0, 
                           0);
      }                                /* endwhile                 */
      WinDestroyWindow(hwndFrame);
   }                                   /* endif                    */
   WinDestroyMsgQueue(hmqQueue);
   WinTerminate(habAnchor);
   return 0;
} 

VOID initSalesInfo(PCLIENTDATA pcdData,PSALESINFO psiParent,
                    PSALESINFO psiSales,USHORT usIndex)
{
   PCHAR            pchPos;

   psiSales->mrcStd.cb = sizeof(MINIRECORDCORE);

   psiSales->mrcStd.pszIcon = malloc(256);
   if (psiSales->mrcStd.pszIcon != NULL)
   {

      if (psiParent != NULL)
      {
         sprintf(psiSales->mrcStd.pszIcon, 
                 "Month %d", 
                 usIndex+1);
      } 
      else
      {
         sprintf(psiSales->mrcStd.pszIcon, 
                 "Year 19%02d", 
                 usIndex+84);
      }                                /* endif                    */
   }                                   /* endif                    */
   psiSales->mrcStd.hptrIcon = pcdData->hptrIcon;
   psiSales->bEmphasized = FALSE;

   if (psiParent != NULL)
   {
      psiSales->ulNumUnits = psiParent->ulNumUnits/12;
   } 
   else
   {
      psiSales->ulNumUnits = usIndex *100;
   }                                   /* endif                    */
   psiSales->fSales = (float)psiSales->ulNumUnits *9.95;

   psiSales->pchSales = malloc(16);

   if (psiSales->pchSales != NULL)
   {
      sprintf(psiSales->pchSales, 
              "$%-10.2f", 
              psiSales->fSales);

      pchPos = psiSales->pchSales;
      while (!isspace(*pchPos) && (*pchPos != 0))
      {
         pchPos++;
      }                                /* endwhile                 */
      *pchPos = 0;
   }                                   /* endif                    */
   return ;
} 

VOID emphasizeRecs(HWND hwndCnr,BOOL bEmphasize)
{
   SHORT            sFlag;
   PSALESINFO       psiYear;

   sFlag = ((bEmphasize)?CRA_SELECTED:CRA_SOURCE);

   psiYear = (PSALESINFO)PVOIDFROMMR(WinSendMsg(hwndCnr, 
                                               CM_QUERYRECORDEMPHASIS
                                                   , 
                                                MPFROMP(CMA_FIRST), 
                                                MPFROMSHORT(sFlag)));

   while (psiYear != NULL)
   {
      if (bEmphasize)
      {
         WinSendMsg(hwndCnr, 
                    CM_SETRECORDEMPHASIS, 
                    MPFROMP(psiYear), 
                    MPFROM2SHORT(TRUE, 
                                 CRA_SOURCE));

         psiYear->bEmphasized = TRUE;
      } 
      else
      {
         WinSendMsg(hwndCnr, 
                    CM_SETRECORDEMPHASIS, 
                    MPFROMP(psiYear), 
                    MPFROM2SHORT(FALSE, 
                                 CRA_SOURCE));

         psiYear->bEmphasized = FALSE;
      }                                /* endif                    */
      psiYear = (PSALESINFO)PVOIDFROMMR(WinSendMsg(hwndCnr, 
                                               CM_QUERYRECORDEMPHASIS
                                                      , 
                                                   MPFROMP(psiYear), 
                                                   MPFROMSHORT(sFlag)
                                                   ));
   }                                   /* endwhile                 */
   return ;
} 

VOID freeCnrInfo(HWND hwndCnr)
{
   PSALESINFO       psiYear;
   PSALESINFO       psiMonth;

   psiYear = (PSALESINFO)PVOIDFROMMR(WinSendMsg(hwndCnr, 
                                                CM_QUERYRECORD, 
                                                0, 
                                                MPFROM2SHORT
                                                   (CMA_FIRST, 
                                                    CMA_ITEMORDER)));
   while (psiYear != NULL)
   {
      psiMonth = (PSALESINFO)PVOIDFROMMR(WinSendMsg(hwndCnr, 
                                                    CM_QUERYRECORD, 
                                                    MPFROMP(psiYear),
                                                    MPFROM2SHORT
                                                    (CMA_FIRSTCHILD, 
                                                     CMA_ITEMORDER)))
         ;
      while (psiMonth != NULL)
      {
         if (psiMonth->mrcStd.pszIcon != NULL)
         {
            free(psiMonth->mrcStd.pszIcon);
         }                             /* endif                    */
         if (psiMonth->pchSales != NULL)
         {
            free(psiMonth->pchSales);
         }                             /* endif                    */
         psiMonth = (PSALESINFO)PVOIDFROMMR(WinSendMsg(hwndCnr, 
                                                       CM_QUERYRECORD
                                                          , 
                                                       MPFROMP
                                                       (psiMonth), 
                                                       MPFROM2SHORT
                                                       (CMA_NEXT, 
                                                        CMA_ITEMORDER
                                                        )));
      }                                /* endwhile                 */
      if (psiYear->mrcStd.pszIcon != NULL)
      {
         free(psiYear->mrcStd.pszIcon);
      }                                /* endif                    */
      if (psiYear->pchSales != NULL)
      {
         free(psiYear->pchSales);
      }                                /* endif                    */
      psiYear = (PSALESINFO)PVOIDFROMMR(WinSendMsg(hwndCnr, 
                                                   CM_QUERYRECORD, 
                                                   MPFROMP(psiYear), 
                                                   MPFROM2SHORT
                                                      (CMA_NEXT, 
                                                       CMA_ITEMORDER)
                                                       ));
   }                                   /* endwhile                 */
   return ;
} 

VOID initColumns(PCLIENTDATA pcdData)
{
   CNRINFO          ciInfo;
   PFIELDINFO       pfiCurrent;
   PFIELDINFO       pfiInfo;
   PFIELDINFO       pfiLefty;
   FIELDINFOINSERT  fiiInfo;

   pfiInfo = (PFIELDINFO)PVOIDFROMMR(WinSendMsg(pcdData->hwndCnr, 
                                              CM_ALLOCDETAILFIELDINFO
                                                   , 
                                                MPFROMLONG
                                                (MAX_COLUMNS), 
                                                0));

   pfiCurrent = pfiInfo;

   pfiCurrent->flData = CFA_BITMAPORICON|CFA_HORZSEPARATOR|CFA_CENTER
      |CFA_SEPARATOR;

   pfiCurrent->flTitle = CFA_STRING|CFA_CENTER;
   pfiCurrent->pTitleData = "Icon";
   pfiCurrent->offStruct = FIELDOFFSET(SALESINFO, 
                                       mrcStd.hptrIcon);

   pfiCurrent = pfiCurrent->pNextFieldInfo;

   pfiCurrent->flData = CFA_STRING|CFA_CENTER|CFA_HORZSEPARATOR;

   pfiCurrent->flTitle = CFA_STRING|CFA_CENTER;
   pfiCurrent->pTitleData = "Year";
   pfiCurrent->offStruct = FIELDOFFSET(SALESINFO, 
                                       mrcStd.pszIcon);

   pfiLefty = pfiCurrent;

   pfiCurrent = pfiCurrent->pNextFieldInfo;
   pfiCurrent->flData = CFA_ULONG|CFA_CENTER|CFA_HORZSEPARATOR|
      CFA_SEPARATOR;

   pfiCurrent->flTitle = CFA_STRING|CFA_CENTER;
   pfiCurrent->pTitleData = "Units Sold";
   pfiCurrent->offStruct = FIELDOFFSET(SALESINFO, 
                                       ulNumUnits);

   pfiCurrent = pfiCurrent->pNextFieldInfo;
   pfiCurrent->flData = CFA_STRING|CFA_RIGHT|CFA_HORZSEPARATOR;

   pfiCurrent->flTitle = CFA_STRING|CFA_CENTER;
   pfiCurrent->pTitleData = "Sales";
   pfiCurrent->offStruct = FIELDOFFSET(SALESINFO, 
                                       pchSales);

   fiiInfo.cb = sizeof(fiiInfo);
   fiiInfo.pFieldInfoOrder = (PFIELDINFO)CMA_FIRST;
   fiiInfo.cFieldInfoInsert = MAX_COLUMNS;
   fiiInfo.fInvalidateFieldInfo = TRUE;

   WinSendMsg(pcdData->hwndCnr, 
              CM_INSERTDETAILFIELDINFO, 
              MPFROMP(pfiInfo), 
              MPFROMP(&fiiInfo));

   memset(&ciInfo, 
          0, 
          sizeof(ciInfo));
   ciInfo.cb = sizeof(CNRINFO);
   ciInfo.pFieldInfoLast = pfiLefty;
   ciInfo.xVertSplitbar = CX_SPLITBAR;

   WinSendMsg(pcdData->hwndCnr, 
              CM_SETCNRINFO, 
              MPFROMP(&ciInfo), 
              MPFROMLONG(CMA_PFIELDINFOLAST|CMA_XVERTSPLITBAR));
   return ;
} 

MRESULT EXPENTRY clientWndProc(HWND hwndClient,ULONG ulMsg,MPARAM 
                                mpParm1,MPARAM mpParm2)
{
   PCLIENTDATA      pcdData;

   pcdData = (PCLIENTDATA)WinQueryWindowPtr(hwndClient, 
                                            0);

   switch (ulMsg)
   {
      case  WM_CREATE :
         {
            MENUITEM         miItem;
            ULONG            ulStyle;
            ULONG            ulExtra;
            RECORDINSERT     riRecord;
            PSALESINFO       psiYears;
            PSALESINFO       psiCYear;
            USHORT           usIndex1;
            PSALESINFO       psiMonths;
            PSALESINFO       psiCMonth;
            USHORT           usIndex2;

            pcdData = (PCLIENTDATA)malloc(sizeof(CLIENTDATA));
            if (pcdData == NULL)
            {
               WinAlarm(HWND_DESKTOP, 
                        WA_ERROR);
               WinMessageBox(HWND_DESKTOP, 
                             HWND_DESKTOP, 
                             "No memory is available", 
                             "Error", 
                             0, 
                             MB_ICONEXCLAMATION|MB_OK);
               return  MRFROMSHORT(TRUE);
            }                          /* endif                    */

            WinSetWindowPtr(hwndClient, 
                            0, 
                            pcdData);

            pcdData->hwndCnr = NULLHANDLE;
            pcdData->hptrIcon = NULLHANDLE;
            pcdData->hwndMenu = NULLHANDLE;
            pcdData->bCnrSelected = FALSE;

            pcdData->hwndCnr = WinCreateWindow(hwndClient, 
                                               WC_CONTAINER, 
                                               "", 
                                               CCS_MINIRECORDCORE|
                                                  CCS_EXTENDSEL|
                                                  WS_VISIBLE, 
                                               0, 
                                               0, 
                                               0, 
                                               0, 
                                               hwndClient, 
                                               HWND_TOP, 
                                               WND_CONTAINER, 
                                               NULL, 
                                               NULL);

            if (pcdData->hwndCnr == NULLHANDLE)
            {
               free(pcdData);
               WinAlarm(HWND_DESKTOP, 
                        WA_ERROR);
               WinMessageBox(HWND_DESKTOP, 
                             HWND_DESKTOP, 
                             "Cannot create container", 
                             "Error", 
                             0, 
                             MB_ICONEXCLAMATION|MB_OK);
               return  MRFROMSHORT(TRUE);
            }                          /* endif                    */

            pcdData->hptrIcon = WinLoadPointer(HWND_DESKTOP, 
                                               NULLHANDLE, 
                                               ICO_ITEM);

            pcdData->hwndMenu = WinLoadMenu(hwndClient, 
                                            NULLHANDLE, 
                                            RES_CLIENT);
            WinSendMsg(pcdData->hwndMenu, 
                       MM_QUERYITEM, 
                       MPFROM2SHORT(M_VIEWS, 
                                    TRUE), 
                       MPFROMP(&miItem));

            ulStyle = WinQueryWindowULong(miItem.hwndSubMenu, 
                                          QWL_STYLE);

            ulStyle |= MS_CONDITIONALCASCADE;
            WinSetWindowULong(miItem.hwndSubMenu, 
                              QWL_STYLE, 
                              ulStyle);

            WinSendMsg(miItem.hwndSubMenu, 
                       MM_SETDEFAULTITEMID, 
                       MPFROMSHORT(MI_ICON), 
                       0);

            ulExtra = sizeof(SALESINFO)-sizeof(MINIRECORDCORE);

            riRecord.cb = sizeof(RECORDINSERT);
            riRecord.pRecordOrder = (PRECORDCORE)CMA_END;
            riRecord.fInvalidateRecord = FALSE;
            riRecord.zOrder = CMA_TOP;

            psiYears = (PSALESINFO)PVOIDFROMMR(WinSendMsg(pcdData->
               hwndCnr, 
                                                       CM_ALLOCRECORD
                                                          , 
                                                          MPFROMLONG
                                                          (ulExtra), 
                                                          MPFROMSHORT
                                                          (MAX_YEARS)
                                                          ));

            psiCYear = psiYears;

            for (usIndex1 = 0; usIndex1 < MAX_YEARS; usIndex1++)
            {

               initSalesInfo(pcdData, 
                             NULL, 
                             psiCYear, 
                             usIndex1);

               riRecord.pRecordParent = NULL;
               riRecord.cRecordsInsert = 1;

               WinSendMsg(pcdData->hwndCnr, 
                          CM_INSERTRECORD, 
                          MPFROMP(psiCYear), 
                          MPFROMP(&riRecord));

               psiMonths = (PSALESINFO)PVOIDFROMMR(WinSendMsg
                  (pcdData->hwndCnr, 
                   CM_ALLOCRECORD, 
                   MPFROMLONG(ulExtra), 
                   MPFROMSHORT(MAX_MONTHS)));

               psiCMonth = psiMonths;

               for (usIndex2 = 0; usIndex2 < MAX_MONTHS; usIndex2++)
               {

                  initSalesInfo(pcdData, 
                                psiCYear, 
                                psiCMonth, 
                                usIndex2);

                  psiCMonth = (PSALESINFO)
                     psiCMonth->mrcStd.preccNextRecord;
               }                       /* endfor                   */
               riRecord.pRecordParent = (PRECORDCORE)psiCYear;
               riRecord.cRecordsInsert = MAX_MONTHS;

               WinSendMsg(pcdData->hwndCnr, 
                          CM_INSERTRECORD, 
                          MPFROMP(psiMonths), 
                          MPFROMP(&riRecord));

               psiCYear = (PSALESINFO)
                  psiCYear->mrcStd.preccNextRecord;
            }                          /* endfor                   */
            initColumns(pcdData);

            WinSendMsg(hwndClient, 
                       WM_COMMAND, 
                       MPFROMSHORT(MI_ICON), 
                       0);
         } 
         break;

      case  WM_DESTROY :
         freeCnrInfo(pcdData->hwndCnr);

         if (pcdData->hwndCnr != NULLHANDLE)
         {
            WinDestroyWindow(pcdData->hwndCnr);
         }                             /* endif                    */
         if (pcdData->hptrIcon != NULLHANDLE)
         {
            WinDestroyPointer(pcdData->hptrIcon);
         }                             /* endif                    */
         free(pcdData);
         break;

      case  WM_SIZE :
         if (pcdData->hwndCnr != NULLHANDLE)
         {

            WinSetWindowPos(pcdData->hwndCnr, 
                            NULLHANDLE, 
                            0, 
                            0, 
                            SHORT1FROMMP(mpParm2), 
                            SHORT2FROMMP(mpParm2), 
                            SWP_MOVE|SWP_SIZE);
         }                             /* endif                    */
         break;

      case  WM_MENUEND :
         switch (SHORT1FROMMP(mpParm1))
         {
            case  FID_MENU :
               if (pcdData->bCnrSelected)
               {
                  WinSendMsg(pcdData->hwndCnr, 
                             CM_SETRECORDEMPHASIS, 
                             0, 
                             MPFROM2SHORT(FALSE, 
                                          CRA_SOURCE));
                  pcdData->bCnrSelected = FALSE;
               } 
               else
               {
                  emphasizeRecs(pcdData->hwndCnr, 
                                FALSE);
               }                       /* endif                    */
               break;

            default  :
               return  WinDefWindowProc(hwndClient, 
                                        ulMsg, 
                                        mpParm1, 
                                        mpParm2);

         }                             /* endswitch                */
         break;

      case  WM_CONTROL :
         switch (SHORT1FROMMP(mpParm1))
         {
            case  WND_CONTAINER :
               switch (SHORT2FROMMP(mpParm1))
               {
                  case  CN_CONTEXTMENU :
                     {
                        PSALESINFO       psiSales;
                        POINTL           ptlMouse;

                        psiSales = (PSALESINFO)PVOIDFROMMP(mpParm2);
                        if (psiSales != NULL)
                        {
                           if ((psiSales->mrcStd.flRecordAttr
                              &CRA_SELECTED) == 0)
                           {
                              WinSendMsg(pcdData->hwndCnr, 
                                         CM_SETRECORDEMPHASIS, 
                                         MPFROMP(psiSales), 
                                         MPFROM2SHORT(TRUE, 
                                                      CRA_SOURCE));
                              psiSales->bEmphasized = TRUE;
                           } 
                           else
                           {
                              emphasizeRecs(pcdData->hwndCnr, 
                                            TRUE);
                           }           /* endif                    */
                        } 
                        else
                        {
                           WinSendMsg(pcdData->hwndCnr, 
                                      CM_SETRECORDEMPHASIS, 
                                      0, 
                                      MPFROM2SHORT(TRUE, 
                                                   CRA_SOURCE));
                           pcdData->bCnrSelected = TRUE;
                        }              /* endif                    */
                        WinQueryPointerPos(HWND_DESKTOP, 
                                           &ptlMouse);
                        WinMapWindowPoints(HWND_DESKTOP, 
                                           hwndClient, 
                                           &ptlMouse, 
                                           1);
                        WinPopupMenu(hwndClient, 
                                     hwndClient, 
                                     pcdData->hwndMenu, 
                                     ptlMouse.x, 
                                     ptlMouse.y, 
                                     M_VIEWS, 
                                     PU_HCONSTRAIN|PU_VCONSTRAIN|
                                        PU_KEYBOARD|PU_MOUSEBUTTON1|
                                        PU_MOUSEBUTTON2|PU_NONE);
                     } 
                     break;

                  default  :
                     return  WinDefWindowProc(hwndClient, 
                                              ulMsg, 
                                              mpParm1, 
                                              mpParm2);

               }                       /* endswitch                */
               break;

            default  :
               return  WinDefWindowProc(hwndClient, 
                                        ulMsg, 
                                        mpParm1, 
                                        mpParm2);

         }                             /* endswitch                */
         break;

      case  WM_COMMAND :
         switch (SHORT1FROMMP(mpParm1))
         {
            case  MI_ICON :
               {
                  CNRINFO          ciInfo;

                  ciInfo.cb = sizeof(CNRINFO);
                  ciInfo.flWindowAttr = CV_ICON;

                  WinSendMsg(pcdData->hwndCnr, 
                             CM_SETCNRINFO, 
                             MPFROMP(&ciInfo), 
                             MPFROMLONG(CMA_FLWINDOWATTR));

                  WinSendMsg(pcdData->hwndCnr, 
                             CM_ARRANGE, 
                             NULL, 
                             NULL);
               } 
               break;

            case  MI_DETAIL :
               {
                  CNRINFO          ciInfo;

                  ciInfo.cb = sizeof(CNRINFO);
                  ciInfo.flWindowAttr = CV_DETAIL|
                     CA_DETAILSVIEWTITLES;

                  WinSendMsg(pcdData->hwndCnr, 
                             CM_SETCNRINFO, 
                             MPFROMP(&ciInfo), 
                             MPFROMLONG(CMA_FLWINDOWATTR));
               } 
               break;

            case  MI_TREE :
               {
                  CNRINFO          ciInfo;

                  ciInfo.cb = sizeof(CNRINFO);
                  ciInfo.flWindowAttr = CV_TREE|CV_ICON|CA_TREELINE;
                  ciInfo.cxTreeIndent = -1;
                  ciInfo.cxTreeLine = -1;

                  WinSendMsg(pcdData->hwndCnr, 
                             CM_SETCNRINFO, 
                             MPFROMP(&ciInfo), 
                             MPFROMLONG(CMA_FLWINDOWATTR));
               } 
               break;

            case  MI_NAMEFLOWED :
               {
                  CNRINFO          ciInfo;

                  ciInfo.cb = sizeof(CNRINFO);
                  ciInfo.flWindowAttr = CV_NAME|CV_FLOW;

                  WinSendMsg(pcdData->hwndCnr, 
                             CM_SETCNRINFO, 
                             MPFROMP(&ciInfo), 
                             MPFROMLONG(CMA_FLWINDOWATTR));

                  WinSendMsg(pcdData->hwndCnr, 
                             CM_ARRANGE, 
                             NULL, 
                             NULL);
               } 
               break;

            case  MI_TEXTFLOWED :
               {
                  CNRINFO          ciInfo;

                  ciInfo.cb = sizeof(CNRINFO);
                  ciInfo.flWindowAttr = CV_TEXT|CV_FLOW;

                  WinSendMsg(pcdData->hwndCnr, 
                             CM_SETCNRINFO, 
                             MPFROMP(&ciInfo), 
                             MPFROMLONG(CMA_FLWINDOWATTR));

                  WinSendMsg(pcdData->hwndCnr, 
                             CM_ARRANGE, 
                             NULL, 
                             NULL);
               } 
               break;

            case  MI_EXIT :
               WinPostMsg(hwndClient, 
                          WM_CLOSE, 
                          0, 
                          0);
               break;

            case  MI_RESUME :
               break;

            default  :
               return  WinDefWindowProc(hwndClient, 
                                        ulMsg, 
                                        mpParm1, 
                                        mpParm2);

         }                             /* endswitch                */
         break;

      case  WM_PAINT :
         {
            HPS              hpsPaint;
            RECTL            rclPaint;

            hpsPaint = WinBeginPaint(hwndClient, 
                                     NULLHANDLE, 
                                     &rclPaint);

            WinFillRect(hpsPaint, 
                        &rclPaint, 
                        SYSCLR_WINDOW);
            WinEndPaint(hpsPaint);
         } 
         break;

      default  :
         return  WinDefWindowProc(hwndClient, 
                                  ulMsg, 
                                  mpParm1, 
                                  mpParm2);

   }                                   /* endswitch                */
   return  MRFROMSHORT(FALSE);
} 

