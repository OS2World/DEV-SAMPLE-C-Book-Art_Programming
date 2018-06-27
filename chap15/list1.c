#define  INCL_WIN
#define  INCL_GPI
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list1.h"


#define  CLS_CLIENT    "MyClass"
#define  NUM_ENTRIES   10

MRESULT EXPENTRY ClientWndProc(HWND hwndClient,ULONG ulMsg,MPARAM
                                mpParm1,MPARAM mpParm2);
MRESULT EXPENTRY DlgProc(HWND hwndDlg,ULONG ulMsg,MPARAM mpParm1,
                          MPARAM mpParm2);
VOID DisplayError(CHAR *pszText);
int main(void);

typedef          struct
{
   USHORT           ausListBoxSel[NUM_ENTRIES];
} LISTBOXINFO,*PLISTBOXINFO;

int main()
{
   HMQ              hmqQueue;
   HAB              habAnchor;
   ULONG            ulFlags;
   HWND             hwndFrame;
   HWND             hwndClient;
   QMSG             qmMsg;

   /************************************************************/
   /* initialization stuff                                     */
   /************************************************************/

   habAnchor = WinInitialize(0);
   hmqQueue = WinCreateMsgQueue(habAnchor,
                                0);

   WinRegisterClass(habAnchor,
                    CLS_CLIENT,
                    ClientWndProc,
                    CS_SYNCPAINT|CS_SIZEREDRAW|CS_CLIPCHILDREN,
                    sizeof(PVOID));

   ulFlags = FCF_TITLEBAR|FCF_SYSMENU|FCF_MENU|FCF_SIZEBORDER|
      FCF_MINMAX|FCF_SHELLPOSITION;

   /************************************************************/
   /* create the frame and client windows                      */
   /************************************************************/

   hwndFrame = WinCreateStdWindow(HWND_DESKTOP,
                                  WS_VISIBLE,
                                  &ulFlags,
                                  CLS_CLIENT,
                                  "Plain Listbox Example",
                                  0,
                                  (HMODULE)0,
                                  ID_CLIENT,
                                  &hwndClient);

   if (hwndFrame != NULLHANDLE)
   {
      while (WinGetMsg(habAnchor,
                       &qmMsg,
                       NULLHANDLE,
                       0,
                       0))
         WinDispatchMsg(habAnchor,
                        &qmMsg);
      WinDestroyWindow(hwndFrame);
   }                                   /* endif                */

   /************************************************************/
   /* clean - up                                               */
   /************************************************************/

   WinDestroyMsgQueue(hmqQueue);
   WinTerminate(habAnchor);
   return 0;
}

MRESULT EXPENTRY ClientWndProc(HWND hwndClient,ULONG ulMsg,MPARAM
                                mpParm1,MPARAM mpParm2)

{
   PLISTBOXINFO     pliInfo;
   BOOL             bReturn;

   /************************************************************/
   /* retrieve listbox info from the client window             */
   /************************************************************/

   pliInfo = WinQueryWindowPtr(hwndClient,
                               0);

   switch (ulMsg)
   {
      case  WM_CREATE :

         /******************************************************/
         /* allocate window word                               */
         /******************************************************/

         pliInfo = (PLISTBOXINFO)calloc(1,
                                        sizeof(LISTBOXINFO));
         if (!pliInfo)
         {
            DisplayError( "Memory Allocation Failure:1 ") ;
            return (MRFROMSHORT(TRUE));
         }                             /* endif                */

         /******************************************************/
         /* assign window word                                 */
         /******************************************************/

         bReturn = WinSetWindowPtr(hwndClient,
                         QWL_USER,
                         pliInfo);
         if (!bReturn)
            DisplayError( "WinSetWindowPtr Failure:1") ;

         break;
      case  WM_DESTROY :

         /******************************************************/
         /* free up window word                                */
         /******************************************************/

         if (pliInfo)
            free(pliInfo);
         break;
      case  WM_PAINT :
         {
            HPS              hpsPresentationSpace;
            RECTL            rectInvalidRect,rclPaintRegion,
               rclNewPaint;
            USHORT           i;

            hpsPresentationSpace = WinBeginPaint(hwndClient,
                                                 NULLHANDLE,
                                                 &rectInvalidRect
               );

            /***************************************************/
            /* get window size                                 */
            /***************************************************/

            bReturn = WinQueryWindowRect(hwndClient,
                               &rclPaintRegion);
            if (!bReturn){
                DisplayError("WinQueryWindowRect Failure:1");
                break;
            }

            /***************************************************/
            /* start 3/4 of the way across window              */
            /***************************************************/

            rclNewPaint.xLeft = (rclPaintRegion.xRight-
               rclPaintRegion.xLeft)  / 4 * 3;
            rclNewPaint.xRight = rclPaintRegion.xRight;

            /***************************************************/
            /* set the top and bottom coordinates              */
            /***************************************************/

            rclNewPaint.yBottom = rclPaintRegion.yBottom;
            rclNewPaint.yTop = rclPaintRegion.yTop;

            /***************************************************/
            /* fill the invalidated rectangle with white       */
            /***************************************************/

            WinFillRect(hpsPresentationSpace,
                        &rectInvalidRect,
                        CLR_WHITE);

            WinDrawText(hpsPresentationSpace,
                        -1,
                        "You have selected:",
                        &rclNewPaint,
                        0,
                        0,
                        DT_LEFT|DT_TEXTATTRS);

            /***************************************************/
            /* drop down one line                              */
            /***************************************************/

            rclNewPaint.yTop -= 15;

            /***************************************************/
            /* if item is selected, drop down one line, and    */
            /* draw the selected listbox item                  */
            /***************************************************/

            for (i = 0; i < NUM_ENTRIES; i++)
               if (pliInfo->ausListBoxSel[i] == TRUE)
               {

                  rclNewPaint.yTop -= 15;
                  WinDrawText(hpsPresentationSpace,
                              -1,
                              pszListBoxEntry[i],
                              &rclNewPaint,
                              0,
                              0,
                              DT_LEFT|DT_TEXTATTRS);

               }                       /* end if selected      */
            WinEndPaint(hpsPresentationSpace);
            break;

         }
      case  WM_COMMAND :
         switch (SHORT1FROMMP(mpParm1))
         {
            case  IDM_DISPLAY :
               {
               ULONG ulReturn ;
               /************************************************/
               /* load up dialog                               */
               /************************************************/

               ulReturn = WinDlgBox(HWND_DESKTOP,
                         hwndClient,
                         DlgProc,
                         NULLHANDLE,
                         IDD_LISTBOX,
                         NULL);
               if (ulReturn == DID_ERROR)
                    DisplayError("WinDlgBox Failure:1") ;
               break;
               }
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

      case  WM_SIZE :
         WinPostMsg(hwndClient,
                    UM_LISTBOXSEL,
                    MPVOID,
                    MPVOID);
         break;
      case  UM_LISTBOXSEL :
         {

            SHORT            sSelect = 0;
            SHORT            sIndex = LIT_FIRST;
            HWND             hwndDlg;
            USHORT           i;

            /***************************************************/
            /* first set all to unselected                     */
            /***************************************************/

            for (i = 0; i < NUM_ENTRIES; i++)
               pliInfo->ausListBoxSel[i] = FALSE;

            hwndDlg = WinWindowFromID(HWND_DESKTOP,
                                      IDD_LISTBOX);

            /***************************************************/
            /* get selected items from listbox                 */
            /***************************************************/

            while (sSelect != LIT_NONE && hwndDlg)
            {

               sSelect = (SHORT)WinSendDlgItemMsg(hwndDlg,
                                                  IDL_LISTBOX,
                                                LM_QUERYSELECTION
                                                     ,
                                                  MPFROMSHORT
                                                  (sIndex),
                                                  MPVOID);

               pliInfo->ausListBoxSel[sSelect] = TRUE;

               /************************************************/
               /* set query to start at last selected item     */
               /************************************************/

               sIndex = sSelect;
            }

            /***************************************************/
            /* invalidate the window                           */
            /***************************************************/

            WinInvalidateRect(hwndClient,
                              NULL,
                              FALSE);
            break;
         }
      default  :
         return  WinDefWindowProc(hwndClient,
                                  ulMsg,
                                  mpParm1,
                                  mpParm2);
   }                                   /* endswitch            */

   return (MRFROMSHORT(FALSE));
}

MRESULT EXPENTRY DlgProc(HWND hwndDlg,ULONG ulMsg,MPARAM mpParm1,
                          MPARAM mpParm2)

{
   USHORT           i;

   switch (ulMsg)
   {

      case  WM_INITDLG :
         {
            HWND             hwndListBox;

            hwndListBox = WinWindowFromID(hwndDlg,
                                          IDL_LISTBOX);
            for (i = 0; i < NUM_ENTRIES; i++)
               WinInsertLboxItem(hwndListBox,
                                 LIT_END,
                                 pszListBoxEntry[i]);

            WinSendDlgItemMsg(hwndDlg,
                              IDL_LISTBOX,
                              LM_SELECTITEM,
                              MPFROMSHORT(0),
                              MPFROMSHORT(TRUE));
         }
         break;

      case  WM_COMMAND :
         {
            SHORT            sCommand;
            HWND             hwndClient;

            sCommand = SHORT1FROMMP(mpParm1);
            switch (sCommand)
            {
               case  DID_OK :
                  hwndClient = WinQueryWindow(hwndDlg,
                                              QW_OWNER);
                  if (!hwndClient){
                    DisplayError("WinQueryWindow Failure:1");
                    break;
                  }
                  WinPostMsg(hwndClient,
                             UM_LISTBOXSEL,
                             MPVOID,
                             MPVOID);

                  /*********************************************/
                  /* if hit OK, don't dismiss dialog           */
                  /*********************************************/

                  return (MRFROMSHORT(TRUE));

               case  DID_CANCEL :
                  hwndClient = WinQueryWindow(hwndDlg,
                                              QW_OWNER);
                  if (!hwndClient){
                    DisplayError("WinQueryWindow Failure:1");
                    break;
                  }
                  WinPostMsg(hwndClient,
                             UM_LISTBOXSEL,
                             MPVOID,
                             MPVOID);
                  WinDismissDlg(hwndDlg,
                                DID_CANCEL );
                  break;

            }                          /* end switch sCommand  */
            break;
         }
      default  :
         return (WinDefDlgProc(hwndDlg,
                               ulMsg,
                               mpParm1,
                               mpParm2));
   }                                   /* endswitch            */
   return (MRFROMSHORT(FALSE));
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
