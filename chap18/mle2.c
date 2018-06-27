#define INCL_WINFRAMEMGR
#define INCL_WINMLE
#define INCL_WINMENUS
#define INCL_WINWINDOWMGR
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mle2rc.h"

#define CLS_CLIENT               "MLE2SampleClass"

#define MYM_SETFOCUS             (WM_USER)

typedef struct _INSTDATA {
   ULONG ulSzStruct;             // Size of the structure
   HAB habAnchor;                // Anchor block handle
   HWND hwndFrame;               // Frame window handle
   HWND hwndMle;                 // MLE window handle
} INSTDATA, *PINSTDATA;

VOID addText(HWND hwndMle)
//---------------------------------------------------------------
// This function adds text to the MLE starting at position 0.
//
// Input:  hwndMle - MLE window handle
//---------------------------------------------------------------
{
   CHAR achImpExp[256];
   USHORT usIndex;
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

   //------------------------------------------------------------
   // Insert 20 lines of text.
   //------------------------------------------------------------
   WinEnableWindowUpdate(hwndMle,FALSE);

   iInsert=0;

   for (usIndex=1; usIndex<=20; usIndex++) {
      sprintf(achImpExp,"This is line %d.\n",usIndex);

      WinSendMsg(hwndMle,
                 MLM_IMPORT,
                 MPFROMP(&iInsert),
                 MPFROMLONG(strlen(achImpExp)));
   } /* endfor */

   WinEnableWindowUpdate(hwndMle,TRUE);
}

VOID selectAllText(HWND hwndMle)
//---------------------------------------------------------------
// This function selects all of the text.
//
// Input:  hwndMle - MLE window handle
//---------------------------------------------------------------
{
   ULONG ulSzText;

   //------------------------------------------------------------
   // Query the amount of text in the MLE.  This will be our
   // cursor point.  Our anchor point is always 0.
   //------------------------------------------------------------
   ulSzText=LONGFROMMR(WinSendMsg(hwndMle,
                                  MLM_QUERYTEXTLENGTH,
                                  0,
                                  0));

   WinSendMsg(hwndMle,
              MLM_SETSEL,
              MPFROMLONG(0),
              MPFROMLONG(ulSzText));
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
      // because PM is still in the focus change processing
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
      case MI_ADDTEXT:
         addText(pidData->hwndMle);
         break;
      case MI_SELECTALL:
         selectAllText(pidData->hwndMle);
         break;
      case MI_READONLY:
         {
            BOOL bReadOnly;
            HWND hwndMenu;

            //---------------------------------------------------
            // Query the current read-only state and toggle.
            //---------------------------------------------------
            bReadOnly=SHORT1FROMMR(
                         WinSendMsg(pidData->hwndMle,
                                    MLM_QUERYREADONLY,
                                    0,
                                    0));
            bReadOnly=!bReadOnly;

            //---------------------------------------------------
            // Set the new read-only state.
            //---------------------------------------------------
            WinSendMsg(pidData->hwndMle,
                       MLM_SETREADONLY,
                       MPFROMSHORT(bReadOnly),
                       0);

            //---------------------------------------------------
            // Check or uncheck the menu item.
            //---------------------------------------------------
            hwndMenu=WinWindowFromID(pidData->hwndFrame,
                                     FID_MENU);
            WinSendMsg(hwndMenu,
                       MM_SETITEMATTR,
                       MPFROM2SHORT(MI_READONLY,TRUE),
                       MPFROM2SHORT(MIA_CHECKED,
                                    bReadOnly?MIA_CHECKED:0));
         }
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
                                "MLE Sample 2",
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
