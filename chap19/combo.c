#define INCL_WINDIALOGS
#define INCL_WINENTRYFIELDS
#define INCL_WINLISTBOXES
#include <os2.h>
#include <stdio.h>
#include "comborc.h"

MRESULT EXPENTRY comboDlgProc(HWND hwndWnd,
                              ULONG ulMsg,
                              MPARAM mpParm1,
                              MPARAM mpParm2)
{
   switch (ulMsg) {
   case WM_INITDLG:
      {
         USHORT usIndex;
         CHAR achText[64];

         for (usIndex=1; usIndex<=10; usIndex++) {
            sprintf(achText,"Item %d", usIndex);

            WinInsertLboxItem(WinWindowFromID(hwndWnd,
                                              DCB_CB_COMBO1),
                              LIT_END,
                              achText);
            WinInsertLboxItem(WinWindowFromID(hwndWnd,
                                              DCB_CB_COMBO2),
                              LIT_END,
                              achText);
            WinInsertLboxItem(WinWindowFromID(hwndWnd,
                                              DCB_CB_COMBO3),
                              LIT_END,
                              achText);
         } /* endfor */
      }
      break;
   default:
      return WinDefDlgProc(hwndWnd,ulMsg,mpParm1,mpParm2);
   } /* endswitch */

   return MRFROMSHORT(FALSE);
}

INT main(VOID)
{
   HAB habAnchor;
   HMQ hmqQueue;

   habAnchor=WinInitialize(0);
   hmqQueue=WinCreateMsgQueue(habAnchor,0);

   WinDlgBox(HWND_DESKTOP,
             HWND_DESKTOP,
             comboDlgProc,
             NULLHANDLE,
             DLG_COMBOBOXES,
             NULL);

   WinDestroyMsgQueue(hmqQueue);
   WinTerminate(habAnchor);
   return 0;
}
