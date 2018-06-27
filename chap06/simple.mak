ALL:    SIMPLE.EXE

SIMPLE.EXE:                       SIMPLE.OBJ
        LINK386 /NOI @<<
SIMPLE
SIMPLE
SIMPLE
OS2386+MYDLL
SIMPLE
<<


SIMPLE.OBJ:                       SIMPLE.C
        ICC -C+ -Gm+ -Kb+ -Sm -Ss+ SIMPLE.C

