#define  INCL_WINHELP
#define  INCL_WINHOOKS
#define  INCL_WINSYS
#include <os2.h>
#include "help2.h"
#define  CLS_CLIENT    "SampleClass"
HWND hwndHelp;

MRESULT EXPENTRY clientWndProc(HWND hwndClient,ULONG ulMsg,MPARAM
                                mpParm1,MPARAM mpParm2);

BOOL EXPENTRY helpHook(HAB habAnchor,SHORT sMode,USHORT usTopic,
                        USHORT usSubTopic,PRECTL prclPos);

INT main(VOID)
{
   HAB              habAnchor;
   HMQ              hmqQueue;
   ULONG            ulFlags;
   HWND             hwndFrame;
   HELPINIT         hiInit;
   BOOL             bLoop;
   QMSG             qmMsg;

   habAnchor = WinInitialize(0);
   hmqQueue = WinCreateMsgQueue(habAnchor,
                                0);

   WinRegisterClass(habAnchor,
                    CLS_CLIENT,
                    clientWndProc,
                    0,
                    0);

   ulFlags = FCF_SIZEBORDER|FCF_TITLEBAR|FCF_TASKLIST|
      FCF_SHELLPOSITION|FCF_SYSMENU|FCF_MENU;

   hwndFrame = WinCreateStdWindow(HWND_DESKTOP,
                                  WS_VISIBLE,
                                  &ulFlags,
                                  CLS_CLIENT,
                        "Help Manager Sample Application 2",

                                  0,
                                  NULLHANDLE,
                                  RES_CLIENT,
                                  NULL);

   hiInit.cb = sizeof(HELPINIT);
   hiInit.ulReturnCode = 0;
   hiInit.pszTutorialName = NULL;
   hiInit.phtHelpTable = (PHELPTABLE)MAKEULONG(HELP_CLIENT,
                                               0xFFFF);
   hiInit.hmodHelpTableModule = NULLHANDLE;
   hiInit.hmodAccelActionBarModule = NULLHANDLE;
   hiInit.idAccelTable = 0;
   hiInit.idActionBar = 0;
   hiInit.pszHelpWindowTitle = "Help Manager Sample Help File";
   hiInit.fShowPanelId = CMIC_HIDE_PANEL_ID;
   hiInit.pszHelpLibraryName = "HELP2.HLP";

   hwndHelp = WinCreateHelpInstance(habAnchor,
                                    &hiInit);

   if ((hwndHelp != NULLHANDLE) && (hiInit.ulReturnCode != 0))
   {
      WinDestroyHelpInstance(hwndHelp);
      hwndHelp = NULLHANDLE;
   }
   else
      if ((hwndHelp != NULLHANDLE))
      {
         WinAssociateHelpInstance(hwndHelp,
                                  hwndFrame);
      }                                /* endif                */
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
   }                                   /* endwhile             */
   if (hwndHelp != NULLHANDLE)
   {
      WinAssociateHelpInstance(NULLHANDLE,
                               hwndFrame);
      WinDestroyHelpInstance(hwndHelp);
      hwndHelp = NULLHANDLE;
   }                                   /* endif                */
   WinDestroyWindow(hwndFrame);
   WinDestroyMsgQueue(hmqQueue);
   WinTerminate(habAnchor);
   return ;
}

MRESULT EXPENTRY clientWndProc(HWND hwndClient,ULONG ulMsg,MPARAM
                                mpParm1,MPARAM mpParm2)
{
   switch (ulMsg)
   {

      case  WM_PAINT :
         {
            HPS              hpsPaint;
            RECTL            rclPaint;

            hpsPaint = WinBeginPaint(hwndClient,
                                     NULLHANDLE,
                                     &rclPaint);
            WinFillRect(hpsPaint,
                        &rclPaint,
                        SYSCLR_WINDOW);
            WinEndPaint(hpsPaint);
         }
         break;
      case  WM_COMMAND :
         switch (SHORT1FROMMP(mpParm1))
         {
            case  MI_HELPINDEX :
               WinSendMsg(hwndHelp,
                          HM_HELP_INDEX,
                          0,
                          0);
               break;
            case  MI_GENERALHELP :
               WinSendMsg(hwndHelp,
                          HM_EXT_HELP,
                          0,
                          0);
               break;
            case  MI_HELPFORHELP :
               WinSendMsg(hwndHelp,
                          HM_DISPLAY_HELP,
                          0,
                          0);
               break;
            case  MI_KEYSHELP :
               WinSendMsg(hwndHelp,
                          HM_KEYS_HELP,
                          0,
                          0);
               break;
            case  MI_PRODINFO :
               WinMessageBox(HWND_DESKTOP,
                             hwndClient,
                       "Copyright 1995 by Larry Salomon,Jr.",

                             "Help Sample",
                             HLP_MESSAGEBOX,
                             MB_OK|MB_HELP|MB_INFORMATION);
               break;
            default  :
               return  WinDefWindowProc(hwndClient,
                                        ulMsg,
                                        mpParm1,
                                        mpParm2);
         }                             /* endswitch            */
         break;

      case  HM_QUERY_KEYS_HELP :
         return  MRFROMSHORT(KEYSHELP_CLIENT);
      default  :
         return  WinDefWindowProc(hwndClient,
                                  ulMsg,
                                  mpParm1,
                                  mpParm2);
   }                                   /* endswitch            */
   return  MRFROMSHORT(FALSE);
}
