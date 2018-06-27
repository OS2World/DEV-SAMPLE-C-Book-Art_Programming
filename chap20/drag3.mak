DRAG3.EXE:                   DRAG3.OBJ \
                             DRAG3.RES
        LINK386 @<<
DRAG3
DRAG3
DRAG3
OS2386
DRAG3
<<
        RC DRAG3.RES DRAG3.EXE

DRAG3.RES:                   DRAG3.RC \
                             DRAG3RC.H
        RC -r DRAG3.RC DRAG3.RES

DRAG3.OBJ:                   DRAG3.C \
                             DRAG3RC.H
        ICC -C+ -Kb+ -Ss+ DRAG3.C
