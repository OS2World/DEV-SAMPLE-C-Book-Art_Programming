#define  INCL_WIN
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include "button.h"
#define  CLS_CLIENT    "MyClass"
MRESULT EXPENTRY ClientWndProc(HWND hwndWnd,ULONG ulMsg,MPARAM 
                                mpParm1,MPARAM mpParm2);

MRESULT EXPENTRY DlgProc(HWND hwndWnd,ULONG ulMsg,MPARAM mpParm1,
                          MPARAM mpParm2);

INT main(VOID)
{
   HAB              habAnchor;
   HMQ              hmqQueue;
   ULONG            ulFlags;
   HWND             hwndFrame;
   HWND             hwndClient;
   BOOL             bLoop;
   QMSG             qmMsg;

   habAnchor = WinInitialize(0);
   hmqQueue = WinCreateMsgQueue(habAnchor, 
                                0);

   WinRegisterClass(habAnchor, 
                    CLS_CLIENT, 
                    ClientWndProc, 
                    CS_SIZEREDRAW, 
                    0);

   ulFlags = FCF_TITLEBAR|FCF_SYSMENU|FCF_MENU|FCF_SIZEBORDER;

   hwndFrame = WinCreateStdWindow(HWND_DESKTOP, 
                                  0, 
                                  &ulFlags, 
                                  CLS_CLIENT, 
                                  "Button Control Example", 
                                  0, 
                                  NULLHANDLE, 
                                  IDR_CLIENT, 
                                  &hwndClient);

   if (hwndFrame != NULLHANDLE)
   {

      WinSetWindowPos(hwndFrame, 
                      NULLHANDLE, 
                      50, 
                      50, 
                      550, 
                      380, 
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

MRESULT EXPENTRY ClientWndProc(HWND hwndWnd,ULONG ulMsg,MPARAM 
                                mpParm1,MPARAM mpParm2)
{
   switch (ulMsg)
   {

      case  WM_ERASEBACKGROUND :
         return  MRFROMSHORT(TRUE);

      case  WM_COMMAND :

         switch (SHORT1FROMMP(mpParm1))
         {

            case  IDM_START :

               /************************************************/
               /* load and run the dialog box                  */
               /************************************************/

               WinDlgBox(HWND_DESKTOP, 
                         hwndWnd, 
                         DlgProc, 
                         NULLHANDLE, 
                         IDD_BUTTON, 
                         NULL);
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
         } 
         break;
      default  :
         return  WinDefWindowProc(hwndWnd, 
                                  ulMsg, 
                                  mpParm1, 

                                  mpParm2);
   }                                   /* end switch ulMsg     */
   return  MRFROMSHORT(FALSE);
} 

MRESULT EXPENTRY DlgProc(HWND hwndWnd,ULONG ulMsg,MPARAM mpParm1,
                          MPARAM mpParm2)
{
   switch (ulMsg)
   {

      case  WM_INITDLG :
         {
            BTNCDATA         bcdData;
            POINTL           ptl;

            bcdData.cb = sizeof(BTNCDATA);
            bcdData.fsCheckState = 0;
            bcdData.fsHiliteState = 0;

            /***************************************************/
            /* load the information pointer                    */
            /***************************************************/

            bcdData.hImage = WinQuerySysPointer(HWND_DESKTOP, 
                                             SPTR_ICONINFORMATION
                                                   , 
                                                FALSE);

            /***************************************************/
            /* thse are the coordinates we want in dialog units*/
            /***************************************************/

            ptl.x = 175;
            ptl.y = 25;

            /***************************************************/
            /* map out to correct window coordinates           */
            /***************************************************/

            WinMapDlgPoints(hwndWnd, 
                            &ptl, 
                            1, 
                            TRUE);

            /***************************************************/
            /* create an icon button                           */
            /***************************************************/

            WinCreateWindow(hwndWnd, 
                            WC_BUTTON, 
                            "", 
                            WS_VISIBLE|WS_TABSTOP|BS_ICON, 
                            ptl.x, 
                            ptl.y, 
                            WinQuerySysValue(HWND_DESKTOP, 
                                             SV_CXICON), 
                            WinQuerySysValue(HWND_DESKTOP, 
                                             SV_CYICON), 
                            hwndWnd, 
                            HWND_TOP, 
                            IDR_ICON, 
                            (PVOID)&bcdData, 
                            NULL);

            /***************************************************/
            /* check some of the buttons                       */
            /***************************************************/

            WinSendDlgItemMsg(hwndWnd, 
                              IDC_AUTOCHECKBOX, 
                              BM_SETCHECK, 
                              MPFROMSHORT(TRUE), 
                              NULL);

            WinSendDlgItemMsg(hwndWnd, 
                              IDR_AUTORADIOBUTTON, 
                              BM_SETCHECK, 
                              MPFROMSHORT(TRUE), 
                              NULL);

            WinSendDlgItemMsg(hwndWnd, 
                              IDC_AUTO3STATE, 
                              BM_SETCHECK, 
                              MPFROMSHORT(2), 
                              NULL);
         } 
         break;
      case  WM_COMMAND :
         switch (SHORT1FROMMP(mpParm1))
         {
            case  DID_OK :
               WinDismissDlg(hwndWnd, 
                             LONGFROMMP(mpParm1));
               break;

               /************************************************/
               /* for the next two items, do not dismiss       */
               /* dialog, instead return FALSE to keep dialog  */
               /* displaying                                   */
               /************************************************/

            case  IDP_NOBORDER :
            case  IDR_ICON :
               break;
            default  :
               return  WinDefDlgProc(hwndWnd, 
                                     ulMsg, 
                                     mpParm1, 
                                     mpParm2);
         }                             /* end switch           */
         break;
      default  :
         return  WinDefDlgProc(hwndWnd, 
                               ulMsg, 
                               mpParm1, 

                               mpParm2);
   }                                   /* endswitch            */
   return  MRFROMSHORT(FALSE);
} 
