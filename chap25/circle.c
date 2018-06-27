#define  INCL_WIN
#define  INCL_GPIBITMAPS
#include <os2.h>
#include "circle.h"
#include <stdlib.h>
#include <stdio.h>
#define  CLS_CLIENT    "ClientClass"

 /* Procedure Prototype                                        */

MRESULT EXPENTRY ClientWndProc(HWND hwndClient,ULONG ulMsg,MPARAM
                                mp1,MPARAM mp2);

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
                                  "Circular Slider Example",
                                  0,
                                  NULLHANDLE,
                                  ID_FRAME,
                                  NULL);

   /************************************************************/
   /* get screen height and width                              */
   /************************************************************/

   lWidth = WinQuerySysValue(HWND_DESKTOP,
                             SV_CXSCREEN);

   lHeight = WinQuerySysValue(HWND_DESKTOP,
                              SV_CYSCREEN);

   /************************************************************/
   /* if failed, and set to default value                      */
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
                                mp1,MPARAM mp2)
{

   HWND             hwndCirc;

   switch (ulMsg)

   {

      case  WM_CREATE :
         {
            RGB2             rgb2 ;

            /***************************************************/
            /* Create circular slider control                  */
            /***************************************************/

            hwndCirc = WinCreateWindow(hwndClient,
                                       WC_CIRCULARSLIDER,
                                       "Volume",
                                       CSS_MIDPOINT,
                                       0,
                                       0,
                                       0,
                                       0, /* Position & size   */
                                       hwndClient,
                                       HWND_TOP,
                                       ID_DIAL,
                                       NULL,
                                       NULL);

            /***************************************************/
            /* error checking                                  */
            /***************************************************/

            if (!hwndCirc)
               return  MRFROMSHORT(FALSE);

            /***************************************************/
            /* change background color of slider to white      */
            /***************************************************/

            rgb2.bRed = 0xFF;
            rgb2.bGreen = 0xFF;
            rgb2.bBlue = 0xFF;
            rgb2.fcOptions = 0 ;

            WinSetPresParam(hwndCirc,
                            PP_BACKGROUNDCOLOR,
                            sizeof(RGB2),
                            &rgb2);

            /***************************************************/
            /* Specify range of values for circular slider     */
            /***************************************************/

            WinSendMsg(hwndCirc,
                       CSM_SETRANGE,
                       MPFROMLONG(0),
                       MPFROMLONG(60));

            /***************************************************/
            /* Specify scroll & tick mark increments           */
            /***************************************************/

            WinSendMsg(hwndCirc,
                       CSM_SETINCREMENT,
                       MPFROMLONG(10),
                       MPFROMLONG(0));

            /***************************************************/
            /* Set initial value                               */
            /***************************************************/

            WinSendMsg(hwndCirc,
                       CSM_SETVALUE,
                       MPFROMLONG(30),
                       NULL);

            return MRFROMSHORT(FALSE);
         }
      case  WM_SIZE :
         {

            SWP              swp;

            /***************************************************/
            /* resize circular slider as proportion of client  */
            /* window size                                     */
            /***************************************************/

            WinQueryWindowPos(hwndClient,
                              &swp);
            hwndCirc = WinWindowFromID(hwndClient,
                                       ID_DIAL);
            WinSetWindowPos(hwndCirc,
                            HWND_TOP,
                            swp.cx/4,
                            swp.cy/4,
                            swp.cx/2,
                            swp.cy/2,
                            SWP_MOVE|SWP_SHOW|SWP_SIZE);
            return MRFROMSHORT(FALSE);
         }
      case  WM_PAINT :
         {

            HPS              hps;

            /***************************************************/
            /* simple paint procedure to fill in client        */
            /* background                                      */
            /***************************************************/

            hps = WinBeginPaint(hwndClient,
                                0,
                                NULL);
            GpiErase(hps);
            WinEndPaint(hps);
            return MRFROMSHORT(FALSE);
         }
      default  :
         return (WinDefWindowProc(hwndClient,
                                  ulMsg,
                                  mp1,
                                  mp2));
   }
}

