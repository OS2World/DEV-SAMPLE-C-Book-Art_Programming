#define  INCL_WIN
#include <os2.h>
#include <stdio.h>
#include <string.h>
#include "menu.h"
#define  CLS_CLIENT    "MyClass"
VOID displayMenuInfo(HWND hwndMenu,USHORT usMenuItem,HWND 
                      hwndClient);

MRESULT EXPENTRY ClientWndProc(HWND hwndWnd,ULONG ulMsg,MPARAM 
                                mpParm1,MPARAM mpParm2);

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
                    0);

   ulFlags = FCF_STANDARD&~FCF_SHELLPOSITION;

   /************************************************************/
   /* create frame window                                      */
   /************************************************************/

   hwndFrame = WinCreateStdWindow(HWND_DESKTOP, 
                                  0, 
                                  &ulFlags, 
                                  CLS_CLIENT, 
                                  "Menu Example", 
                                  0, 
                                  NULLHANDLE, 
                                  RES_FRAME, 
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

   switch (ulMsg)
   {
      case  WM_CREATE :
         {
            HPS              hpsWnd;
            HBITMAP          hbmBitmap;
            MENUITEM         miItem;
            HWND             hwndMenu;

            hpsWnd = WinGetPS(hwndClient);

            /***************************************************/
            /* load bitmap from resource file                  */
            /***************************************************/

            hbmBitmap = GpiLoadBitmap(hpsWnd, 
                                      NULLHANDLE, 
                                      IDB_BITMAP, 
                                      32, 
                                      32);
            WinReleasePS(hpsWnd);

            /***************************************************/
            /* set up MENUITEM data structure                  */
            /***************************************************/

            miItem.iPosition = 0;
            miItem.afStyle = MIS_BITMAP;
            miItem.afAttribute = 0;
            miItem.id = IDM_BITMAP;
            miItem.hwndSubMenu = NULLHANDLE;
            miItem.hItem = hbmBitmap;

            hwndMenu = WinWindowFromID(WinQueryWindow(hwndClient,
                                                      QW_PARENT),
                                       FID_MENU);

            /***************************************************/
            /* Set MENUITEM                                    */
            /***************************************************/

            WinSendMsg(hwndMenu, 
                       MM_SETITEM, 
                       MPFROM2SHORT(0, 
                                    TRUE), 
                       MPFROMP(&miItem));
         } 
         break;
      case  WM_PAINT :
         {
            HPS              hpsPaint;
            RECTL            rclPaint;

            hpsPaint = WinBeginPaint(hwndClient, 
                                     NULLHANDLE, 
                                     &rclPaint);

            /***************************************************/
            /* clear window                                    */
            /***************************************************/

            WinFillRect(hpsPaint, 
                        &rclPaint, 
                        SYSCLR_WINDOW);
            WinEndPaint(hpsPaint);
         } 
         break;
      case  WM_COMMAND :
         switch (SHORT1FROMMP(mpParm1))
         {
            case  IDM_ITEM1 :
            case  IDM_ITEM2 :
            case  IDM_ITEM3 :
            case  IDM_BITMAP :
            case  IDM_CUT :
            case  IDM_COPY :
               {
                  HWND             hwndFrame;
                  HWND             hwndMenu;
                  USHORT           usAttr;
                  MRESULT          mrReply;
                  CHAR             achText[64];

                  /*********************************************/
                  /* get the menu window handle                */
                  /*********************************************/

                  hwndFrame = WinQueryWindow(hwndClient, 
                                             QW_PARENT);
                  hwndMenu = WinWindowFromID(hwndFrame, 
                                             FID_MENU);

                  /*********************************************/
                  /* show menuitem information                 */
                  /*********************************************/

                  displayMenuInfo(hwndMenu, 
                                  SHORT1FROMMP(mpParm1), 
                                  hwndClient);

                  /*********************************************/
                  /* if this is the checked menu item, toggle  */
                  /* it                                        */
                  /*********************************************/

                  if (SHORT1FROMMP(mpParm1) == IDM_ITEM1)
                  {
                     mrReply = WinSendMsg(hwndMenu, 
                                          MM_QUERYITEMATTR, 
                                          MPFROM2SHORT(IDM_ITEM1,
                                                       TRUE), 
                                          MPFROMSHORT(MIA_CHECKED
                                          ));

                     usAttr = SHORT1FROMMR(mrReply);

                     /******************************************/
                     /* toggle checked bit                     */
                     /******************************************/

                     usAttr ^= MIA_CHECKED;

                     if (usAttr != 0)
                     {
                        strcpy(achText, 
                               " ~Checked item\tAlt + C");
                     } 
                     else
                     {
                        strcpy(achText, 
                               " ~Unchecked item\tAlt + C");
                     }                 /* endif                */

                     /******************************************/
                     /* change to newly toggled state          */
                     /******************************************/

                     WinSendMsg(hwndMenu, 
                                MM_SETITEMATTR, 
                                MPFROM2SHORT(IDM_ITEM1, 
                                             TRUE), 
                                MPFROM2SHORT(MIA_CHECKED, 
                                             usAttr));

                     /******************************************/
                     /* change text to reflect change of state */
                     /******************************************/

                     WinSendMsg(hwndMenu, 
                                MM_SETITEMTEXT, 
                                MPFROMSHORT(IDM_ITEM1), 
                                MPFROMP(achText));
                  }                    /* endif                */
               } 
               break;
            default  :
               return  WinDefWindowProc(hwndClient, 
                                        ulMsg, 
                                        mpParm1, 
                                        mpParm2);

         }                             /* endswitch            */
         break;
      default  :
         return  WinDefWindowProc(hwndClient, 
                                  ulMsg, 
                                  mpParm1, 
                                  mpParm2);

   }                                   /* endswitch            */
   return  MRFROMSHORT(FALSE);
} 

VOID displayMenuInfo(HWND hwndMenu,USHORT usMenuItem,HWND 
                      hwndClient)
{
   USHORT           usAllStyles;
   USHORT           usAttr;
   USHORT           usSzText;
   CHAR             achItemText[32];
   CHAR             achText[128];

   /************************************************************/
   /* look for all different kinds of attributes               */
   /************************************************************/

   usAllStyles = MIA_NODISMISS|MIA_FRAMED|MIA_CHECKED|
      MIA_DISABLED|MIA_HILITED;

   usAttr = SHORT1FROMMR(WinSendMsg(hwndMenu, 
                                    MM_QUERYITEMATTR, 
                                    MPFROM2SHORT(usMenuItem, 
                                                 TRUE), 
                                    MPFROMSHORT(usAllStyles)));

   /************************************************************/
   /* query menu item text                                     */
   /************************************************************/

   usSzText = SHORT1FROMMR(WinSendMsg(hwndMenu, 
                                      MM_QUERYITEMTEXT, 
                                      MPFROM2SHORT(usMenuItem, 
                                                   30), 
                                      MPFROMP(achItemText)));

   sprintf(achText, 
           "Menu Item: \"%s\"\nMenu Item Styles are: 0x%04x", 
           usSzText?achItemText:" (null) ", 
           usAttr);

   /************************************************************/
   /* display information                                      */
   /************************************************************/

   WinMessageBox(HWND_DESKTOP, 
                 hwndClient, 
                 achText, 
                 "Menu Information", 
                 0, 
                 MB_OK);

   return ;
} 
