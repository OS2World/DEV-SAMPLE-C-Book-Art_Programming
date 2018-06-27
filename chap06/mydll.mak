ALL:    MYDLL.DLL MYDLL.LIB

MYDLL.LIB:                       MYDLL.DLL
        IMPLIB MYDLL.LIB MYDLL.DEF

MYDLL.DLL:                       MYDLL.OBJ
        LINK386 /NOI @<<
MYDLL
MYDLL.DLL
MYDLL
OS2386
MYDLL
<<


MYDLL.OBJ:                  MYDLL.C MYDLL.MAK MYDLL.DEF
        ICC -C+ -Ge- -Kb+ -Ss+ MYDLL.C

