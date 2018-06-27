ALL:    CHK16550.EXE

CHK16550.EXE:                       CHK16550.OBJ
        LINK386 /NOI @<<
CHK16550
CHK16550
CHK16550
OS2386+16bitio
CHK16550
<<


CHK16550.OBJ:                       CHK16550.C
        ICC -C+ -Gm+ -Kb+ -Sm -Ss+  CHK16550.C

