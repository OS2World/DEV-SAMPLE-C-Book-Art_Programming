#define INCL_DOSPROCESS
#define INCL_WINFRAMEMGR
#define INCL_WININPUT
#define INCL_WINMENUS
#define INCL_WINPOINTERS
#define INCL_WINSYS
#define INCL_WINWINDOWMGR
#include <os2.h>
#include <stdlib.h>
#include <string.h>
#include "thrd2rc.h"

#define CLS_MAIN                 "Thread2Class"

#define ASYNC_TEST               0x00000001L

typedef VOID (* _Optlink PFNREQ)(PVOID);

#define MYM_BASE                 (WM_USER)
#define MYM_STARTTHREAD          (MYM_BASE)
#define MYM_ENDTHREAD            (MYM_BASE+1)

typedef struct _THREADINFO {
   //-------------------------------------------------
   // Initialized by the main thread
   //-------------------------------------------------
   ULONG ulSzStruct;
   HWND hwndOwner;
   BOOL bKillThread;
   //-------------------------------------------------
   // Initialized by the secondary thread
   //-------------------------------------------------
   HAB habThread;
   BOOL bThreadDead;
   BOOL bResult;
} THREADINFO, *PTHREADINFO;

typedef struct _TESTTHREADINFO {
   THREADINFO tiInfo;
} TESTTHREADINFO, *PTESTTHREADINFO;

typedef struct _INSTDATA {
   ULONG ulSzStruct;
   HWND hwndMenu;
   ULONG ulAsync;
} INSTDATA, *PINSTDATA;

VOID _Optlink testThread(PTESTTHREADINFO pttiInfo)
//----------------------------------------------------
// This is a do nothing thread that beeps, sleeps,
// and beeps again
//----------------------------------------------------
{
   HAB habAnchor;
   HMQ hmqQueue;

   habAnchor=WinInitialize(0);
   hmqQueue=WinCreateMsgQueue(habAnchor,0);

   pttiInfo->tiInfo.habThread=habAnchor;
   pttiInfo->tiInfo.bThreadDead=FALSE;
   pttiInfo->tiInfo.bResult=FALSE;

   WinAlarm(HWND_DESKTOP,WA_NOTE);
   DosSleep(2000);
   WinAlarm(HWND_DESKTOP,WA_NOTE);

   WinPostMsg(pttiInfo->tiInfo.hwndOwner,
              MYM_ENDTHREAD,
              MPFROMLONG(ASYNC_TEST),
              MPFROMP(pttiInfo));

   WinDestroyMsgQueue(hmqQueue);
   WinTerminate(habAnchor);

   DosEnterCritSec();
   pttiInfo->tiInfo.bThreadDead=TRUE;
   return;
}

MRESULT EXPENTRY wndProc(HWND hwndWnd,
                         ULONG ulMsg,
                         MPARAM mpParm1,
                         MPARAM mpParm2)
{
   PINSTDATA pidData;

   pidData=(PINSTDATA)WinQueryWindowPtr(hwndWnd,0);

   switch (ulMsg) {
   case WM_CREATE:
      pidData=(PINSTDATA)malloc(sizeof(INSTDATA));
      if (pidData==NULL) {
         WinAlarm(HWND_DESKTOP,WA_ERROR);
         return MRFROMSHORT(TRUE);
      } /* endif */

      WinSetWindowPtr(hwndWnd,0,(PVOID)pidData);

      pidData->ulSzStruct=sizeof(INSTDATA);
      pidData->hwndMenu=WinLoadMenu(HWND_OBJECT,
                                    NULLHANDLE,
                                    RES_CLIENT);

      pidData->ulAsync=0;
      break;
   case WM_DESTROY:
      WinDestroyWindow(pidData->hwndMenu);
      free(pidData);
      break;
   case WM_CONTEXTMENU:
      {
         POINTL ptlMouse;

         WinQueryPointerPos(HWND_DESKTOP,&ptlMouse);
         WinPopupMenu(HWND_DESKTOP,
                      hwndWnd,
                      pidData->hwndMenu,
                      ptlMouse.x,
                      ptlMouse.y,
                      0,
                      PU_HCONSTRAIN | PU_VCONSTRAIN |
                         PU_NONE | PU_KEYBOARD |
                         PU_MOUSEBUTTON1 |
                         PU_MOUSEBUTTON2);
      }
      break;
   case WM_COMMAND:
      switch (SHORT1FROMMP(mpParm1)) {
      case MI_THREAD:
         {
            TESTTHREADINFO ttiTest;

            //----------------------------------------
            // Request a thread
            //----------------------------------------
            WinSendMsg(hwndWnd,
                       MYM_STARTTHREAD,
                       MPFROMLONG(ASYNC_TEST),
                       MPFROMP(&ttiTest));
         }
         break;
      case MI_EXIT:
         WinPostMsg(hwndWnd,WM_CLOSE,0,0);
         break;
      default:
         return WinDefWindowProc(hwndWnd,
                                 ulMsg,
                                 mpParm1,
                                 mpParm2);
      } /* endswitch */
      break;
   case WM_PAINT:
      {
         HPS hpsWnd;
         RECTL rclPaint;

         hpsWnd=WinBeginPaint(hwndWnd,
                              NULLHANDLE,
                              &rclPaint);
         WinFillRect(hpsWnd,&rclPaint,SYSCLR_WINDOW);
         WinEndPaint(hpsWnd);
      }
      break;
   case WM_INITMENU:
      switch (SHORT1FROMMP(mpParm1)) {
      case FID_MENU:
         if ((pidData->ulAsync & ASYNC_TEST)!=0) {
            WinEnableMenuItem(HWNDFROMMP(mpParm2),
                              MI_THREAD,
                              FALSE);
         } else {
            WinEnableMenuItem(HWNDFROMMP(mpParm2),
                              MI_THREAD,
                              TRUE);
         } /* endif */
         break;
      default:
         return WinDefWindowProc(hwndWnd,
                                 ulMsg,
                                 mpParm1,
                                 mpParm2);
      } /* endswitch */
      break;
   case WM_MOUSEMOVE:
      {
         HPOINTER hpPointer;

         if (pidData->ulAsync>0) {
            hpPointer=
               WinQuerySysPointer(HWND_DESKTOP,
                                  SPTR_WAIT,
                                  FALSE);
            WinSetPointer(HWND_DESKTOP,hpPointer);
            return MRFROMSHORT(TRUE);
         } else {
            return WinDefWindowProc(hwndWnd,
                                    ulMsg,
                                    mpParm1,
                                    mpParm2);
         } /* endif */
      }
   case WM_CONTROLPOINTER:
      {
         HPOINTER hpPointer;

         if (pidData->ulAsync>0) {
            hpPointer=
               WinQuerySysPointer(HWND_DESKTOP,
                                  SPTR_WAIT,
                                  FALSE);
            return MRFROMLONG(hpPointer);
         } else {
            return WinDefWindowProc(hwndWnd,
                                    ulMsg,
                                    mpParm1,
                                    mpParm2);
         } /* endif */
      }
   case MYM_STARTTHREAD:
      {
         ULONG ulBit;
         PTHREADINFO ptiInput;
         PFNREQ pfnThread;
         PVOID pvParm;

         ulBit=LONGFROMMR(mpParm1);
         ptiInput=(PTHREADINFO)PVOIDFROMMP(mpParm2);

         ptiInput->hwndOwner=hwndWnd;
         ptiInput->bKillThread=FALSE;

         switch (ulBit) {
         case ASYNC_TEST:
            {
               PTESTTHREADINFO pttiInfo;

               ptiInput->ulSzStruct=
                  sizeof(TESTTHREADINFO);

               pttiInfo=
                  (PTESTTHREADINFO)malloc(
                     sizeof(TESTTHREADINFO));
               if (pttiInfo==NULL) {
                  WinMessageBox(HWND_DESKTOP,
                                hwndWnd,
                                "There is not "
                                   "enough memory.",
                                "Error",
                                0,
                                MB_OK |
                                   MB_ICONEXCLAMATION|
                                   MB_MOVEABLE);
                  return MRFROMSHORT(FALSE);
               } /* endif */

               memcpy(pttiInfo,
                      ptiInput,
                      sizeof(TESTTHREADINFO));
               pfnThread=(PFNREQ)testThread;
               pvParm=(PVOID)pttiInfo;
            }
            break;
         default:
            WinMessageBox(HWND_DESKTOP,
                          hwndWnd,
                          "There is an internal "
                             "error.",
                          "Error",
                          0,
                          MB_OK |
                             MB_ICONEXCLAMATION |
                             MB_MOVEABLE);
            return MRFROMSHORT(FALSE);
         } /* endswitch */

         if (_beginthread(pfnThread,
                          NULL,
                          0x4000,
                          pvParm)==-1) {
            free(pvParm);
            WinMessageBox(HWND_DESKTOP,
                          hwndWnd,
                          "The thread could not be "
                             "created.",
                          "Error",
                          0,
                          MB_OK |
                             MB_ICONEXCLAMATION |
                             MB_MOVEABLE);
            return MRFROMSHORT(FALSE);
         } /* endif */

         pidData->ulAsync|=ulBit;
      }
      break;
   case MYM_ENDTHREAD:
      {
         ULONG ulBit;
         PTHREADINFO ptiInput;

         ulBit=LONGFROMMR(mpParm1);
         ptiInput=(PTHREADINFO)PVOIDFROMMP(mpParm2);

         //-------------------------------------------
         // Wait for the thread to finish dying.
         // There is a bug in DosSleep() such that if
         // 0 is the argument, nothing happens.  Call
         // it with 1 instead to achieve the same
         // result.
         //-------------------------------------------
         while (!ptiInput->bThreadDead) {
            DosSleep(1);
         } /* endwhile */

         switch (ulBit) {
         case ASYNC_TEST:
            {
               PTESTTHREADINFO pttiInfo;

               pttiInfo=(PTESTTHREADINFO)ptiInput;
               free(pttiInfo);
            }
            break;
         default:
            return MRFROMSHORT(FALSE);
         } /* endswitch */

         pidData->ulAsync&=~ulBit;
      }
      break;
   default:
      return WinDefWindowProc(hwndWnd,
                              ulMsg,
                              mpParm1,
                              mpParm2);
   } /* endswitch */

   return MRFROMSHORT(FALSE);
}

INT main(USHORT usArgs,PCHAR apchArgs[])
{
   HAB habAnchor;
   HMQ hmqQueue;
   ULONG ulCreate;
   HWND hwndFrame;
   HWND hwndClient;
   BOOL bLoop;
   QMSG qmMsg;

   habAnchor=WinInitialize(0);
   hmqQueue=WinCreateMsgQueue(habAnchor,0);

   WinRegisterClass(habAnchor,
                    CLS_MAIN,
                    wndProc,
                    CS_SIZEREDRAW,
                    sizeof(PVOID));

   ulCreate=FCF_SYSMENU | FCF_TITLEBAR | FCF_MINMAX |
               FCF_SIZEBORDER | FCF_TASKLIST |
               FCF_SHELLPOSITION;

   if (WinQuerySysValue(HWND_DESKTOP,SV_ANIMATION)) {
      ulCreate|=WS_ANIMATE;
   } /* endif */

   hwndFrame=WinCreateStdWindow(HWND_DESKTOP,
                                WS_VISIBLE,
                                &ulCreate,
                                CLS_MAIN,
                                "Asynchronous Call "
                                   "With Feedback",
                                0L,
                                NULLHANDLE,
                                RES_CLIENT,
                                &hwndClient);
   if (hwndFrame!=NULLHANDLE) {
      bLoop=WinGetMsg(habAnchor,
                      &qmMsg,
                      NULLHANDLE,
                      0,
                      0);
      while (bLoop) {
         WinDispatchMsg(habAnchor,&qmMsg);
         bLoop=WinGetMsg(habAnchor,
                         &qmMsg,
                         NULLHANDLE,
                         0,
                         0);
      } /* endwhile */

      WinDestroyWindow(hwndFrame);
   } /* endif */

   WinDestroyMsgQueue(hmqQueue);
   WinTerminate(habAnchor);
   return 0;
}
