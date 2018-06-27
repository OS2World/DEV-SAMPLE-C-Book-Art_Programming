ICCOPTS=-C+ -Gm- -Kb+ -Ss+
LINKOPTS=/MAP /A:16

CONTAIN4.EXE:                   CONTAIN4.OBJ \
                                CONTAIN4.RES
        LINK386 $(LINKOPTS) @<<
CONTAIN4
CONTAIN4
CONTAIN4
OS2386
CONTAIN4
<<
        RC CONTAIN4.RES CONTAIN4.EXE

CONTAIN4.RES:                   CONTAIN4.RC \
                                CONTAIN4.H
        RC -r CONTAIN4.RC CONTAIN4.RES

CONTAIN4.OBJ:                   CONTAIN4.C \
                                CONTAIN4.H
        ICC $(ICCOPTS) CONTAIN4.C
