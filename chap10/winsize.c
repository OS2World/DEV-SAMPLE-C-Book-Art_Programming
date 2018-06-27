#define  INCL_WIN
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define  CLS_CLIENT    "MyClass"

typedef struct _FRAMEINFO
{
   LONG             lWidth;
   LONG             lHeight;
   PFNWP            pfnNormalFrameProc;
} FRAMEINFO,*PFRAMEINFO;

MRESULT EXPENTRY ClientWndProc(HWND hwndWnd,ULONG ulMsg,MPARAM
                                mpParm1,MPARAM mpParm2);

MRESULT EXPENTRY SubclassFrameProc(HWND hwndWnd,ULONG ulMsg,
                                  MPARAM mpParm1,MPARAM mpParm2);

VOID DisplayError(CHAR *pszText);

INT main(VOID)
{

   HAB              habAnchor;
   HMQ              hmqQueue;
   ULONG            ulFlags;
   HWND             hwndFrame;
   BOOL             bLoop;
   QMSG             qmMsg;
   PFRAMEINFO       pFrameInfo;
   PFNWP            pfnNormalFrameProc;
   LONG             lHeight,lWidth;

   habAnchor = WinInitialize(0);
   hmqQueue = WinCreateMsgQueue(habAnchor,
                                0);

   WinRegisterClass(habAnchor,
                    CLS_CLIENT,
                    ClientWndProc,
                    0,
                    sizeof(PVOID));

   ulFlags = FCF_TITLEBAR|FCF_SYSMENU|FCF_SIZEBORDER|FCF_MINMAX|
      FCF_TASKLIST;

   /************************************************************/
   /* create frame window                                      */
   /************************************************************/

   hwndFrame = WinCreateStdWindow(HWND_DESKTOP,
                                  0,
                                  &ulFlags,
                                  CLS_CLIENT,
                                  "Frame Window",
                                  0,
                                  NULLHANDLE,
                                  0,
                                  NULL);

   /************************************************************/
   /* subclass the frame window and point to subclass frame    */
   /* proc                                                     */
   /************************************************************/

   pfnNormalFrameProc = WinSubclassWindow(hwndFrame,
                                          SubclassFrameProc);

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
      DisplayError("WinQuerySysValue failed");
      lWidth = 640;
      lHeight = 480;
   }

   if (hwndFrame != NULLHANDLE)
   {

      /*********************************************************/
      /* allocate memory for window word                       */
      /*********************************************************/

      pFrameInfo = calloc(1,
                          sizeof(FRAMEINFO));
      if (!pFrameInfo)
      {

         /******************************************************/
         /* error handling                                     */
         /******************************************************/

         DisplayError("No memory allocated for frame info");
         WinDestroyWindow(hwndFrame);
         WinDestroyMsgQueue(hmqQueue);
         WinTerminate(habAnchor);
         return 0;
      }
      WinSetWindowPtr(hwndFrame,
                      QWL_USER,
                      pFrameInfo);
      pFrameInfo->lWidth = lWidth;
      pFrameInfo->lHeight = lHeight;
      pFrameInfo->pfnNormalFrameProc = pfnNormalFrameProc;

      /*********************************************************/
      /* set window position                                   */
      /*********************************************************/

      WinSetWindowPos(hwndFrame,
                      NULLHANDLE,
                      lWidth/4,
                      lHeight/4,
                      lWidth/2,
                      lHeight/2,
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

MRESULT EXPENTRY ClientWndProc(HWND hwndWnd,ULONG ulMsg,MPARAM
                                mpParm1,MPARAM mpParm2)
{
   switch (ulMsg)
   {

      case  WM_ERASEBACKGROUND :
         return  MRFROMSHORT(TRUE);

      default  :
         return  WinDefWindowProc(hwndWnd,
                                  ulMsg,
                                  mpParm1,
                                  mpParm2);
   }                                   /* endswitch            */
}

MRESULT EXPENTRY SubclassFrameProc(HWND hwndFrame,ULONG ulMsg,
                                   MPARAM mpParm1,MPARAM mpParm2)
{

   PFRAMEINFO       pFrameInfo;
   PFNWP            pfnNormalFrameProc;
   CLASSINFO        classInfo;
   HAB              hab;

   /************************************************************/
   /* retrieve frame information from frame window word        */
   /************************************************************/

   pFrameInfo = WinQueryWindowPtr(hwndFrame,
                                  QWL_USER);
   if (!pFrameInfo)
   {

      /*********************************************************/
      /* if cannot find window word, use the default window    */
      /* proc                                                  */
      /*********************************************************/

      hab = WinQueryAnchorBlock(hwndFrame);
      WinQueryClassInfo(hab,
                        WC_FRAME,
                        &classInfo);
      pfnNormalFrameProc = classInfo.pfnWindowProc;
      return ((*pfnNormalFrameProc)(hwndFrame,
                                    ulMsg,
                                    mpParm1,
                                    mpParm2));
   }
   pfnNormalFrameProc = pFrameInfo->pfnNormalFrameProc;

   switch (ulMsg)
   {
      case  WM_QUERYTRACKINFO :
         {
            PTRACKINFO       pTrackInfo;
            MRESULT          mrReply;

            /***************************************************/
            /* call default procedure first to get TRACKINFO   */
            /* structure                                       */
            /***************************************************/

            mrReply = (*pfnNormalFrameProc)(hwndFrame,
                                            ulMsg,
                                            mpParm1,
                                            mpParm2);
            pTrackInfo = (PTRACKINFO)PVOIDFROMMP(mpParm2);

            /***************************************************/
            /* set limits at 1/2 the height and width of screen*/
            /***************************************************/

            pTrackInfo->ptlMinTrackSize.x = pFrameInfo->lWidth/2;
            pTrackInfo->ptlMinTrackSize.y = pFrameInfo->lHeight/2
               ;

            /***************************************************/
            /* put limits in TRACKINFO structure               */
            /***************************************************/

            return  mrReply;

         }
      case  WM_DESTROY :
         {

            /***************************************************/
            /* clean up                                        */
            /***************************************************/

            if (pFrameInfo)
               free(pFrameInfo);
            break;
         }
      default  :
         break;

   }                                   /* endswitch            */

   /************************************************************/
   /* return normal frame window procedure                     */
   /************************************************************/

   return ((*pfnNormalFrameProc)(hwndFrame,
                                 ulMsg,
                                 mpParm1,
                                 mpParm2));

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