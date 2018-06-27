/* Simple DLL loader                                           */

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(VOID);

extern int MyDLLFunction(PFN,PCHAR);
extern int _System MyPrintFunctionInDLL(PCHAR,PCHAR,...);

#define  MY_CHAR_IN_VALUE "Input  character string"
#define  MY_CHAR_IN_SIZE strlen(MY_CHAR_IN_VALUE)
int main(VOID)

{

   int              rc = 0;
   PCHAR            MyCharacterPointer;

   MyCharacterPointer = (PCHAR)malloc(MY_CHAR_IN_SIZE);
   if (MyCharacterPointer)
   {
      strcpy(MyCharacterPointer, 
             MY_CHAR_IN_VALUE);
      printf("\n Sending   < %s > to   DLL\n", 
             MyCharacterPointer);
      rc = MyDLLFunction(MyPrintFunctionInDLL, 
                         MyCharacterPointer);
      printf("\n Returned  < %s > from DLL\n", 
             MyCharacterPointer);
      return (rc);
   } 
   else
      return (-1);
} 

