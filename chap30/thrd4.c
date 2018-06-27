#define INCL_DOSPROCESS
#define INCL_WININPUT
#define INCL_WINMENUS
#define INCL_WINPOINTERS
#define INCL_WINSYS
#define INCL_WINWINDOWMGR
#include <os2.h>
#include <stdlib.h>
#include "thrd4rc.h"

#define CLS_MAIN                 "Thread4Class"
#define CLS_OBJECT               "Thread4ObjectClass"

#define MYM_BASE                 (WM_USER)
#define MYM_STARTREQUEST         (MYM_BASE)
#define MYM_ENDREQUEST           (MYM_BASE+1)

#define ASYNC_NOTE               0
#define ASYNC_WARNING            1
#define ASYNC_ERROR              2

typedef VOID (* _Optlink PFNREQ)(PVOID);

typedef struct _INSTDATA {
   ULONG ulSzStruct;
   HWND hwndMenu;
   HWND hwndObject;
} INSTDATA, *PINSTDATA;

MRESULT EXPENTRY objectProc(HWND hwndWnd,
                            ULONG ulMsg,
                            MPARAM mpParm1,
                            MPARAM mpParm2)
{
   switch (ulMsg) {
   case MYM_STARTREQUEST:
      {
         ULONG ulRequest;
         HWND hwndOwner;
         ULONG ulNote;

         //--------------------------------------------
         // Get the specifics for this request
         //--------------------------------------------
         ulRequest=LONGFROMMP(mpParm1);
         hwndOwner=HWNDFROMMP(mpParm2);

         switch (ulRequest) {
         case ASYNC_NOTE:
            ulNote=WA_NOTE;
            break;
         case ASYNC_WARNING:
            ulNote=WA_WARNING;
            break;
         case ASYNC_ERROR:
         default:
            ulNote=WA_ERROR;
            break;
         } /* endswitch */

         //--------------------------------------------
         // Perform a dummy action
         //--------------------------------------------
         WinAlarm(HWND_DESKTOP,ulNote);
         DosSleep(2000);
         WinAlarm(HWND_DESKTOP,ulNote);

         //--------------------------------------------
         // Notify the owner that we're done
         //--------------------------------------------
         WinPostMsg(hwndOwner,
                    MYM_ENDREQUEST,
                    MPFROMLONG(ulRequest),
                    0);
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

VOID _Optlink objectThread(PINSTDATA pidData)
{
   HAB habAnchor;
   HMQ hmqQueue;
   BOOL bLoop;
   QMSG qmMsg;

   habAnchor=WinInitialize(0);
   hmqQueue=WinCreateMsgQueue(habAnchor,0);

   WinRegisterClass(habAnchor,
                    CLS_OBJECT,
                    objectProc,
                    0,
                    0);

   //--------------------------------------------------
   // Create the object window
   //--------------------------------------------------
   pidData->hwndObject=WinCreateWindow(HWND_OBJECT,
                                       CLS_OBJECT,
                                       "",
                                       0,
                                       0,
                                       0,
                                       0,
                                       0,
                                       HWND_OBJECT,
                                       HWND_TOP,
                                       WND_OBJECT,
                                       NULL,
                                       NULL);
   if (pidData->hwndObject!=NULLHANDLE) {
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

      WinDestroyWindow(pidData->hwndObject);
   } /* endif */

   WinDestroyMsgQueue(hmqQueue);
   WinTerminate(habAnchor);
}

VOID disableMenuItems(HWND hwndMenu,BOOL bDisable)
//-----------------------------------------------------
// This function disables or enables the "send
// request" menuitems.
//-----------------------------------------------------
{
   SHORT sDisable;

   sDisable=bDisable?MIA_DISABLED:0;

   WinSendMsg(hwndMenu,
              MM_SETITEMATTR,
              MPFROM2SHORT(MI_NOTETHREAD,TRUE),
              MPFROM2SHORT(MIA_DISABLED,sDisable));
   WinSendMsg(hwndMenu,
              MM_SETITEMATTR,
              MPFROM2SHORT(MI_WARNINGTHREAD,TRUE),
              MPFROM2SHORT(MIA_DISABLED,sDisable));
   WinSendMsg(hwndMenu,
              MM_SETITEMATTR,
              MPFROM2SHORT(MI_ERRORTHREAD,TRUE),
              MPFROM2SHORT(MIA_DISABLED,sDisable));
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
      {
         TID tidThread;

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
         pidData->hwndObject=NULLHANDLE;

         //--------------------------------------------
         // Start the object thread
         //--------------------------------------------
         tidThread=_beginthread((PFNREQ)objectThread,
                                NULL,
                                0x4000,
                                pidData);
         if (tidThread==(TID)-1) {
            WinDestroyWindow(pidData->hwndMenu);
            free(pidData);
            WinAlarm(HWND_DESKTOP,WA_ERROR);
            return MRFROMSHORT(TRUE);
         } /* endif */
      }
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
      case MI_NOTETHREAD:
         //--------------------------------------------
         // Disable the menu items and send the
         // request to the object window
         //--------------------------------------------
         disableMenuItems(pidData->hwndMenu,TRUE);

         WinPostMsg(pidData->hwndObject,
                    MYM_STARTREQUEST,
                    MPFROMLONG(ASYNC_NOTE),
                    MPFROMHWND(hwndWnd));
         break;
      case MI_WARNINGTHREAD:
         disableMenuItems(pidData->hwndMenu,TRUE);

         WinPostMsg(pidData->hwndObject,
                    MYM_STARTREQUEST,
                    MPFROMLONG(ASYNC_WARNING),
                    MPFROMHWND(hwndWnd));
         break;
      case MI_ERRORTHREAD:
         disableMenuItems(pidData->hwndMenu,TRUE);

         WinPostMsg(pidData->hwndObject,
                    MYM_STARTREQUEST,
                    MPFROMLONG(ASYNC_ERROR),
                    MPFROMHWND(hwndWnd));
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
   case MYM_ENDREQUEST:
      //-----------------------------------------------
      // The object window has finished our request
      // so enable the menuitems
      //-----------------------------------------------
      disableMenuItems(pidData->hwndMenu,FALSE);
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
                                "Object Window",
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
