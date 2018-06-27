#include <os2.h>
#include <stdio.h>
#include <string.h>
#define  MY_CHAR_OUT_VALUE "Output character string"
int MyDLLFunction(PFN,PCHAR);

int _System MyPrintFunctionInDLL(PCHAR,PCHAR,...);

int MyDLLFunction(PFN MyFunctionPointer,PCHAR MyCharacterPointer)
{
   int              rc = 0;

   rc = (MyFunctionPointer)("\n Modifying < %s > in   DLL\n", 
                            MyCharacterPointer);
   return (rc);
} 

int _System MyPrintFunctionInDLL(PCHAR First,PCHAR Second,...)
{
   printf(First, 
          Second);
   strcpy(Second, 
          MY_CHAR_OUT_VALUE);
   return (0);
} 
