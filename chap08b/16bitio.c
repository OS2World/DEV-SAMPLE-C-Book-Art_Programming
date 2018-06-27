/* 16-bit I/O dll */

_acrtused = 0;

#include <conio.h>

int far _pascal my_inp          (unsigned );
int far _pascal my_outp         (unsigned , unsigned );
unsigned far _pascal my_inpw    (unsigned );
unsigned far _pascal my_outpw   (unsigned , unsigned );

int far _pascal my_inp  ( unsigned usPort ){
   return (inp(usPort));
}

int far _pascal my_outp ( unsigned usPort, unsigned usValue ){
   return (outp(usPort, usValue));
}


unsigned far _pascal my_inpw  ( unsigned usPort ){
   return (inpw(usPort));
}

unsigned far _pascal my_outpw ( unsigned usPort, unsigned usValue ){
   return (outpw(usPort, usValue));
}
