ALL:    AUT16550.EXE

AUT16550.EXE:                       AUT16550.OBJ
        LINK386 /NOI @<<
AUT16550
AUT16550
AUT16550
OS2386+16bitio
AUT16550
<<


AUT16550.OBJ:                       AUT16550.C AUT16550.MAK AUT16550.DEF
        ICC -C+ -Gm+ -Kb+ -Sm -Ss+ AUT16550.C

