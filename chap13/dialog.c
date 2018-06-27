#define  INCL_WIN
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dialog.h"
#define  CLS_CLIENT    "MyClass"
MRESULT EXPENTRY ClientWndProc(HWND hwndWnd,ULONG ulMsg,MPARAM 
                                mpParm1,MPARAM mpParm2);

MRESULT EXPENTRY DlgProc(HWND hwndWnd,ULONG ulMsg,MPARAM mpParm1,
                          MPARAM mpParm2);

typedef struct   _DLGINFO
{
  ULONG            ulStructSize;

  BOOL             bModal;
  HWND             hwndModeless;
  HWND             hwndClient;

} DLGINFO,*PDLGINFO;

VOID DisplayError(CHAR *pString);

INT main(VOID)
{
  HAB              habAnchor;
  HMQ              hmqQueue;
  ULONG            ulFlags;
  HWND             hwndFrame;
  HWND             hwndClient;
  QMSG             qmMsg;
  BOOL             bLoop;
  LONG             lHeight;
  LONG             lWidth;

  habAnchor = WinInitialize(0);
  hmqQueue = WinCreateMsgQueue(habAnchor, 
                               0);

  /*************************************************************/
  /* register class with space for window word                 */
  /*************************************************************/

  WinRegisterClass(habAnchor, 
                   CLS_CLIENT, 
                   ClientWndProc, 
                   CS_SIZEREDRAW, 
                   sizeof(PVOID));

  ulFlags = FCF_STANDARD&~FCF_SHELLPOSITION&~FCF_ACCELTABLE&~
     FCF_TASKLIST;

  /*************************************************************/
  /* create the frame, client, etc.                            */
  /*************************************************************/

  hwndFrame = WinCreateStdWindow(HWND_DESKTOP, 
                                 0, 
                                 &ulFlags, 
                                 CLS_CLIENT, 
                                 "Dialog Box Example", 
                                 0, 
                                 NULLHANDLE, 
                                 IDR_CLIENT, 
                                 &hwndClient);

  /*************************************************************/
  /* find desktop dimensions                                   */
  /*************************************************************/

  lHeight = WinQuerySysValue(HWND_DESKTOP, 
                             SV_CYSCREEN);
  lWidth = WinQuerySysValue(HWND_DESKTOP, 
                            SV_CXSCREEN);

  /*************************************************************/
  /* center frame window on desktop                            */
  /*************************************************************/

  if (hwndFrame != 0)
  {
    WinSetWindowPos(hwndFrame, 
                    NULLHANDLE, 
                    lWidth/8, 
                    lHeight/8, 
                    lWidth/8*6, 
                    lHeight/8*6, 
                    SWP_SIZE|SWP_MOVE|SWP_ACTIVATE|SWP_SHOW);

    /***********************************************************/
    /* message loop processing                                 */
    /***********************************************************/

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
    }                                  /* endwhile             */
    WinDestroyWindow(hwndFrame);
  }                                    /* endif                */
  WinDestroyMsgQueue(hmqQueue);
  WinTerminate(habAnchor);
  return 0;
} 

MRESULT EXPENTRY DlgProc(HWND hwndDlg,ULONG ulMsg,MPARAM mpParm1,
                          MPARAM mpParm2)
{
  switch (ulMsg)
  {

    case  WM_INITDLG :
      {
        BOOL             bModal;
        CHAR             achMessage[64];
        PDLGINFO         pDlgInfo;
        RECTL            rclClient;
        POINTL           ptPoints;
        LONG             lHeight,lWidth;

        /*******************************************************/
        /* dialog create information is stored in mpParm2      */
        /*******************************************************/

        pDlgInfo = PVOIDFROMMP(mpParm2);

        if (!pDlgInfo)
        {
          DisplayError("No create information");
          return  MRFROMSHORT(TRUE);
        } 

        /*******************************************************/
        /* the bModal flags says whether we're modal or        */
        /* modeless                                            */
        /*******************************************************/

        bModal = pDlgInfo->bModal;

        /*******************************************************/
        /* do the sizing calculations, so dialogs look right   */
        /*******************************************************/

        WinQueryWindowRect(pDlgInfo->hwndClient, 
                           &rclClient);

        lHeight = rclClient.yTop-rclClient.yBottom;
        lWidth = rclClient.xRight-rclClient.xLeft;

        /*******************************************************/
        /* since parent of dialogs are desktop, they will be   */
        /* positioned relative to the desktop, but we want them*/
        /* centered on the client start at 1/8th of the client */
        /*******************************************************/

        ptPoints.x = lWidth/8;

        /*******************************************************/
        /* start at 1/19th of the height of the client and     */
        /* 10/19th for the modeless dialog                     */
        /*******************************************************/

        ptPoints.y = bModal?lHeight/19:lHeight/19*10;

        /*******************************************************/
        /* find out where these points are relative to the     */
        /* desktop                                             */
        /*******************************************************/

        WinMapWindowPoints(pDlgInfo->hwndClient, 
                           HWND_DESKTOP, 
                           &ptPoints, 
                           1);

        WinSetWindowPos(hwndDlg, 
                        NULLHANDLE, 
                        ptPoints.x, 
                        ptPoints.y, 
                        lWidth/8*6, 
                        lHeight/19*8, 
                        SWP_MOVE|SWP_SIZE);

        sprintf(achMessage, "I'm a %s dialog box", (bModal?
           ("modal"):("modeless")));

        WinSetDlgItemText(hwndDlg, 
                          IDT_DIALOGNAME, 
                          achMessage);

        if (bModal)
        {
          strcpy(achMessage, "Try and click on the main window");
        } 
        else
        {
          strcpy(achMessage, "Click on the main window");
        }                              /* endif                */
        WinSetDlgItemText(hwndDlg, 
                          IDT_CLICK, 
                          achMessage);
      } 
      break;
    case  WM_COMMAND :

      switch (SHORT1FROMMP(mpParm1))
      {
        case  DID_OK :
        case  DID_CANCEL :

          /*****************************************************/
          /* for the modeless dialog, this function hides the  */
          /* dialog for the modal dialog, this function        */
          /* destroys the dialog                               */
          /*****************************************************/

          WinDismissDlg(hwndDlg, 
                        FALSE);
          break;

        default  :
          return  WinDefDlgProc(hwndDlg, 
                                ulMsg, 
                                mpParm1, 

                                mpParm2);
      }                                /* endswitch            */
      break;

    default  :
      return  WinDefDlgProc(hwndDlg, 
                            ulMsg, 
                            mpParm1, 
                            mpParm2);
  }                                    /* endswitch            */
  return  MRFROMSHORT(FALSE);
} 

MRESULT EXPENTRY ClientWndProc(HWND hwndWnd,ULONG ulMsg,MPARAM 
                                mpParm1,MPARAM mpParm2)
{
  switch (ulMsg)
  {

    case  WM_ERASEBACKGROUND :
      return  MRFROMSHORT(TRUE);

    case  WM_CREATE :
      {
        PDLGINFO         pDlgInfo;

        pDlgInfo = calloc(1, sizeof(DLGINFO));
        if (!pDlgInfo)
        {

          /*****************************************************/
          /* if error, display message and halt creation       */
          /*****************************************************/

          DisplayError("Cannot allocate memory");
          return  MRFROMSHORT(TRUE);

        } 

        pDlgInfo->ulStructSize = sizeof(DLGINFO);
        pDlgInfo->hwndClient = hwndWnd;

        /*******************************************************/
        /* associate pDlgInfo with window word                 */
        /*******************************************************/

        WinSetWindowPtr(hwndWnd, 
                        QWL_USER, 
                        pDlgInfo);
        break;
      }                                /* end WM_CREATE        */
    case  WM_DESTROY :
      {
        PDLGINFO         pDlgInfo;

        pDlgInfo = WinQueryWindowPtr(hwndWnd, 
                                     QWL_USER);

        /*******************************************************/
        /* clean up                                            */
        /*******************************************************/

        if (pDlgInfo)
        {

          /*****************************************************/
          /* if the modelesss dialog box is still around,      */
          /* destroy it                                        */
          /*****************************************************/

          if (pDlgInfo->hwndModeless)
            WinDestroyWindow(pDlgInfo->hwndModeless);
          free(pDlgInfo);
        } 
        break;
      } 

    case  WM_COMMAND :

      switch (SHORT1FROMMP(mpParm1))
      {
        case  IDM_MODELESS :
          {
            PDLGINFO         pDlgInfo;

            pDlgInfo = WinQueryWindowPtr(hwndWnd, 
                                         QWL_USER);
            if (!pDlgInfo)
            {
              DisplayError("Missing Window Word");
              break;
            } 

            pDlgInfo->bModal = FALSE;

            /***************************************************/
            /* if the dialog has not been created, call the    */
            /* loader, else, just show the dialog window       */
            /***************************************************/

            if (!pDlgInfo->hwndModeless)
              pDlgInfo->hwndModeless = WinLoadDlg(HWND_DESKTOP, 
                                                  hwndWnd, 
                                                  DlgProc, 
                                                  NULLHANDLE, 
                                                  IDD_DIALOG, 
                                                  pDlgInfo);
            else
              WinSetWindowPos(pDlgInfo->hwndModeless, 
                              HWND_TOP, 
                              0, 
                              0, 
                              0, 
                              0, 
                              SWP_SHOW|SWP_ACTIVATE);
          } 
          break;

        case  IDM_MODAL :
          {
            PDLGINFO         pDlgInfo;

            pDlgInfo = WinQueryWindowPtr(hwndWnd, 
                                         QWL_USER);
            if (!pDlgInfo)
            {
              DisplayError("Missing Window Word");
              break;
            } 

            pDlgInfo->bModal = TRUE;

            WinDlgBox(HWND_DESKTOP, 
                      hwndWnd, 
                      DlgProc, 
                      NULLHANDLE, 
                      IDD_DIALOG, 
                      pDlgInfo);
          } 
          break;

        case  IDM_EXIT :
          WinPostMsg(hwndWnd, 
                     WM_CLOSE, 
                     0, 
                     0);
          break;

        default  :
          return  WinDefWindowProc(hwndWnd, 
                                   ulMsg, 
                                   mpParm1, 

                                   mpParm2);
      }                                /* endswitch            */
      break;
    default  :
      return  WinDefWindowProc(hwndWnd, 
                               ulMsg, 
                               mpParm1, 

                               mpParm2);
  }                                    /* endswitch            */
  return  MRFROMSHORT(FALSE);
} 

/* small function to beep and display error message            */

VOID DisplayError(CHAR *pString)
{

  WinAlarm(HWND_DESKTOP, 
           WA_ERROR);
  WinMessageBox(HWND_DESKTOP, 
                HWND_DESKTOP, 
                pString, 
                "Error", 
                0, 
                MB_ICONEXCLAMATION|MB_OK);

  return ;

} 
