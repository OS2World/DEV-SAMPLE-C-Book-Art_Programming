/*  Assume                                                     */
/*                                                             */
/*        COM1 -> 0x3F8                                        */
/*        COM2 -> 0x2F8                                        */
/*                                                             */
/*One attempts to first clear the 16550 FIFO by writing a 0x00 to*/
/*the FIFO Control register at offset 0x02.  Then one attempts to*/
/*enable the FIFOs by setting bit0 of the FIFO Control register*/
/*at offset 0x02.  Reading the Interrupt Identification register*/
/*at offset 0x02 will tell one if 16550 is present.          */
/*                                                             */

#define  INCL_DOSMODULEMGR
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "man16550.h"
int main(void);

#define  BIT_6_7_SET   0x00C0
#define  IGNORE_ORDINAL_NUMBER 0
int main(void)

{
   unsigned         Byte = 0;

   HMODULE          hmod;
   APIRET           rc = 0;
   UCHAR            szDLLName[CCHMAXPATH];/* CCHMAXPATH ->
                                          bsedos.h via toolkit */
   UCHAR            szBadName[CCHMAXPATH];/* Load the
                                          16bitio.dll          */

   strcpy(szDLLName,
          ".\\16bitio.dll");           /* Look for DLL in the
                                          same directory       */
   rc = DosLoadModule(szBadName,
                      CCHMAXPATH-1,
                      szDLLName,
                      &hmod);

   if (rc)
   {
      printf("\n Could not load %s successfully, bad %s, rc = \n"
         ,
             szDLLName,
             szBadName,
             rc);
      return (-1);
   }
   else
      printf("\n Loaded %s successfully\n",
             szDLLName);               /* Get the my_inp
                                          function address     */
   rc = DosQueryProcAddr(hmod,
                         IGNORE_ORDINAL_NUMBER,
                         "MY_INP",
                         (PFN *)&my_inp);

   if (rc)
   {
      printf("\n Could not find address for my_inp, rc = \n",
             rc);
      DosFreeModule(hmod);
      return (-1);
   }
   else
      printf("\n Found my_inp()  function address\n");/* Get
                                          the my_outp function
                                          address              */
   rc = DosQueryProcAddr(hmod,
                         IGNORE_ORDINAL_NUMBER,
                         "MY_OUTP",
                         (PFN *)&my_outp);

   if (rc)
   {
      printf("\n Could not find address for my_outp, rc = \n",
             rc);
      DosFreeModule(hmod);
      return (-1);
   }
   else
      printf("\n Found my_outp() function address\n");

   printf("\n\n Attempting to find 16550 UART ...");/* test
                                          COM1                 */
   my_outp(MY_COM1+MY_FIFO_CTRL,
           0x00);                      /* Clear the FIFO reg   */
   Byte = my_inp(MY_COM1+MY_INT_ID);
   Byte &= BIT_6_7_SET;
   if (!Byte)
   {

      my_outp(MY_COM1+MY_FIFO_CTRL,
              0x01);                   /* Set the FIFO reg     */
      if (my_inp(MY_COM1+MY_INT_ID)&BIT_6_7_SET)
         printf(
           "\n\n 16550 appears to be present for COM1->0x3F8.\n")
            ;
      else
         printf(
            "\n\n 16550 appears to be absent for COM1->0x3F8.\n")
            ;

   }
   else
   {
      printf(
         "\n\n Unknown error for COM1->0x3F8. Exiting ... \n\n");
      DosFreeModule(hmod);
      return (-1);
   }                                   /* test COM2 loop? :)   */
   my_outp(MY_COM2+MY_FIFO_CTRL,
           0x00);                      /* Clear the FIFO reg   */
   if (!(Byte = (my_inp(MY_COM2+MY_INT_ID)&BIT_6_7_SET)))
   {

      my_outp(MY_COM2+MY_FIFO_CTRL,
              0x01);                   /* Set the FIFO reg     */
      if (my_inp(MY_COM2+MY_INT_ID)&BIT_6_7_SET)
         printf(
           "\n 16550 appears to be present for COM2->0x2F8.\n\n")
            ;
      else
         printf(
            "\n 16550 appears to be absent for COM2->0x2F8.\n\n")
            ;

   }
   else
   {
      printf(
         "\n\n Unknown error for COM2->0x2F8. Exiting ... \n\n");
      DosFreeModule(hmod);
      return (-1);
   }

   /************************************************************/
   /* Free the 16bitio.dll module                              */
   /************************************************************/

   rc = DosFreeModule(hmod);

   if (rc)
   {
      printf("\n Could not free %s successfully, rc = \n",
             szDLLName,
             rc);
      return (-1);
   }
   else
      printf("\n Freed %s successfully\n",
             szDLLName);

   return (0);
}
