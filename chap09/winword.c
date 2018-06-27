#define  INCL_WIN
#define  INCL_GPI
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <os2.h>
#define  CLS_CLIENT    "MyClass"
MRESULT EXPENTRY ClientWndProc(HWND hwndWnd,ULONG ulMsg,MPARAM
                                mpParm1,MPARAM mpParm2);

typedef struct   _WININFO
{
   ULONG            ulStructSize;
   BOOL             bStructInit;
   SHORT            sNumWindows;
   HWND             ahwndWindows[10];

} WININFO,*PWININFO;

typedef struct   _LINEINFO
{
   ULONG            ulCharHeight;
   ULONG            usxLeft;
   ULONG            usxRight;
} LINEINFO,*PLINEINFO;

VOID DisplayError(CHAR *pszText);
USHORT DropOneLine(PRECTL prclRect,ULONG ulCharHeight);
VOID DrawString(HPS hpsPaint,HWND hwndPaint,PRECTL prclRect,
                 PLINEINFO pLineInfo,CHAR *pString);

VOID WriteWindowInfo(HPS hpsPaint,HWND hwndPaint,PRECTL prclRect,
                      PLINEINFO pLineInfo);

INT main(VOID)
{
   HAB              habAnchor;
   HMQ              hmqQueue;
   ULONG            ulFlags;
   HWND             hwndFrame;
   HWND             hwndClient;
   BOOL             bLoop;
   QMSG             qmMsg;

   /************************************************************/
   /* initialization                                           */
   /************************************************************/

   habAnchor = WinInitialize(0);
   hmqQueue = WinCreateMsgQueue(habAnchor,
                                0);

   WinRegisterClass(habAnchor,
                    CLS_CLIENT,
                    ClientWndProc,
                    CS_SIZEREDRAW,
                    sizeof(PVOID));

   ulFlags = FCF_TITLEBAR|FCF_SYSMENU|FCF_SIZEBORDER|FCF_MINMAX|
      FCF_SHELLPOSITION|FCF_TASKLIST;

   hwndFrame = WinCreateStdWindow(HWND_DESKTOP,
                                  WS_VISIBLE,
                                  &ulFlags,
                                  CLS_CLIENT,
                                  "Titlebar",
                                  0L,
                                  NULLHANDLE,
                                  0,
                                  &hwndClient);

   if (hwndFrame != NULLHANDLE)
   {
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

   /************************************************************/
   /* clean-up                                                 */
   /************************************************************/

   WinDestroyMsgQueue(hmqQueue);
   WinTerminate(habAnchor);
   return 0;
}

MRESULT EXPENTRY ClientWndProc(HWND hwndWnd,ULONG ulMsg,MPARAM
                                mpParm1,MPARAM mpParm2)
{
   PWININFO         pWinInfo;

   switch (ulMsg)
   {
      case  WM_CREATE :
         {
            pWinInfo = (PWININFO)calloc(1,
                                        sizeof(WININFO));
            if (pWinInfo)
               WinSetWindowPtr(hwndWnd,
                               QWL_USER,
                               pWinInfo);
            else
               DisplayError("No memory allocated for pWinInfo");
            break;
         }
      case  WM_DESTROY :
         {

            pWinInfo = (PWININFO)WinQueryWindowPtr(hwndWnd,
                                                   QWL_USER);
            if (pWinInfo)
               free(pWinInfo);

            break;
         }

      case  WM_PAINT :
         {
            HPS              hpsPaint;
            RECTL            rclRect;
            RECTL            rclWindow;
            HWND             hwndEnum;
            HENUM            heEnum;
            FONTMETRICS      fmMetrics;
            SHORT            i;
            LINEINFO         liLineInfo;

            hpsPaint = WinBeginPaint(hwndWnd,
                                     NULLHANDLE,
                                     &rclRect);

            /***************************************************/
            /* erase the invalid region                        */
            /***************************************************/

            WinFillRect(hpsPaint,
                        &rclRect,
                        SYSCLR_WINDOW);

            /***************************************************/
            /* get font size and save information              */
            /***************************************************/

            GpiQueryFontMetrics(hpsPaint,
                                sizeof(fmMetrics),
                                &fmMetrics);

            liLineInfo.ulCharHeight = fmMetrics.lMaxBaselineExt;

            /***************************************************/
            /* calculate window size information               */
            /***************************************************/

            WinQueryWindowRect(hwndWnd,
                               &rclWindow);
            liLineInfo.usxLeft = (USHORT)fmMetrics.lAveCharWidth;
            liLineInfo.usxRight = rclWindow.xRight-(USHORT)
               fmMetrics.lAveCharWidth;

            /***************************************************/
            /* move down one line                              */
            /***************************************************/

            rclWindow.yTop = rclWindow.yTop-
               liLineInfo.ulCharHeight;
            rclWindow.yBottom = rclWindow.yTop-
               liLineInfo.ulCharHeight;

            /***************************************************/
            /* retrieve window word                            */
            /***************************************************/

            pWinInfo = (PWININFO)WinQueryWindowPtr(hwndWnd,
                                                   QWL_USER);
            if (!pWinInfo)

               /************************************************/
               /* error                                        */
               /************************************************/

               DisplayError("No pWinInfo retrieved in WM_PAINT");
            else
            {

               /************************************************/
               /* first time through initialize structures and */
               /* enumerate windows                            */
               /************************************************/

               if (!pWinInfo->bStructInit)
               {

                  SHORT            sNumWindows = 0;
                  HWND             hwndFrame;

                  /*********************************************/
                  /* start enumrating with the frame window    */
                  /*********************************************/

                  hwndFrame = WinQueryWindow(hwndWnd,
                                             QW_PARENT);
                  WriteWindowInfo(hpsPaint,
                                  hwndFrame,
                                  &rclWindow,
                                  &liLineInfo);

                  heEnum = WinBeginEnumWindows(hwndFrame);

                  /*********************************************/
                  /* save the window handles in window word    */
                  /*********************************************/

                  pWinInfo->ahwndWindows[sNumWindows] = hwndFrame
                     ;
                  hwndEnum = WinGetNextWindow(heEnum);

                  while (hwndEnum != NULLHANDLE)
                  {
                     WriteWindowInfo(hpsPaint,
                                     hwndEnum,
                                     &rclWindow,
                                     &liLineInfo);
                     sNumWindows++;
                     pWinInfo->ahwndWindows[sNumWindows] =
                        hwndEnum;
                     hwndEnum = WinGetNextWindow(heEnum);
                  }                    /* end while hwndEnum   */
                  WinEndEnumWindows(heEnum);

                  /*********************************************/
                  /* keep track of how many windows we have    */
                  /*********************************************/

                  pWinInfo->sNumWindows = sNumWindows;

                  /*********************************************/
                  /* set flag so we know structure is          */
                  /* initialized                               */
                  /*********************************************/

                  pWinInfo->bStructInit = TRUE;
               }                       /* end if structure is
                                          NOT initialized      */
               else
               {

                  /*********************************************/
                  /* if structure has been initialized, just   */
                  /* loop through the window handles and write */
                  /* info to the window                        */
                  /*********************************************/

                  for (i = 0; i <= pWinInfo->sNumWindows; i++)
                  {
                     WriteWindowInfo(hpsPaint,
                                     pWinInfo->ahwndWindows[i],
                                     &rclWindow,
                                     &liLineInfo);
                  }                    /* end for              */
               }                       /* end else struct is
                                          initialized          */
            }                          /* end else             */
            WinEndPaint(hpsPaint);
         }
         break;
      default  :
         return  WinDefWindowProc(hwndWnd,
                                  ulMsg,
                                  mpParm1,

                                  mpParm2);
   }                                   /* endswitch            */
   return  MRFROMSHORT(FALSE);
}

VOID WriteWindowInfo(HPS hpsPaint,HWND hwndPaint,PRECTL prclRect,
                      PLINEINFO pLineInfo)
{
   CHAR             achString[200],achStyle[200];
   CHAR             achClass[65];
   CHAR             achClassText[25];
   PCHAR            pchStart;
   USHORT           usIndex;
   HWND             hwndParent,hwndOwner;
   ULONG            ulStyle;
   PCHAR            apchClasses[] =
   {
      " ","WC_FRAME","WC_COMBOBOX","WC_BUTTON","WC_MENU",
      "WC_STATIC","WC_ENTRYFIELD","WC_LISTBOX","WC_SCROLLBAR",
      "WC_TITLEBAR"
   }

   ;

   /************************************************************/
   /* get window class name                                    */
   /************************************************************/

   WinQueryClassName(hwndPaint,
                     sizeof(achClass),
                     achClass);

   /************************************************************/
   /* save start of classname                                  */
   /************************************************************/

   pchStart = achClass;

   /************************************************************/
   /* public classes are enumerated as #number strip off the   */
   /* pound sign to get index to class name array              */
   /************************************************************/

   if (achClass[0] == '#')
   {
      usIndex = atoi(&achClass[1]);
      strcpy(achClassText,
             apchClasses[usIndex]);
   }
   else
   {
      strcpy(achClassText,
             pchStart);
   }                                   /* endif                */

   /************************************************************/
   /* get window parent and owner                              */
   /************************************************************/

   hwndParent = WinQueryWindow(hwndPaint,
                               QW_PARENT);
   hwndOwner = WinQueryWindow(hwndPaint,
                              QW_OWNER);

   sprintf(achString,
   "Window: 0x%08lX Class: %s, Parent: 0x%08lX Owner: 0x%08lX",
           hwndPaint,
           achClassText,
           hwndParent,
           hwndOwner);

   /************************************************************/
   /* draw the string                                          */
   /************************************************************/

   DrawString(hpsPaint,
              hwndPaint,
              prclRect,
              pLineInfo,
              achString);

   /************************************************************/
   /* get the window styles                                    */
   /************************************************************/

   ulStyle = WinQueryWindowULong(hwndPaint,
                                 QWL_STYLE);
   strcpy(achStyle,
          "Styles: ");

   /************************************************************/
   /* check for some of the more common styles                 */
   /************************************************************/

   if (ulStyle&WS_VISIBLE)
      strcat(achStyle,
             "Visible;");
   if (ulStyle&WS_DISABLED)
      strcat(achStyle,
             " Disabled;");
   if (ulStyle&WS_CLIPCHILDREN)
      strcat(achStyle,
             " Clip Children;");
   if (ulStyle&WS_CLIPSIBLINGS)
      strcat(achStyle,
             " Clip Siblings;");
   if (ulStyle&WS_PARENTCLIP)
      strcat(achStyle,
             " Clip to Parent;");
   if (ulStyle&WS_MAXIMIZED)
      strcat(achStyle,
             " Maximized;");
   if (ulStyle&WS_MINIMIZED)
      strcat(achStyle,
             " Minimized;");
   if (ulStyle&WS_SYNCPAINT)
      strcat(achStyle,
             " Sync Paint;");
   if (ulStyle&WS_SAVEBITS)
      strcat(achStyle,
             " Save Bits;");
   if (usIndex == 1 && (ulStyle&FCF_TITLEBAR))
      strcat(achStyle,
             " Titlebar;");
   if (usIndex == 1 && (ulStyle&FCF_SYSMENU))
      strcat(achStyle,
             " System Menu;");
   if (usIndex == 1 && (ulStyle&FCF_MENU))
      strcat(achStyle,
             " Menu;");
   if (usIndex == 1 && (ulStyle&FCF_SIZEBORDER))
      strcat(achStyle,
             " Size Border;");
   if (usIndex == 1 && (ulStyle&FCF_SHELLPOSITION))
      strcat(achStyle,
             " Shell Position;");
   if (usIndex == 1 && (ulStyle&FCF_TASKLIST))
      strcat(achStyle,
             " Task List;");

   DrawString(hpsPaint,
              hwndPaint,
              prclRect,
              pLineInfo,
              achStyle);
   DropOneLine(prclRect,
               pLineInfo->ulCharHeight);

   return ;
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

USHORT DropOneLine(PRECTL prclRect,ULONG ulCharHeight)
{

   /************************************************************/
   /* small function to move down one line                     */
   /************************************************************/

   LONG             lNewBottom;

   /************************************************************/
   /* will new bottom go off the edge of the window?           */
   /************************************************************/

   lNewBottom = (LONG)(prclRect->yTop-(2*ulCharHeight));
   if (lNewBottom >= 0)
   {
      prclRect->yTop = prclRect->yBottom;
      prclRect->yBottom = lNewBottom;
      return (TRUE);
   }
   else
      return (FALSE);
}

VOID DrawString(HPS hpsPaint,HWND hwndPaint,PRECTL prclRect,
                 PLINEINFO pLineInfo,CHAR *pString)
{

   USHORT           usStringLength;
   USHORT           usNumChars;
   BOOL             bFinished;
   USHORT           usOffset;
   USHORT           usReturn;

   bFinished = FALSE;
   usStringLength = strlen(pString);

   usOffset = 0;

   while (!bFinished)
   {

      /*********************************************************/
      /* move down to next line                                */
      /*********************************************************/

      usReturn = DropOneLine(prclRect,
                             pLineInfo->ulCharHeight);

      /*********************************************************/
      /* if we can't move down any more, stop trying to write  */
      /* any more                                              */
      /*********************************************************/

      if (!usReturn)
         return ;

      /*********************************************************/
      /* set the left and right drawing coordinates            */
      /*********************************************************/

      prclRect->xLeft = pLineInfo->usxLeft;
      prclRect->xRight = pLineInfo->usxRight;

      /*********************************************************/
      /* draw text that will fit                               */
      /*********************************************************/

      usNumChars = WinDrawText(hpsPaint,
                               strlen(&pString[usOffset]),
                               &pString[usOffset],
                               prclRect,
                               0,
                               0,
                               DT_LEFT|DT_TEXTATTRS|DT_WORDBREAK)
         ;

      if (!usNumChars || (usOffset+usNumChars == usStringLength))

         /******************************************************/
         /* if no characters were printed, or we are at the end*/
         /* of the string, quit                                */
         /******************************************************/

         bFinished = TRUE;
      else

         /******************************************************/
         /* offset string to new position                      */
         /******************************************************/

         usOffset += usNumChars;
   }

   return ;

}
