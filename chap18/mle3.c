#define INCL_WINFRAMEMGR
#define INCL_WINMLE
#define INCL_WINMENUS
#define INCL_WINWINDOWMGR
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mle3rc.h"

#define CLS_CLIENT               "MLE3SampleClass"

#define MYM_SETFOCUS             (WM_USER)

typedef struct _INSTDATA {
   ULONG ulSzStruct;             // Size of the structure
   HAB habAnchor;                // Anchor block handle
   HWND hwndFrame;               // Frame window handle
   HWND hwndMle;                 // MLE window handle
} INSTDATA, *PINSTDATA;

VOID importText(HWND hwndMle)
//---------------------------------------------------------------
// This function imports text into the MLE.
//
// Input:  hwndMle - MLE window handle
//---------------------------------------------------------------
{
   FILE *pfImport;
   CHAR achImpExp[256];
   IPT iInsert;

   //------------------------------------------------------------
   // Set the import/export buffer and the format to
   // MLFIE_NOTRANS.  Remember that, internally, a '\n'
   // character is a LF only.  Only when it gets output to a
   // "text-mode" stream does it get converted to CR-LF.
   //------------------------------------------------------------
   WinSendMsg(hwndMle,
              MLM_SETIMPORTEXPORT,
              MPFROMP(achImpExp),
              MPFROMLONG(sizeof(achImpExp)));
   WinSendMsg(hwndMle,
              MLM_FORMAT,
              MPFROMLONG(MLFIE_NOTRANS),
              0);

   pfImport=fopen("MLE3.IMP","r");
   if (pfImport==NULL) {
      WinAlarm(HWND_DESKTOP,WA_ERROR);
      return;
   } /* endif */

   WinEnableWindowUpdate(hwndMle,FALSE);

   iInsert=0;

   while (fgets(achImpExp,sizeof(achImpExp),pfImport)!=NULL) {
      WinSendMsg(hwndMle,
                 MLM_IMPORT,
                 MPFROMP(&iInsert),
                 MPFROMLONG(strlen(achImpExp)));
   } /* endwhile */

   WinEnableWindowUpdate(hwndMle,TRUE);

   fclose(pfImport);
}

VOID exportText(HWND hwndMle)
//---------------------------------------------------------------
// This function exports text from the MLE.
//
// Input:  hwndMle - MLE window handle
//---------------------------------------------------------------
{
   FILE *pfExport;
   CHAR achImpExp[256];
   LONG lNumLines;
   LONG lIndex;
   IPT iBegin;
   LONG lSzLine;

   //------------------------------------------------------------
   // Set the import/export buffer and the format to
   // MLFIE_NOTRANS.  Remember that, internally, a '\n'
   // character is a LF only.  Only when it gets output to a
   // "text-mode" stream does it get converted to CR-LF.
   //------------------------------------------------------------
   WinSendMsg(hwndMle,
              MLM_SETIMPORTEXPORT,
              MPFROMP(achImpExp),
              MPFROMLONG(sizeof(achImpExp)));
   WinSendMsg(hwndMle,
              MLM_FORMAT,
              MPFROMLONG(MLFIE_NOTRANS),
              0);

   pfExport=fopen("MLE3.EXP","w");
   if (pfExport==NULL) {
      WinAlarm(HWND_DESKTOP,WA_ERROR);
      return;
   } /* endif */

   WinEnableWindowUpdate(hwndMle,FALSE);

   lNumLines=LONGFROMMR(WinSendMsg(hwndMle,
                                   MLM_QUERYLINECOUNT,
                                   0,
                                   0));

   for (lIndex=0; lIndex<lNumLines; lIndex++) {
      iBegin=LONGFROMMR(WinSendMsg(hwndMle,
                                   MLM_CHARFROMLINE,
                                   MPFROMLONG(lIndex),
                                   0));
      lSzLine=LONGFROMMR(WinSendMsg(hwndMle,
                                    MLM_QUERYLINELENGTH,
                                    MPFROMLONG(iBegin),
                                    0));

      memset(achImpExp,0,sizeof(achImpExp));

      WinSendMsg(hwndMle,
                 MLM_EXPORT,
                 MPFROMP(&iBegin),
                 MPFROMP(&lSzLine));

      fputs(achImpExp,pfExport);
   } /* endfor */

   WinEnableWindowUpdate(hwndMle,TRUE);

   fclose(pfExport);
}

MRESULT EXPENTRY clientWndProc(HWND hwndWnd,
                               ULONG ulMsg,
                               MPARAM mpParm1,
                               MPARAM mpParm2)
//---------------------------------------------------------------
// Client window procedure.
//
// Input, Output, Returns:  message-specific
//---------------------------------------------------------------
{
   PINSTDATA pidData;

   pidData=(PINSTDATA)WinQueryWindowPtr(hwndWnd,0);

   switch (ulMsg) {
   case WM_CREATE:
      //---------------------------------------------------------
      // Allocate memory for the instance data.
      //---------------------------------------------------------
      pidData=calloc(1,sizeof(INSTDATA));
      if (pidData==NULL) {
         WinAlarm(HWND_DESKTOP,WA_ERROR);
         return MRFROMSHORT(TRUE);
      } /* endif */

      //---------------------------------------------------------
      // Initialize the instance data.
      //---------------------------------------------------------
      pidData->ulSzStruct=sizeof(INSTDATA);

      WinSetWindowPtr(hwndWnd,0,pidData);

      pidData->habAnchor=WinQueryAnchorBlock(hwndWnd);
      pidData->hwndFrame=WinQueryWindow(hwndWnd,QW_PARENT);

      //---------------------------------------------------------
      // Create the MLE.
      //---------------------------------------------------------
      pidData->hwndMle=WinCreateWindow(hwndWnd,
                                       WC_MLE,
                                       "",
                                       WS_VISIBLE |
                                          MLS_HSCROLL |
                                          MLS_VSCROLL |
                                          MLS_BORDER,
                                       0,
                                       0,
                                       0,
                                       0,
                                       hwndWnd,
                                       HWND_TOP,
                                       WND_MLE,
                                       NULL,
                                       NULL);
      if (pidData->hwndMle==NULLHANDLE) {
         WinAlarm(HWND_DESKTOP,WA_ERROR);
         free(pidData);
         return MRFROMSHORT(TRUE);
      } /* endif */
      break;
   case WM_DESTROY:
      free(pidData);
      break;
   case WM_SIZE:
      //---------------------------------------------------------
      // Resize the MLE according to our new dimensions.
      //---------------------------------------------------------
      WinSetWindowPos(pidData->hwndMle,
                      NULLHANDLE,
                      0,
                      0,
                      SHORT1FROMMP(mpParm2),
                      SHORT2FROMMP(mpParm2),
                      SWP_SIZE);
      break;
   case WM_SETFOCUS:
      //---------------------------------------------------------
      // If we are getting the focus, post ourselves a message
      // to change it to the MLE.  We may not do it now,
      // because PM is still in the focus change processing,
      // meaning that any call to WinSetFocus() will get
      // overwritten when the focus change processing completes.
      //---------------------------------------------------------
      if (SHORT1FROMMP(mpParm2)) {
         WinPostMsg(hwndWnd,MYM_SETFOCUS,0,0);
      } /* endif */
      break;
   case MYM_SETFOCUS:
      WinSetFocus(HWND_DESKTOP,pidData->hwndMle);
      break;
   case WM_COMMAND:
      switch (SHORT1FROMMP(mpParm1)) {
      case MI_IMPORTTEXT:
         importText(pidData->hwndMle);
         break;
      case MI_EXPORTTEXT:
         exportText(pidData->hwndMle);
         break;
      case MI_EXIT:
         WinPostMsg(hwndWnd,WM_CLOSE,0,0);
         break;
      default:
         return WinDefWindowProc(hwndWnd,ulMsg,mpParm1,mpParm2);
      } /* endswitch */
      break;
   default:
      return WinDefWindowProc(hwndWnd,ulMsg,mpParm1,mpParm2);
   } /* endswitch */

   return MRFROMSHORT(FALSE);
}

INT main(VOID)
//---------------------------------------------------------------
// Standard main function for PM applications.
//---------------------------------------------------------------
{
   HAB habAnchor;
   HMQ hmqQueue;
   ULONG ulCreate;
   HWND hwndFrame;
   HWND hwndClient;
   BOOL bLoop;
   QMSG qmMsg;

   habAnchor=WinInitialize(0);
   hmqQueue=WinCreateMsgQueue(habAnchor,0);

   WinRegisterClass(habAnchor,
                    CLS_CLIENT,
                    clientWndProc,
                    CS_SIZEREDRAW,
                    sizeof(PVOID));

   ulCreate=FCF_SYSMENU | FCF_TITLEBAR | FCF_MINMAX |
               FCF_SIZEBORDER | FCF_MENU | FCF_SHELLPOSITION |
               FCF_TASKLIST;

   hwndFrame=WinCreateStdWindow(HWND_DESKTOP,
                                WS_VISIBLE,
                                &ulCreate,
                                CLS_CLIENT,
                                "MLE Sample 3",
                                0,
                                NULLHANDLE,
                                RES_CLIENT,
                                &hwndClient);
   if (hwndFrame!=NULLHANDLE) {
      bLoop=WinGetMsg(habAnchor,&qmMsg,NULLHANDLE,0,0);
      while (bLoop) {
         WinDispatchMsg(habAnchor,&qmMsg);
         bLoop=WinGetMsg(habAnchor,&qmMsg,NULLHANDLE,0,0);
      } /* endwhile */

      WinDestroyWindow(hwndFrame);
   } /* endif */

   WinDestroyMsgQueue(hmqQueue);
   WinTerminate(habAnchor);
   return 0;
}
