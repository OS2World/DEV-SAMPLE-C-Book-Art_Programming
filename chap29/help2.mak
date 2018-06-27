ICCOPTS=-C+ -Gm- Kb+ -Ss+
LINKOPTS=/MAP /A:16

HELP2.EXE:                      HELP2.OBJ \
                                HELP2.RES \
                                HELP2.HLP
        LINK386 $(LINKOPTS) @<<
HELP2
HELP2
HELP2
OS2386
HELP2
<<
        RC HELP2.RES HELP2.EXE

HELP2.OBJ:                      HELP2.C \
                                HELP2.H
        ICC $(ICCOPTS) HELP2.C

HELP2.RES:                      HELP2.RC \
                                HELP2.H
        RC -r HELP2.RC HELP2.RES

HELP2.HLP:                      HELP2.IPF
        IPFC HELP2.IPF
