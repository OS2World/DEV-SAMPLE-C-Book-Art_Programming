#define  INCL_WIN
#include <os2.h>
#include "popup.h"
#include "stdlib.h"
MRESULT EXPENTRY ClientWndProc(HWND hwnd,ULONG msg,MPARAM mp1,
                                MPARAM mp2);

#define  CLS_CLIENT    "MyClass"
typedef          struct
{
   HWND             hwndMenu;

   HPOINTER         hptrFileIcon;
} MENUDATA,*PMENUDATA;

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

   /************************************************************/
   /* create frame window                                      */
   /************************************************************/

   ulFlags = FCF_TASKLIST|FCF_TITLEBAR|FCF_SYSMENU|FCF_MINMAX|
      FCF_SIZEBORDER|FCF_SHELLPOSITION;

   hwndFrame = WinCreateStdWindow(HWND_DESKTOP, 
                                  0, 
                                  &ulFlags, 
                                  CLS_CLIENT, 
                                  "Popup Menu Example", 
                                  0, 
                                  NULLHANDLE, 
                                  0, 
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
                      lWidth/8, 
                      lHeight/8, 
                      lWidth/8*6, 
                      lHeight/8*6, 
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
   PMENUDATA        pmdMenuData;

   switch (ulMsg)
   {

      case  WM_CREATE :
         {
            pmdMenuData = malloc(sizeof(MENUDATA));
            WinSetWindowPtr(hwndClient, 
                            0, 
                            pmdMenuData);

            pmdMenuData->hwndMenu = WinLoadMenu(hwndClient, 
                                                NULLHANDLE, 
                                                IDM_POPUP);
            pmdMenuData->hptrFileIcon = WinLoadFileIcon
               ("POPUP.EXE", 
                FALSE);
         } 
         break;

      case  WM_DESTROY :

         pmdMenuData = WinQueryWindowPtr(hwndClient, 
                                         0);
         if (pmdMenuData)
         {
            if (pmdMenuData->hptrFileIcon != NULLHANDLE)
            {
               WinFreeFileIcon(pmdMenuData->hptrFileIcon);
            }                          /* endif                */
            if (pmdMenuData->hwndMenu)
               WinDestroyWindow(pmdMenuData->hwndMenu);
            free(pmdMenuData);
         }                             /* endif                */
         break;

      case  WM_PAINT :
         {
            HPS              hpsPaint;
            RECTL            rclInvalid;

            hpsPaint = WinBeginPaint(hwndClient, 
                                     NULLHANDLE, 
                                     &rclInvalid);
            WinFillRect(hpsPaint, 
                        &rclInvalid, 
                        SYSCLR_WINDOW);
            pmdMenuData = WinQueryWindowPtr(hwndClient, 
                                            0);

            if (pmdMenuData->hptrFileIcon != NULLHANDLE)
            {
               WinDrawPointer(hpsPaint, 
                              50, 
                              50, 
                              pmdMenuData->hptrFileIcon, 
                              DP_NORMAL);
            }                          /* endif                */
            WinEndPaint(hpsPaint);
         } 
         break;
      case  WM_CONTEXTMENU :
         {
            POINTL           ptlMouse;
            RECTL            rclIcon;
            HAB              habAnchor;
            BOOL             bInside;
            BOOL             bKeyboardUsed;

            pmdMenuData = WinQueryWindowPtr(hwndClient, 
                                            0);
            habAnchor = WinQueryAnchorBlock(hwndClient);
            bKeyboardUsed = SHORT2FROMMP(mpParm2);

            /*-------------------------------------------------*/
            /* If the mouse was used, check to see if the      */
            /* pointer is over the icon, else always display   */
            /* the menu.                                       */
            /*-------------------------------------------------*/

            if (!bKeyboardUsed)
            {

               rclIcon.xLeft = 50;
               rclIcon.xRight = rclIcon.xLeft+WinQuerySysValue
                  (HWND_DESKTOP, 
                   SV_CXICON);

               rclIcon.yBottom = 50;
               rclIcon.yTop = rclIcon.yBottom+WinQuerySysValue
                  (HWND_DESKTOP, 
                   SV_CYICON);

               ptlMouse.x = (LONG)SHORT1FROMMP(mpParm1);
               ptlMouse.y = (LONG)SHORT2FROMMP(mpParm1);

               bInside = WinPtInRect(habAnchor, 
                                     &rclIcon, 
                                     &ptlMouse);
            } 
            else
            {

               bInside = TRUE;
               ptlMouse.x = 100;
               ptlMouse.y = 100;

            }                          /* endif                */
            if ((bInside) && (pmdMenuData->hwndMenu))
            {
               WinPopupMenu(hwndClient, 
                            hwndClient, 
                            pmdMenuData->hwndMenu, 
                            ptlMouse.x, 
                            ptlMouse.y, 
                            IDM_ICON, 
                            PU_POSITIONONITEM|PU_KEYBOARD|
                               PU_MOUSEBUTTON1|PU_MOUSEBUTTON2);
            }                          /* endif                */
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
