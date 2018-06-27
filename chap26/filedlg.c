#define  INCL_WINSTDFILE
#define  INCL_WINSYS
#define  INCL_WINWINDOWMGR
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "filedlg.h"
MRESULT EXPENTRY ClientWndProc(HWND hwndClient,ULONG ulMsg,MPARAM
                                mpParm1,MPARAM mpParm2);

BOOL FindFile(HWND hwndWnd,CHAR *pchFile);
VOID DisplayError(CHAR *pszText);

#define  CLS_CLIENT    "MyClass"
INT main(VOID)

{
   HMQ              hmqQueue;
   HAB              habAnchor;
   ULONG            ulFlags;
   HWND             hwndClient;
   BOOL             bLoop;
   QMSG             qmMsg;
   LONG             lWidth,lHeight;
   HWND             hwndFrame = NULLHANDLE;

   habAnchor = WinInitialize(0);
   hmqQueue = WinCreateMsgQueue(habAnchor, 
                                0);

   WinRegisterClass(habAnchor, 
                    CLS_CLIENT, 
                    ClientWndProc, 
                    CS_SIZEREDRAW|CS_SYNCPAINT, 
                    sizeof(PVOID));

   ulFlags = FCF_TITLEBAR|FCF_SYSMENU|FCF_SIZEBORDER|FCF_MENU|
      FCF_MINMAX;

   /************************************************************/
   /* create frame and client window                           */
   /************************************************************/

   hwndFrame = WinCreateStdWindow(HWND_DESKTOP, 
                                  0, 
                                  &ulFlags, 
                                  CLS_CLIENT, 
                                  "Font Dialog Example", 
                                  0, 
                                  NULLHANDLE, 
                                  RES_CLIENT, 
                                  &hwndClient);

   /************************************************************/
   /* get screen height and width                              */
   /************************************************************/

   lWidth = WinQuerySysValue(HWND_DESKTOP, 
                             SV_CXSCREEN);

   lHeight = WinQuerySysValue(HWND_DESKTOP, 
                              SV_CYSCREEN);

   /************************************************************/
   /* set size and position of frame window                    */
   /************************************************************/

   if (hwndFrame)
   {
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
   PCHAR            pchFile;

   switch (ulMsg)
   {

      case  WM_CREATE :
         pchFile = (PCHAR)calloc(1, 
                                 CCHMAXPATH);
         if (!pchFile)
         {
            DisplayError("No memory could be allocated");
            return  MRFROMSHORT(TRUE);
         }                             /* endif                */

         WinSetWindowPtr(hwndClient, 
                         QWL_USER, 
                         pchFile);
         break;

      case  WM_DESTROY :
         pchFile = WinQueryWindowPtr(hwndClient, 
                                     0);
         if (pchFile)
         {
            free(pchFile);
         }                             /* endif                */
         break;

      case  WM_PAINT :
         {
            HPS              hpsPaint;
            RECTL            rclInvalid;
            CHAR             achText[CCHMAXPATH];

            hpsPaint = WinBeginPaint(hwndClient, 
                                     NULLHANDLE, 
                                     &rclInvalid);

            WinFillRect(hpsPaint, 
                        &rclInvalid, 
                        SYSCLR_WINDOW);
            pchFile = WinQueryWindowPtr(hwndClient, 
                                        0);

            if (pchFile[0] != 0)
            {

               WinQueryWindowRect(hwndClient, 
                                  &rclInvalid);
               sprintf(achText, 
                       "You have selected file %s", 
                       pchFile);
               WinDrawText(hpsPaint, 
                           -1, 
                           achText, 
                           &rclInvalid, 
                           0, 
                           0, 
                           DT_CENTER|DT_VCENTER|DT_TEXTATTRS);
            }                          /* endif                */
            WinEndPaint(hpsPaint);
         } 
         break;

      case  WM_COMMAND :

         switch (SHORT1FROMMP(mpParm1))
         {

            case  IDM_OPEN :
               pchFile = WinQueryWindowPtr(hwndClient, 
                                           0);
               if (pchFile)
               {
                  FindFile(hwndClient, 
                           pchFile);
               }                       /* endif                */
               WinInvalidateRect(hwndClient, 
                                 NULL, 
                                 TRUE);
               break;

            case  IDM_EXIT :
               WinPostMsg(hwndClient, 
                          WM_QUIT, 
                          0, 
                          0);
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

BOOL FindFile(HWND hwndClient,CHAR *pchFile)
{
   FILEDLG          fdFileDlg;

   memset(&fdFileDlg, 
          0, 
          sizeof(FILEDLG));

   fdFileDlg.cbSize = sizeof(FILEDLG);
   fdFileDlg.fl = FDS_CENTER|FDS_PRELOAD_VOLINFO|FDS_OPEN_DIALOG;

   if (WinFileDlg(HWND_DESKTOP, 
                  hwndClient, 
                  &fdFileDlg) != DID_OK)
   {
      DisplayError("WinFileDlg failed");
      return  FALSE;
   }                                   /* endif                */

   strcpy(pchFile, 
          fdFileDlg.szFullFile);
   return  TRUE;
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
