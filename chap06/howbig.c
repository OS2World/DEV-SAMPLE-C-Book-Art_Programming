#include <stdio.h>
int main(void);

extern unsigned long far pascal HowMany(unsigned int *,char *);

#define  FILE_NAME     "BIGFILE"
int main(void)

{
   unsigned long    ulCount = 0;
   unsigned int     usCheckSum = 0;

   printf("\n Now inside 16-bit %s", 
          __FILE__);
   printf("\n size of usCheckSum (USHORT * )  = %d", 
          sizeof(int *));
   printf("\n size of FILE_NAME  (char *   )  = %d", 
          sizeof(char *));

   printf(
      "\n About to call the 32-bit DLL function automatically\n")
      ;

   ulCount = HowMany(&usCheckSum, 
                     FILE_NAME);

   if (ulCount == 0)
      printf("\n Could not calculate checksum, sorry!\n\n");
   else
      printf("\n File: %s, checksum: %04X, Count: %ld\n\n", 
             FILE_NAME, 
             usCheckSum, 
             ulCount);
   return (0);
} 
