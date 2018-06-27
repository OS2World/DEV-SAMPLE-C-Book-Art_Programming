#define  INCL_DOS
#define  INCL_WINPROGRAMLIST
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
INT main(VOID)

{

   STARTDATA        sd;
   PID              procID;
   ULONG            sessID;
   APIRET           rc;

/* initialize everything to 0                                  */

   procID = 0;
   sessID = 0;
   memset(&sd,
          0,
          sizeof(STARTDATA));

/* for Warp or OS/2 for Windows, start real Windows            */

   sd.PgmName = "C:\\WIN\\WIN.COM";

/* insert path to some Windows program here                    */

   sd.PgmInputs = "C:\\WIN\\NOTEPAD.EXE";
   sd.SessionType = PROG_SEAMLESSCOMMON;
   sd.Length = sizeof(STARTDATA);

/* in this case start as independent                           */

   sd.Related = SSF_RELATED_INDEPENDENT;
   sd.FgBg = SSF_FGBG_FORE;
   sd.TraceOpt = SSF_TRACEOPT_NONE;
   sd.TermQ = 0;
   sd.Environment = NULL;
   sd.PgmControl = SSF_CONTROL_VISIBLE|SSF_CONTROL_SETPOS;
   sd.InitXPos = 50;
   sd.InitYPos = 50;
   sd.InitXSize = 400;
   sd.InitYSize = 600;
   sd.Reserved = 0;
   sd.ObjectBuffer = NULL;
   sd.ObjectBuffLen = 0;

   rc = DosStartSession(&sd,
                        &sessID,
                        &procID);
   printf("\nReturn code rc from DosStartSession = %d",
          rc);
   fflush(stdout);
   return (0);

}

