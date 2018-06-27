#define  INCL_WIN
#define  INCL_GPI
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "listbox.h"

typedef struct _BITMAPDATA
{
   CHAR             achName[100];
   USHORT           usNumber;
} BITMAPDATA,*PBITMAPDATA;

#define  MAX_BITMAPS   52
#define  CLS_CLIENT    "MyClass"

BITMAPDATA       abdBitmaps[MAX_BITMAPS] =
{
    "SBMP_OLD_SYSMENU",   SBMP_OLD_SYSMENU,
    "SBMP_OLD_SBUPARROW",   SBMP_OLD_SBUPARROW,
    "SBMP_OLD_SBDNARROW",   SBMP_OLD_SBDNARROW,
    "SBMP_OLD_SBRGARROW",   SBMP_OLD_SBRGARROW,
    "SBMP_OLD_SBLFARROW",   SBMP_OLD_SBLFARROW,
    "SBMP_MENUCHECK",SBMP_MENUCHECK,
    "SBMP_BTNCORNERS", SBMP_BTNCORNERS,
    "SBMP_OLD_MINBUTTON",SBMP_OLD_MINBUTTON,
    "SBMP_OLD_MAXBUTTON",SBMP_OLD_MAXBUTTON,

    "SBMP_OLD_RESTOREBUTTON",SBMP_OLD_RESTOREBUTTON,
    "SBMP_OLD_CHILDSYSMENU",SBMP_OLD_CHILDSYSMENU,

    "SBMP_DRIVE",SBMP_DRIVE,
    "SBMP_FILE",SBMP_FILE,
    "SBMP_FOLDER", SBMP_FOLDER,
    "SBMP_TREEPLUS",SBMP_TREEPLUS,
    "SBMP_TREEMINUS", SBMP_TREEMINUS,

    "SBMP_PROGRAM",  SBMP_PROGRAM,
    "SBMP_MENUATTACHED", SBMP_MENUATTACHED,
    "SBMP_SIZEBOX", SBMP_SIZEBOX,
    "SBMP_SYSMENU", SBMP_SYSMENU,
    "SBMP_MINBUTTON", SBMP_MINBUTTON,
    "SBMP_MAXBUTTON", SBMP_MAXBUTTON,
    "SBMP_RESTOREBUTTON", SBMP_RESTOREBUTTON,
    "SBMP_CHILDSYSMENU", SBMP_CHILDSYSMENU,
    "SBMP_SYSMENUDEP", SBMP_SYSMENUDEP,

    "SBMP_MINBUTTONDEP",SBMP_MINBUTTONDEP,
    "SBMP_MAXBUTTONDEP",SBMP_MAXBUTTONDEP,
    "SBMP_RESTOREBUTTONDEP",SBMP_RESTOREBUTTONDEP,
    "SBMP_CHILDSYSMENUDEP",SBMP_CHILDSYSMENUDEP,
    "SBMP_SBUPARROW",SBMP_SBUPARROW,
    "SBMP_SBDNARROW",SBMP_SBDNARROW,
    "SBMP_SBLFARROW",SBMP_SBLFARROW,
    "SBMP_SBRGARROW",SBMP_SBRGARROW,
    "SBMP_SBUPARROWDEP",SBMP_SBUPARROWDEP,
    "SBMP_SBDNARROWDEP",SBMP_SBDNARROWDEP,

    "SBMP_SBLFARROWDEP",SBMP_SBLFARROWDEP,
    "SBMP_SBRGARROWDEP",SBMP_SBRGARROWDEP,
    "SBMP_SBUPARROWDIS",SBMP_SBUPARROWDIS,
    "SBMP_SBDNARROWDIS",SBMP_SBDNARROWDIS,
    "SBMP_SBLFARROWDIS",SBMP_SBLFARROWDIS,
    "SBMP_SBRGARROWDIS",SBMP_SBRGARROWDIS,
    "SBMP_COMBODOWN",SBMP_COMBODOWN,

 /* "SBMP_OLD_CHECKBOXES",SBMP_OLD_CHECKBOXES, */
 /* "SBMP_CHECKBOXES",SBMP_CHECKBOXES,         */

    "SBMP_OLD_CHECKBOXES",NULLHANDLE,   
    "SBMP_CHECKBOXES",NULLHANDLE,   

    "#50",50,
    "#51",51,
    "#52",52,
    "#53",53,
    "#54",54,
    "#55",55,
    "#56",56,
    "#57",57
} ;

MRESULT EXPENTRY ClientWndProc(HWND hwndWnd,ULONG ulMsg,MPARAM
                                mpParm1,MPARAM mpParm2);

BOOL Draw1Bitmap(HPS hpsDraw,HBITMAP hbmBitmap,PRECTL prclDest);

MRESULT EXPENTRY DlgProc(HWND hwndWnd,ULONG ulMsg,MPARAM mpParm1,
                          MPARAM mpParm2);

INT main(VOID)
{
   HMQ              hmqQueue;
   HAB              habAnchor;
   ULONG            ulFlags;
   HWND             hwndFrame;
   HWND             hwndClient;
   BOOL             bLoop;
   QMSG             qmMsg;
   LONG             lWidth,lHeight;
   int i;

   habAnchor = WinInitialize(0);
   hmqQueue = WinCreateMsgQueue(habAnchor,
                                0);

   WinRegisterClass(habAnchor,
                    CLS_CLIENT,
                    ClientWndProc,
                    CS_SIZEREDRAW|CS_SYNCPAINT,
                    0);

   ulFlags = FCF_TITLEBAR|FCF_SYSMENU|FCF_MENU|FCF_SIZEBORDER|
      FCF_MINMAX;

   /************************************************************/
   /* create frame and client window                           */
   /************************************************************/

   hwndFrame = WinCreateStdWindow(HWND_DESKTOP,
                                  0,
                                  &ulFlags,
                                  CLS_CLIENT,
                                  "Listbox Example",
                                  0,
                                  NULLHANDLE,
                                  ID_RESOURCE,
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

   if (hwndFrame != NULLHANDLE)
   {
      WinSetWindowPos(hwndFrame,
                      NULLHANDLE,
                      10,
                      10,
                      lWidth/10*8,
                      lHeight/10*8,
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

      case  WM_COMMAND :
         switch (SHORT1FROMMP(mpParm1))
         {
            case  IDM_DISPLAY :

               /************************************************/
               /* load up dialog                               */
               /************************************************/

               WinDlgBox(HWND_DESKTOP,
                         hwndClient,
                         DlgProc,
                         NULLHANDLE,
                         IDD_LISTBOX,
                         NULL);
               break;
            case  IDM_EXIT :

               /************************************************/
               /* close up window                              */
               /************************************************/

               WinPostMsg(hwndClient,
                          WM_CLOSE,
                          MPVOID,
                          MPVOID);
               break;
            default  :
               return  WinDefWindowProc(hwndClient,
                                        ulMsg,
                                        mpParm1,
                                        mpParm2);
         }
         break;
      case  WM_PAINT :
         {
            HPS              hpsPaint;

            hpsPaint = WinBeginPaint(hwndClient,
                                     NULLHANDLE,
                                     NULL);

            /***************************************************/
            /* erase the invalidated region                    */
            /***************************************************/

            GpiErase(hpsPaint);
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

MRESULT EXPENTRY DlgProc(HWND hwndDlg,ULONG ulMsg,MPARAM mpParm1,
                          MPARAM mpParm2)

{
   switch (ulMsg)
   {

      case  WM_INITDLG :
         {
            USHORT           usIndex;

            /***************************************************/
            /* insert dummy items into listbox as placeholders */
            /***************************************************/

            for (usIndex = 0; usIndex < MAX_BITMAPS; usIndex++)
            {
               WinSendDlgItemMsg(hwndDlg,
                                 IDL_LISTBOX,
                                 LM_INSERTITEM,
                                 MPFROMSHORT(usIndex),
                                 MPFROMP(""));
            }                          /* endfor               */

            /***************************************************/
            /* select the first listbox item                   */
            /***************************************************/

            WinSendDlgItemMsg(hwndDlg,
                              IDL_LISTBOX,
                              LM_SELECTITEM,
                              MPFROMSHORT(0),
                              MPFROMSHORT(TRUE));
         }
         break;

      case  WM_COMMAND :
         switch (SHORT1FROMMP(mpParm1))
         {
            case  DID_OK :
            case  DID_CANCEL :
               WinDismissDlg(hwndDlg,
                             FALSE);
               break;

            default  :
               return  WinDefDlgProc(hwndDlg,
                                     ulMsg,
                                     mpParm1,
                                     mpParm2);
         }                             /* endswitch            */
         break;

      case  WM_MEASUREITEM :
         {
            HPS              hpsChar;
            FONTMETRICS      fmMetrics;
            LONG             lMaxCy;
            USHORT           usIndex;
            HBITMAP          hbmBitmap;
            BITMAPINFOHEADER2 bmihHeader;

            /***************************************************/
            /* get font size                                   */
            /***************************************************/

            hpsChar = WinGetPS(hwndDlg);
            GpiQueryFontMetrics(hpsChar,
                                (LONG)sizeof(fmMetrics),
                                &fmMetrics);
            WinReleasePS(hpsChar);

            lMaxCy = fmMetrics.lMaxBaselineExt;

            /***************************************************/
            /* get size of bitmaps                             */
            /***************************************************/

            for (usIndex = 0; usIndex < MAX_BITMAPS; usIndex++)
            {
               hbmBitmap = WinGetSysBitmap(HWND_DESKTOP,
                                           abdBitmaps[usIndex].
                                              usNumber);

               bmihHeader.cbFix = 16;
               GpiQueryBitmapInfoHeader(hbmBitmap,
                                        &bmihHeader);

               /************************************************/
               /* which is larger, previous max or bitmap      */
               /************************************************/

               lMaxCy = max(lMaxCy,
                            bmihHeader.cy);

               /************************************************/
               /* free the bitmap                              */
               /************************************************/

               GpiDeleteBitmap(hbmBitmap);
            }                          /* endfor               */
            return  MRFROMLONG(lMaxCy);
         }

      case  WM_DRAWITEM :
         {
            POWNERITEM       poiItem;
            HBITMAP          hbmBitmap;
            RECTL            rclText;

            poiItem = (POWNERITEM)PVOIDFROMMP(mpParm2);

            rclText = poiItem->rclItem;
            rclText.xLeft = (rclText.xRight-rclText.xLeft)/7;

            /***************************************************/
            /* draw the bitmap name                            */
            /***************************************************/

            WinDrawText(poiItem->hps,
                        -1,
                        abdBitmaps[poiItem->idItem].achName,
                        &rclText,
                        poiItem->fsState?CLR_YELLOW:CLR_BLUE,
                        poiItem->fsState?CLR_BLUE:CLR_WHITE,
                        DT_LEFT|DT_VCENTER|DT_ERASERECT);

            rclText = poiItem->rclItem;
            rclText.xRight = (rclText.xRight-rclText.xLeft)/7;

            /***************************************************/
            /* fill the rectangle with white                   */
            /***************************************************/

            WinFillRect(poiItem->hps,
                        &rclText,
                        CLR_WHITE);

            hbmBitmap = WinGetSysBitmap(HWND_DESKTOP,
                                        abdBitmaps[poiItem->
                                           idItem].usNumber);

            /***************************************************/
            /* draw the bitmap, then delete                    */
            /***************************************************/

            Draw1Bitmap(poiItem->hps,
                        hbmBitmap,
                        &rclText);

            GpiDeleteBitmap(hbmBitmap);

            /***************************************************/
            /* set both states to FALSE and return TRUE        */
            /***************************************************/

            poiItem->fsState = FALSE;
            poiItem->fsStateOld = FALSE;

            return  MRFROMSHORT(TRUE);
         }

      default  :
         return  WinDefDlgProc(hwndDlg,
                               ulMsg,
                               mpParm1,
                               mpParm2);
   }                                   /* endswitch            */
   return  MRFROMSHORT(FALSE);
}

BOOL Draw1Bitmap(HPS hpsDraw,HBITMAP hbmBitmap,PRECTL prclDest)
{
   BITMAPINFOHEADER2 bmihHeader;
   POINTL           ptlPoint;
   BOOL             bRc;

   bmihHeader.cbFix = 16;
   GpiQueryBitmapInfoHeader(hbmBitmap,
                            &bmihHeader);

   /************************************************************/
   /* set the x and y coordinates                              */
   /************************************************************/

   ptlPoint.x = (prclDest->xRight-prclDest->xLeft-bmihHeader.cx)/
      2+prclDest->xLeft;

   ptlPoint.y = (prclDest->yTop-prclDest->yBottom-bmihHeader.cy)/
      2+prclDest->yBottom;

   /************************************************************/
   /* draw the bitmap                                          */
   /************************************************************/

   bRc = WinDrawBitmap(hpsDraw,
                       hbmBitmap,
                       NULL,
                       &ptlPoint,
                       0,
                       0,
                       DBM_NORMAL|DBM_IMAGEATTRS);
   return  bRc;
}
