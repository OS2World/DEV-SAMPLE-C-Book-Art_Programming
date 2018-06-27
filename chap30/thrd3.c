#define INCL_DOSPROCESS
#define INCL_WININPUT
#define INCL_WINMENUS
#define INCL_WINPOINTERS
#define INCL_WINSYS
#define INCL_WINWINDOWMGR
#include <os2.h>
#include <stdlib.h>
#include "thrd3rc.h"

#define CLS_MAIN                 "Thread3Class"

#define DT_NOERROR               0
#define DT_QUITRECEIVED          1
#define DT_ERROR                 2

typedef VOID (* _Optlink PFNREQ)(PVOID);

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
} INSTDATA, *PINSTDATA;

USHORT dispatchThread(HAB habAnchor,
                      PFNREQ pfnThread,
                      PTHREADINFO ptiInfo);
VOID _Optlink _testThread(PTESTTHREADINFO pttiInfo);
BOOL testThread(HWND hwndWnd,
                PTESTTHREADINFO pttiInfo);

USHORT dispatchThread(HAB habAnchor,
                      PFNREQ pfnThread,
                      PTHREADINFO ptiInfo)
//----------------------------------------------------
// This is the thread dispatch procedure.  It calls
// _beginthread() and goes into a
// WinPeekMsg()/WinDispatchMsg() loop until the
// thread is finished or WM_QUIT is received.  Note
// the semantics of the latter event:  if WM_QUIT is
// received, then it is assumed that the application
// will kill itself on return and thus any system
// resources will automatically be unallocated by the
// system when the application ends.  So we do not
// set bKillThread=TRUE and wait but instead call
// DosKillThread() and return.
//----------------------------------------------------
{
   TID tidThread;
   BOOL bLoop;
   QMSG qmMsg;

   ptiInfo->bKillThread=FALSE;
   ptiInfo->bThreadDead=FALSE;

   tidThread=_beginthread(pfnThread,
                          NULL,
                          0x4000,
                          ptiInfo);
   if (tidThread==(TID)-1) {
      return DT_ERROR;
   } /* endif */

   //-------------------------------------------------
   // WinGetMsg() cannot be used because it blocks if
   // there is no message waiting.  When the thread
   // dies, therefore, the function will never return
   // if the user takes his/her hands off of the
   // keyboard, mouse, and no timers are started
   // because we will never get a message!
   //-------------------------------------------------
   WinPeekMsg(habAnchor,
              &qmMsg,
              NULLHANDLE,
              0,
              0,
              PM_REMOVE);
   bLoop=((qmMsg.msg!=WM_QUIT) &&
            (!ptiInfo->bThreadDead));

   while (bLoop) {
      WinDispatchMsg(habAnchor,&qmMsg);
      WinPeekMsg(habAnchor,
                 &qmMsg,
                 NULLHANDLE,
                 0,
                 0,
                 PM_REMOVE);
      bLoop=((qmMsg.msg!=WM_QUIT) &&
               (!ptiInfo->bThreadDead));
   } /* endwhile */

   if (qmMsg.msg==WM_QUIT) {
      DosKillThread(tidThread);
      return DT_QUITRECEIVED;
   } /* endif */

   return DT_NOERROR;
}

VOID _Optlink _testThread(PTESTTHREADINFO pttiInfo)
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

   WinDestroyMsgQueue(hmqQueue);
   WinTerminate(habAnchor);

   DosEnterCritSec();
   pttiInfo->tiInfo.bThreadDead=TRUE;
   return;
}

BOOL testThread(HWND hwndWnd,PTESTTHREADINFO pttiInfo)
//----------------------------------------------------
// This function simply initializes any thread-
// specific fields and calls dispatchThread().
//----------------------------------------------------
{
   pttiInfo->tiInfo.ulSzStruct=sizeof(TESTTHREADINFO);

   return dispatchThread(WinQueryAnchorBlock(hwndWnd),
                         (PFNREQ)_testThread,
                         (PTHREADINFO)pttiInfo);
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
            // Call testThread() directly
            //----------------------------------------
            if (testThread(hwndWnd,&ttiTest)==
                  DT_QUITRECEIVED) {
               WinPostMsg(hwndWnd,WM_CLOSE,0,0);
            } /* endif */
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
                                "Synchronous Call",
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
