#define  INCL_WIN
#define  INCL_GPILCIDS

#include <os2.h>
#include <string.h>
#include <ctype.h>

#define  CLS_CLIENT    "MyClass"
#define  IDE_ENTRYFIELD 256
#define  STR_TEXT      "Zip code:"
#define  UM_CREATEDONE WM_USER+1

MRESULT EXPENTRY newEntryWndProc(HWND hwndWnd,ULONG ulMsg,MPARAM
                                  mpParm1,MPARAM mpParm2);

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

   ulFlags = FCF_TITLEBAR|FCF_SYSMENU|FCF_SIZEBORDER|FCF_MINMAX|
      FCF_TASKLIST;

   /************************************************************/
   /* create frame window                                      */
   /************************************************************/

   hwndFrame = WinCreateStdWindow(HWND_DESKTOP,
                                  0,
                                  &ulFlags,
                                  CLS_CLIENT,
                                  "Subclass Example",
                                  0,
                                  NULLHANDLE,
                                  0,
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
         WinPostMsg(hwndClient,
                    UM_CREATEDONE,
                    MPVOID,
                    MPVOID);
         break;

      case  UM_CREATEDONE :

         {
            HPS              hpsChar;
            FONTMETRICS      fmMetrics;
            ENTRYFDATA       edEntry;
            HWND             hwndEntry;
            PFNWP            pfnOldEntryProc;
            RECTL            rclClient;
            LONG             lHeight,lWidth;

            hpsChar = WinGetPS(hwndClient);
            GpiQueryFontMetrics(hpsChar,
                                sizeof(fmMetrics),
                                &fmMetrics);
            WinReleasePS(hpsChar);

            /***************************************************/
            /* set up entryfield data                          */
            /***************************************************/

            edEntry.cb = sizeof(edEntry);
            edEntry.cchEditLimit = 9;
            edEntry.ichMinSel = 0;
            edEntry.ichMaxSel = 0;

            WinQueryWindowRect(hwndClient,
                               &rclClient);
            lHeight = rclClient.yTop-rclClient.yBottom;
            lWidth = rclClient.xRight-rclClient.xLeft;

            /***************************************************/
            /* create entryfield window                        */
            /***************************************************/

            hwndEntry = WinCreateWindow(hwndClient,
                                        WC_ENTRYFIELD,
                                        "",
                                        WS_VISIBLE|ES_MARGIN|
                                           ES_AUTOSIZE,
                                        lWidth/4,
                                        lHeight/4,
                                        fmMetrics.lAveCharWidth
                                        *15,
                                       fmMetrics.lMaxBaselineExt,
                                        hwndClient,
                                        HWND_TOP,
                                        IDE_ENTRYFIELD,
                                        &edEntry,
                                        NULL);

            pfnOldEntryProc = WinSubclassWindow(hwndEntry,
                                                newEntryWndProc);

            WinSetWindowPtr(hwndEntry,
                            QWL_USER,
                            (PVOID)pfnOldEntryProc);

            WinSetFocus(HWND_DESKTOP,
                        hwndEntry);
         }
         break;

      case  WM_DESTROY :
         WinDestroyWindow(WinWindowFromID(hwndClient,
                                          IDE_ENTRYFIELD));
         break;

      case  WM_PAINT :
         {
            HPS              hpsPaint;
            SWP              swpEntry;
            POINTL           ptlText;

            hpsPaint = WinBeginPaint(hwndClient,
                                     NULLHANDLE,
                                     NULL);

            GpiErase(hpsPaint);
            WinQueryWindowPos(WinWindowFromID(hwndClient,
                                              IDE_ENTRYFIELD),
                              &swpEntry);

            /***************************************************/
            /* position label on top of entryfield             */
            /***************************************************/

            ptlText.x = swpEntry.x;
            ptlText.y = swpEntry.y+swpEntry.cy+10;

            GpiCharStringAt(hpsPaint,
                            &ptlText,
                            strlen(STR_TEXT),
                            STR_TEXT);

            WinEndPaint(hpsPaint);
         }
         break;
      default  :
         return  WinDefWindowProc(hwndClient,
                                  ulMsg,
                                  mpParm1,

                                  mpParm2);
   }                                   /* endswitch            */
   return  MRFROMSHORT(FALSE);
}

MRESULT EXPENTRY newEntryWndProc(HWND hwndEntry,ULONG ulMsg,
                                  MPARAM mpParm1,MPARAM mpParm2)
{
   PFNWP            pfnOldEntryProc;

   pfnOldEntryProc = (PFNWP)WinQueryWindowPtr(hwndEntry,
                                              QWL_USER);

   switch (ulMsg)
   {

      /*********************************************************/
      /* check for keystrokes                                  */
      /*********************************************************/

      case  WM_CHAR :
         if (CHARMSG(&ulMsg)->fs&KC_CHAR)
         {

            /***************************************************/
            /* test for what is allowed                        */
            /***************************************************/

            if (!isdigit(CHARMSG(&ulMsg)->chr) &&
               (CHARMSG(&ulMsg )->chr != '\b'))
            {

               WinMessageBox(HWND_DESKTOP,
                             HWND_DESKTOP,
               "Only numeric characters are allowed in this field",
                             "Numeric Field",
                             0,
                             MB_OK|MB_ERROR);

               return  MRFROMSHORT(TRUE);
            }                          /* endif                */
         }                             /* endif                */
         break;
      case  EM_PASTE :
         {

            /***************************************************/
            /* check for pasting from the clipboard            */
            /***************************************************/

            HAB              habAnchor;
            PCHAR            pchText;
            CHAR             achText[1024];
            USHORT           usIndex;

            habAnchor = WinQueryAnchorBlock(hwndEntry);
            WinOpenClipbrd(habAnchor);
            pchText = (PCHAR)WinQueryClipbrdData(habAnchor,
                                                 CF_TEXT);

            if (pchText)
            {

               strcpy(achText,
                      pchText);
               WinCloseClipbrd(habAnchor);

               usIndex = 0;

               /************************************************/
               /* loop through string checking for non-numeric */
               /************************************************/

               while (achText[usIndex])
               {
                  if (!isdigit(achText[usIndex++]))
                  {

                     WinMessageBox(HWND_DESKTOP,
                                   HWND_DESKTOP,
                                   "Only numeric characters are "
                                      "allowed in this field",
                                   "Numerical Field",
                                   0,
                                   MB_OK|MB_ERROR);

                     return  MRFROMSHORT(TRUE);
                  }                    /* endif                */
               }                       /* endwhile             */
            }

            else
            {
               WinCloseClipbrd(habAnchor);
            }                          /* endif                */
         }
         break;
      default  :
         break;
   }                                   /* endswitch            */
   return (*pfnOldEntryProc)(hwndEntry,
                             ulMsg,
                             mpParm1,
                             mpParm2);
}
