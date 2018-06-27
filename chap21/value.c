#define  INCL_WIN
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "value.h"
#define  CLS_CLIENT    "MyClass"
#define  USRM_UPDATE   ( WM_USER )
static LONG alColors[] =
{

   CLR_BLUE,
   CLR_PINK,
   CLR_GREEN,
   CLR_CYAN,
   CLR_YELLOW,
   CLR_NEUTRAL,
   CLR_DARKGRAY,
   CLR_DARKBLUE,
   CLR_DARKRED,
   CLR_DARKPINK,
   CLR_DARKGREEN,
   CLR_DARKCYAN
} ;

typedef          struct
{
   SHORT            sColor;
   HWND             hwndDlg;
} WNDDATA,*PWNDDATA;

MRESULT EXPENTRY DlgProc(HWND hwndDlg,ULONG ulMsg,MPARAM mpParm1,
                          MPARAM mpParm2);

MRESULT EXPENTRY ClientWndProc(HWND hwndWnd,ULONG ulMsg,MPARAM
                                mpParm1,MPARAM mpParm2);

VOID DisplayError(CHAR *pszText);
VOID ProcessSelect(HWND hwndDlg,MPARAM mpParm2);

INT main(VOID)
{

   HAB              habAnchor;
   HMQ              hmqQueue;
   ULONG            ulFlags;
   HWND             hwndFrame;
   BOOL             bLoop;
   QMSG             qmMsg;
   LONG             lWidth,lHeight;

   habAnchor = WinInitialize(0);
   hmqQueue = WinCreateMsgQueue(habAnchor,
                                0);

   WinRegisterClass(habAnchor,
                    CLS_CLIENT,
                    ClientWndProc,
                    0,
                    sizeof(PVOID));

   ulFlags = FCF_TITLEBAR|FCF_SYSMENU|FCF_SIZEBORDER|FCF_MINMAX|
      FCF_MENU|FCF_TASKLIST;

   /************************************************************/
   /* create frame window                                      */
   /************************************************************/

   hwndFrame = WinCreateStdWindow(HWND_DESKTOP,
                                  0,
                                  &ulFlags,
                                  CLS_CLIENT,
                                  "Value Set Example",
                                  0,
                                  NULLHANDLE,
                                  ID_FRAME,
                                  NULL);

   /************************************************************/
   /* get screen height and width                              */
   /************************************************************/

   lWidth = WinQuerySysValue(HWND_DESKTOP,
                             SV_CXSCREEN);

   lHeight = WinQuerySysValue(HWND_DESKTOP,
                              SV_CYSCREEN);

   /************************************************************/
   /* if failed, display error, and set to default value       */
   /************************************************************/

   if (!lWidth || !lHeight)
   {
      DisplayError("WinQuerySysValue failed");
      lWidth = 640;
      lHeight = 480;
   }

   if (hwndFrame != NULLHANDLE)
   {

      /*********************************************************/
      /* set window position                                   */
      /*********************************************************/

      WinSetWindowPos(hwndFrame,
                      NULLHANDLE,
                      10,
                      10,
                      lWidth/10*8,
                      lHeight/10*8,
                      SWP_SIZE|SWP_MOVE|SWP_ACTIVATE|SWP_SHOW);

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
      }                                /* endwhile             */
      WinDestroyWindow(hwndFrame);
   }                                   /* endif                */
   WinDestroyMsgQueue(hmqQueue);
   WinTerminate(habAnchor);
   return 0;
}

MRESULT EXPENTRY ClientWndProc(HWND hwndClient,ULONG ulMsg,MPARAM
                                mpParm1,MPARAM mpParm2)
{
   PWNDDATA         pwdData;

   switch (ulMsg)
   {

      case  WM_CREATE :

         /******************************************************/
         /* allocate window word                               */
         /******************************************************/

         pwdData = (PWNDDATA)malloc(sizeof(WNDDATA));
         if (pwdData == NULL)
         {
            DisplayError("Unable to allocate window word");
            return  MRFROMSHORT(TRUE);
         }                             /* endif                */

         WinSetWindowPtr(hwndClient,
                         QWL_USER,
                         pwdData);
         pwdData->sColor = -1;
         break;

      case  WM_COMMAND :
         pwdData = WinQueryWindowPtr(hwndClient,
                                     QWL_USER);
         switch (SHORT1FROMMP(mpParm1))
         {
            case  IDM_DISPLAY :

               /************************************************/
               /* load up dialog, if dialog already exists,    */
               /* just make it visible                         */
               /************************************************/

               if (!pwdData->hwndDlg)
                  pwdData->hwndDlg = WinLoadDlg(HWND_DESKTOP,
                                                hwndClient,
                                                DlgProc,
                                                NULLHANDLE,
                                                IDD_VALUE,
                                                NULL);
               else
                  WinSetWindowPos(pwdData->hwndDlg,
                                  HWND_TOP,
                                  0,
                                  0,
                                  0,
                                  0,
                                  SWP_SHOW|SWP_ACTIVATE);
               break;
            case  IDM_EXIT :

               /************************************************/
               /* close up window                              */
               /************************************************/

               WinPostMsg(hwndClient,
                          WM_CLOSE,
                          MPVOID,
                          MPVOID);
               break;
            default  :
               return  WinDefWindowProc(hwndClient,
                                        ulMsg,
                                        mpParm1,
                                        mpParm2);
         }
         break;

      case  WM_DESTROY :

         /******************************************************/
         /* free up memory                                     */
         /******************************************************/

         pwdData = WinQueryWindowPtr(hwndClient,
                                     QWL_USER);
         if (pwdData)
            free(pwdData);
         break;

      case  USRM_UPDATE :

         /******************************************************/
         /* user message indicates end-user selected new color */
         /* in value set, window needs to repaint itself with  */
         /* new color                                          */
         /******************************************************/

         pwdData = WinQueryWindowPtr(hwndClient,
                                     QWL_USER);
         if (!pwdData)
         {
            DisplayError("WinQueryWindowPtr failed");
            break;
         }
         pwdData->sColor = SHORT1FROMMP(mpParm1);
         WinInvalidateRect(hwndClient,
                           NULL,
                           FALSE);
         WinUpdateWindow(hwndClient);
         break;

      case  WM_PAINT :
         {
            HPS              hpsPaint;
            RECTL            rclPaint;
            SHORT            sColor;

            /***************************************************/
            /* variable to indicate whether to paint or not    */
            /***************************************************/

            BOOL             bPaint = FALSE;

            pwdData = WinQueryWindowPtr(hwndClient,
                                        QWL_USER);

            /***************************************************/
            /* paint the entire client with the dropped color  */
            /***************************************************/

            hpsPaint = WinBeginPaint(hwndClient,
                                     NULLHANDLE,
                                     &rclPaint);
            GpiErase(hpsPaint);

            /***************************************************/
            /* do some error checking                          */
            /***************************************************/

            if (pwdData)
            {
               if (pwdData->sColor >= 0)
               {
                  bPaint = TRUE;
                  sColor = pwdData->sColor;
               }
            }
            if (bPaint)
               WinFillRect(hpsPaint,
                           &rclPaint,
                           alColors[sColor]);
            WinEndPaint(hpsPaint);
         }
         break;

      default  :
         return  WinDefWindowProc(hwndClient,
                                  ulMsg,
                                  mpParm1,
                                  mpParm2);

   }                                   /* endswitch            */
   return  MRFROMSHORT(FALSE);
}

MRESULT EXPENTRY DlgProc(HWND hwndDlg,ULONG ulMsg,MPARAM mpParm1,
                          MPARAM mpParm2)
{

   switch (ulMsg)
   {
      case  WM_INITDLG :
         {
            SHORT            sColor;
            USHORT           usX;
            USHORT           usY;
            MRESULT          mrReply;

            sColor = 0;

            /***************************************************/
            /* loop through the rows and columns to initialize */
            /* items                                           */
            /***************************************************/

            for (usX = 1; usX <= 3; usX++)
            {
               for (usY = 1; usY <= 4; usY++)
               {

                  mrReply = WinSendDlgItemMsg(hwndDlg,
                                              IDV_VALUE,
                                              VM_SETITEM,
                                              MPFROM2SHORT(usX,
                                                           usY),
                                              MPFROMLONG(alColors
                                                 [sColor++]));
                  if (!LONGFROMMR(mrReply))
                     DisplayError("WinSendDlgItemMsg failed");

               }                       /* endfor               */
            }                          /* endfor               */
         }                             /* end WM_INITDLG       */
         break;

      case  WM_CONTROL :
         if (SHORT1FROMMP(mpParm1) == IDV_VALUE)
         {
            switch (SHORT2FROMMP(mpParm1))
            {
               case  VN_SELECT :
                  {
                     ProcessSelect(hwndDlg,
                                   mpParm2);
                     break;
                  }
               default  :
                  return  WinDefDlgProc(hwndDlg,
                                        ulMsg,
                                        mpParm1,
                                        mpParm2);

            }                          /* endswitch            */
         }                             /* end if == IDV_VALUE  */

         break;                        /* end WM_CONTROL       */
      case  WM_COMMAND :
         switch (SHORT1FROMMP(mpParm1))
         {
            case  DID_CANCEL :
               WinDismissDlg(hwndDlg,
                             FALSE);
               break;

            default  :
               return  WinDefDlgProc(hwndDlg,
                                     ulMsg,
                                     mpParm1,
                                     mpParm2);
         }                             /* endswitch mpParm1    */

         break;                        /* end WM_COMMAND       */
      default  :
         return  WinDefDlgProc(hwndDlg,
                               ulMsg,
                               mpParm1,
                               mpParm2);

   }                                   /* endswitch            */
   return  MRFROMSHORT(FALSE);

}

VOID DisplayError(CHAR *pszText)
{

   /************************************************************/
   /* small function to display error string                   */
   /************************************************************/

   WinAlarm(HWND_DESKTOP,
            WA_ERROR);
   WinMessageBox(HWND_DESKTOP,
                 HWND_DESKTOP,
                 pszText,
                 "Error!",
                 0,
                 MB_OK|MB_ERROR);

   return ;

}

VOID ProcessSelect(HWND hwndDlg,MPARAM mpParm2)
{

   /************************************************************/
   /* small function to get color that was selected            */
   /************************************************************/

   HWND             hwndClient,hwndFrame;
   USHORT           usRow,usCol;
   SHORT            sColorIndex;
   BOOL             bSuccess;

   /************************************************************/
   /* get row and column of selected item                      */
   /************************************************************/

   usRow = SHORT1FROMMP(mpParm2);
   usCol = SHORT2FROMMP(mpParm2);

   /************************************************************/
   /* calculate index into color array                         */
   /************************************************************/

   sColorIndex = ((usRow-1)*4)+(usCol-1);
   if (sColorIndex < 0)
   {
      DisplayError("Invalid selected item");
      return ;
   }

   /************************************************************/
   /* get the client window handle to post message             */
   /************************************************************/

   hwndFrame = WinWindowFromID(HWND_DESKTOP,
                               ID_FRAME);
   if (!hwndFrame)
   {
      DisplayError("WinWindowFromID:1 failed");
      return ;
   }
   hwndClient = WinWindowFromID(hwndFrame,
                                FID_CLIENT);
   if (!hwndClient)
   {
      DisplayError("WinWindowFromID:2 failed");
      return ;
   }

   bSuccess = WinPostMsg(hwndClient,
                         USRM_UPDATE,
                         MPFROMSHORT(sColorIndex),
                         MPVOID);

   if (!bSuccess)
      DisplayError("WinPostMsg failed");

   return ;

}
