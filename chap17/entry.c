#define  INCL_WINDIALOGS
#define  INCL_WINENTRYFIELDS
#include <os2.h>
#include "entryrc.h"
MRESULT EXPENTRY entryDlgProc(HWND hwndWnd,ULONG ulMsg,MPARAM 
                               mpParm1,MPARAM mpParm2)

{
   switch (ulMsg)
   {
      case  WM_INITDLG :
         {
            WinSetDlgItemText(hwndWnd, 
                              DEF_ENTRY1, 
                              "Left");
            WinSetDlgItemText(hwndWnd, 
                              DEF_ENTRY2, 
                              "Center");
            WinSetDlgItemText(hwndWnd, 
                              DEF_ENTRY3, 
                              "Right");
            WinSetDlgItemText(hwndWnd, 
                              DEF_ENTRY4, 
                              "Read only");
            WinSetDlgItemText(hwndWnd, 
                              DEF_ENTRY5, 
                              "Unreadable");
         } 
         break;
      default  :
         return  WinDefDlgProc(hwndWnd, 
                               ulMsg, 
                               mpParm1, 
                               mpParm2);
   }                                   /* endswitch            */
   return  MRFROMSHORT(FALSE);
} 

INT main(VOID)
{
   HAB              habAnchor;
   HMQ              hmqQueue;

   habAnchor = WinInitialize(0);
   hmqQueue = WinCreateMsgQueue(habAnchor, 
                                0);

   WinDlgBox(HWND_DESKTOP, 
             HWND_DESKTOP, 
             entryDlgProc, 
             NULLHANDLE, 
             DLG_ENTRYFIELDS, 
             NULL);

   WinDestroyMsgQueue(hmqQueue);
   WinTerminate(habAnchor);
   return 0;
} 
