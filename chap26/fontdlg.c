#define  INCL_WIN
#define  INCL_STDDLG
#define  INCL_GPI
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fontdlg.h"

typedef struct
{
   FONTDLG          fdFontDlg;
   USHORT           bInit;
} MYFONTINFO,*PMYFONTINFO;

MRESULT EXPENTRY ClientWndProc(HWND hwndClient,ULONG ulMsg,MPARAM
                                mpParm1,MPARAM mpParm2);

VOID SetFont(HWND hwndClient,PMYFONTINFO pmfiFont);
BOOL InitFont(HWND hwndClient,PFONTDLG pfdFontDlg);
VOID DisplayError(CHAR *pszText);

#define  CLS_CLIENT    "MyClass"

INT main(VOID)

{
   HMQ              hmqQueue;
   HAB              habAnchor;
   ULONG            ulFlags;
   HWND             hwndClient;
   BOOL             bLoop;
   QMSG             qmMsg;
   LONG             lWidth,lHeight;
   HWND             hwndFrame = NULLHANDLE;

   habAnchor = WinInitialize(0);
   hmqQueue = WinCreateMsgQueue(habAnchor,
                                0);

   WinRegisterClass(habAnchor,
                    CLS_CLIENT,
                    ClientWndProc,
                    CS_SIZEREDRAW|CS_SYNCPAINT,
                    sizeof(PVOID));

   ulFlags = FCF_TITLEBAR|FCF_SYSMENU|FCF_SIZEBORDER|FCF_MENU|
      FCF_MINMAX;

   /************************************************************/
   /* create frame and client window                           */
   /************************************************************/

   hwndFrame = WinCreateStdWindow(HWND_DESKTOP,
                                  0,
                                  &ulFlags,
                                  CLS_CLIENT,
                                  "Font Dialog Example",
                                  0,
                                  NULLHANDLE,
                                  RES_CLIENT,
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

   if (hwndFrame)
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
   PMYFONTINFO      pmfiFont;

   switch (ulMsg)
   {

      case  WM_CREATE :
         {

            /***************************************************/
            /* allocate the window word                        */
            /***************************************************/

            pmfiFont = calloc(1,
                              sizeof(MYFONTINFO));
            if (!pmfiFont)
            {
               DisplayError("No memory could be allocated");
               return  MRFROMSHORT(TRUE);
            }                          /* endif                */

            WinSetWindowPtr(hwndClient,
                            QWL_USER,
                            pmfiFont);

            /***************************************************/
            /* set font to 8 pt Times Roman                    */
            /***************************************************/

            WinSetPresParam(hwndClient,
                            PP_FONTNAMESIZE,
                            10,
                            "8.Tms Rmn");

            /***************************************************/
            /* indicates first time dialog is brought up       */
            /***************************************************/

            pmfiFont->bInit = FALSE;
            break;
         }
      case  WM_DESTROY :
         {

            /***************************************************/
            /* clean up                                        */
            /***************************************************/

            pmfiFont = WinQueryWindowPtr(hwndClient,
                                         QWL_USER);
            if (pmfiFont)
            {
               free(pmfiFont);
            }                          /* endif                */
            break;
         }
      case  WM_PAINT :
         {
            HPS              hpsPaint;
            ULONG            ulReturn;
            RECTL            rclPaint;
            CHAR             achFontName[200],achMsg[256];

            hpsPaint = WinBeginPaint(hwndClient,
                                     NULLHANDLE,
                                     &rclPaint);

            /***************************************************/
            /* get the current font                            */
            /***************************************************/

            ulReturn = WinQueryPresParam(hwndClient,
                                         PP_FONTNAMESIZE,
                                         0,
                                         NULL,
                                         256,
                                         achFontName,
                                         0);
            if (ulReturn)
            {
               sprintf(achMsg,
                       "The font selected is \"%s\"",
                       achFontName);
            }
            else
            {
               strcpy(achMsg,
                      "No font selected");
            }                          /* endif                */

            /***************************************************/
            /* clear out the window                            */
            /***************************************************/

            WinFillRect(hpsPaint,
                        &rclPaint,
                        SYSCLR_WINDOW);
            WinQueryWindowRect(hwndClient,
                               &rclPaint);

            /***************************************************/
            /* write out current font on client area           */
            /***************************************************/

            WinDrawText(hpsPaint,
                        -1,
                        achMsg,
                        &rclPaint,
                        0,
                        0,
                        DT_VCENTER|DT_CENTER|DT_TEXTATTRS);

            WinEndPaint(hpsPaint);
         }
         break;

      case  WM_COMMAND :

         switch (SHORT1FROMMP(mpParm1))
         {

            case  IDM_FONT :
               {

                  /*********************************************/
                  /* get window word                           */
                  /*********************************************/

                  pmfiFont = WinQueryWindowPtr(hwndClient,
                                               QWL_USER);
                  if (!pmfiFont)
                  {
                     DisplayError(
                 "Unable to retrieve font information structure")
                        ;
                     break;
                  }

                  /*********************************************/
                  /* do font dialog                            */
                  /*********************************************/

                  SetFont(hwndClient,
                          pmfiFont);

                  /*********************************************/
                  /* repaint window with new font              */
                  /*********************************************/

                  WinInvalidateRect(hwndClient,
                                    NULL,
                                    TRUE);
                  break;
               }
            case  IDM_EXIT :
               {
                  WinPostMsg(hwndClient,
                             WM_CLOSE,
                             0,
                             0);
                  break;
               }
            default  :
               return  WinDefWindowProc(hwndClient,
                                        ulMsg,
                                        mpParm1,

                                        mpParm2);
         }                             /* endswitch            */
         break;

      default  :
         return  WinDefWindowProc(hwndClient,
                                  ulMsg,
                                  mpParm1,

                                  mpParm2);
   }                                   /* endswitch            */
   return  MRFROMSHORT(FALSE);
}

VOID SetFont(HWND hwndClient,PMYFONTINFO pmfiFont)
{
   FATTRS           faAttrs;
   FIXED            fxSzFont;
   CHAR             achFamily[256];
   CHAR             achFont[256];

static char TestString[]="abcdABCD ¡¢£¤€‚ƒ„\n0123456789";
   /************************************************************/
   /* save some values                                         */
   /************************************************************/

   faAttrs = pmfiFont->fdFontDlg.fAttrs;
   fxSzFont = pmfiFont->fdFontDlg.fxPointSize;

   /************************************************************/
   /* clear out the structures                                 */
   /************************************************************/

   memset(&pmfiFont->fdFontDlg,
          0,
          sizeof(FONTDLG));
   memset(achFont,
          0,
          256);

   if (pmfiFont->bInit)
   {

      /*********************************************************/
      /* fontdialog structure has already been initialized     */
      /*********************************************************/

      pmfiFont->fdFontDlg.fAttrs = faAttrs;
      pmfiFont->fdFontDlg.fxPointSize = fxSzFont;
   }                                   /* endif                */
   else
   {

      /*********************************************************/
      /* first time through                                    */
      /*********************************************************/

      InitFont(hwndClient,
               &(pmfiFont->fdFontDlg));
      pmfiFont->bInit = TRUE;
   }

   /************************************************************/
   /* joint initialization                                     */
   /************************************************************/

   pmfiFont->fdFontDlg.hpsScreen = WinGetPS(hwndClient);
   pmfiFont->fdFontDlg.cbSize = sizeof(FONTDLG);

   pmfiFont->fdFontDlg.pszFamilyname = achFamily;
   pmfiFont->fdFontDlg.usFamilyBufLen = sizeof(achFamily);
   pmfiFont->fdFontDlg.fl = FNTS_CENTER|FNTS_INITFROMFATTRS;
   pmfiFont->fdFontDlg.clrFore = CLR_NEUTRAL;
   pmfiFont->fdFontDlg.clrBack = SYSCLR_WINDOW;
   pmfiFont->fdFontDlg.pszPreview = TestString;

   /************************************************************/
   /* return from font dialog is TRUE or FALSE for non-modeless*/
   /* font dialog, window handle if FNTS_MODELESS is set ours  */
   /* is non-modeless, so we check for TRUE or FALSE           */
   /************************************************************/

   if (!WinFontDlg(HWND_DESKTOP,
                   hwndClient,
                   &pmfiFont->fdFontDlg))
   {

      DisplayError("WinFontDlg failed");
      return ;
   }                                   /* endif                */

   /************************************************************/
   /* clean up                                                 */
   /************************************************************/

   WinReleasePS(pmfiFont->fdFontDlg.hpsScreen);

   sprintf(achFont,
           "%d.%s",
           FIXEDINT(pmfiFont->fdFontDlg.fxPointSize),
           pmfiFont->fdFontDlg.fAttrs.szFacename);

   /************************************************************/
   /* check for various attributes                             */
   /************************************************************/

   if (pmfiFont->fdFontDlg.fAttrs.fsSelection&FATTR_SEL_ITALIC)
   {
      strcat(achFont,
             ".Italic");
   }                                   /* endif                */
   if (pmfiFont->fdFontDlg.fAttrs.fsSelection
      &FATTR_SEL_UNDERSCORE)
   {
      strcat(achFont,
             ".Underscore");
   }                                   /* endif                */
   if (pmfiFont->fdFontDlg.fAttrs.fsSelection&FATTR_SEL_STRIKEOUT
      )
   {
      strcat(achFont,
             ".Strikeout");
   }                                   /* endif                */
   if (pmfiFont->fdFontDlg.fAttrs.fsSelection&FATTR_SEL_BOLD)
   {
      strcat(achFont,
             ".Bold");
   }                                   /* endif                */
   if (pmfiFont->fdFontDlg.fAttrs.fsSelection&FATTR_SEL_OUTLINE)
   {
      strcat(achFont,
             ".Outline");
   }                                   /* endif                */

   /************************************************************/
   /* set the new font                                         */
   /************************************************************/

   WinSetPresParam(hwndClient,
                   PP_FONTNAMESIZE,
                   strlen(achFont)+1,
                   achFont);

   return ;
}

/***************************************************************/
/*                                                             */
/* function that analyzes the current font and initializes     */
/* the font dialog to use the current font as the default font */
/*                                                             */
/***************************************************************/

BOOL InitFont(HWND hwndClient,PFONTDLG pfdFontDlg)
{
   FONTMETRICS      fm;
   HPS              hPS;
   HDC              hDC;
   SIZEF            sizef;
   LONG             lxFontResolution;

   /************************************************************/
   /* Get the presentation space to query fonts from           */
   /************************************************************/

   hPS = WinGetPS(hwndClient);

   if (GpiQueryFontMetrics(hPS,
                           (LONG)sizeof(FONTMETRICS),
                           &fm))
   {

      /*********************************************************/
      /* Initialize the font dialog structure and the fattrs   */
      /* fields                                                */
      /*********************************************************/

      memset(&pfdFontDlg->fAttrs,
             0,
             sizeof(FATTRS));
      pfdFontDlg->fAttrs.usRecordLength = sizeof(FATTRS);

      /*********************************************************/
      /* Initialize the font attributes                        */
      /*********************************************************/

      if (fm.fsSelection&FM_SEL_ITALIC)
         pfdFontDlg->fAttrs.fsSelection |= FATTR_SEL_ITALIC;
      if (fm.fsSelection&FM_SEL_UNDERSCORE)
         pfdFontDlg->fAttrs.fsSelection |= FATTR_SEL_UNDERSCORE;
      if (fm.fsSelection&FM_SEL_OUTLINE)
         pfdFontDlg->fAttrs.fsSelection |= FATTR_SEL_OUTLINE;
      if (fm.fsSelection&FM_SEL_STRIKEOUT)
         pfdFontDlg->fAttrs.fsSelection |= FATTR_SEL_STRIKEOUT;
      if (fm.fsSelection&FM_SEL_BOLD)
         pfdFontDlg->fAttrs.fsSelection |= FATTR_SEL_BOLD;

      /*********************************************************/
      /* Initialize the fattrs match-font and registry id      */
      /*********************************************************/

      pfdFontDlg->fAttrs.lMatch = fm.lMatch;
      pfdFontDlg->fAttrs.idRegistry = fm.idRegistry;

      /*********************************************************/
      /* Initialize the fattrs code page                       */
      /*********************************************************/

      pfdFontDlg->fAttrs.usCodePage = GpiQueryCp(hPS);

      /*********************************************************/
      /* Initialize the fattrs max baseline ext and avg char   */
      /* width                                                 */
      /*********************************************************/

      if (fm.fsDefn&FM_DEFN_OUTLINE)
      {
         pfdFontDlg->fAttrs.lAveCharWidth = 0;
         pfdFontDlg->fAttrs.lMaxBaselineExt = 0;
      }
      else
      {
         pfdFontDlg->fAttrs.lMaxBaselineExt = fm.lMaxBaselineExt;
         pfdFontDlg->fAttrs.lAveCharWidth = fm.lAveCharWidth;
      }

      /*********************************************************/
      /* Initialize the fattrs type indicator field            */
      /*********************************************************/

      if (fm.fsType&FM_TYPE_KERNING)
         pfdFontDlg->fAttrs.fsType |= FATTR_TYPE_KERNING;
      if (fm.fsType&FM_TYPE_DBCS)
         pfdFontDlg->fAttrs.fsType |= FATTR_TYPE_DBCS;
      if (fm.fsType&FM_TYPE_MBCS)
         pfdFontDlg->fAttrs.fsType |= FATTR_TYPE_MBCS;

      /*********************************************************/
      /* Initialize the fattrs typeface name field             */
      /*********************************************************/

      strcpy(pfdFontDlg->fAttrs.szFacename,
             fm.szFacename);

      /*********************************************************/
      /* Initialize the fontdlg style flags                    */
      /*********************************************************/

      pfdFontDlg->fl = FNTS_CENTER|FNTS_HELPBUTTON|
         FNTS_INITFROMFATTRS;

      /*********************************************************/
      /* Initialize the fontdlg weight and width fields        */
      /*********************************************************/

      pfdFontDlg->usWeight = fm.usWeightClass;
      pfdFontDlg->usWidth = fm.usWidthClass;

      /*********************************************************/
      /* Obtain character box size Note: This is critical for  */
      /* outline font calculations ...                         */
      /*********************************************************/

      GpiQueryCharBox(hPS,
                      &sizef);

      /*********************************************************/
      /* Query the underlying device's capabilities for the    */
      /* horizontal device resolution                          */
      /*********************************************************/

      hDC = GpiQueryDevice(hPS);
      DevQueryCaps(hDC,
                   CAPS_HORIZONTAL_FONT_RES,
                   1L,
                   &lxFontResolution);

      /*********************************************************/
      /* Initialize the fattrs font-use indicators, and the    */
      /* fontdlg point size field.                             */
      /*********************************************************/

      if (fm.fsDefn&FM_DEFN_OUTLINE)
      {
         pfdFontDlg->fAttrs.fsFontUse = FATTR_FONTUSE_OUTLINE;
         pfdFontDlg->fxPointSize = (FIXED)((sizef.cx *72)/
            lxFontResolution);

      }                                /* if outline font      */
      else
      {
         pfdFontDlg->fAttrs.fsFontUse = FATTR_FONTUSE_NOMIX;
         pfdFontDlg->fxPointSize = (FIXED)(fm.sNominalPointSize/
            100);

      }                                /* not outline font     */

      /*********************************************************/
      /* Release the PS from above and return                  */
      /*********************************************************/

      WinReleasePS(hPS);
      return (TRUE);
   }

   /************************************************************/
   /* error in GpiQueryFontMetrics                             */
   /************************************************************/

   DisplayError("GpiQueryFontMetrics failed");

   /************************************************************/
   /* Release the PS from above and return                     */
   /************************************************************/

   WinReleasePS(hPS);
   return (FALSE);
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