ALL:    count.DLL count.LIB

count.LIB:                       count.DLL
        IMPLIB count.LIB count.DEF

count.DLL:                       count.OBJ
        LINK386 /NOI @<<
count
count.DLL
count
OS2386
count
<<


count.OBJ:                  count.C count.MAK count.DEF
        ICC -C+ -Ge- -Kb+ -Ss+ count.C

