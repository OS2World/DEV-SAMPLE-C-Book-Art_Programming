ICCOPTS=-C+ -Gm- Kb+ -Ss+
LINKOPTS=/MAP /A:16

HELP1.EXE:                      HELP1.OBJ \
                                HELP1.RES \
                                HELP1.HLP
        LINK386 $(LINKOPTS) @<<
HELP1
HELP1
HELP1
OS2386
HELP1
<<
        RC HELP1.RES HELP1.EXE

HELP1.OBJ:                      HELP1.C \
                                HELP1.H
        ICC $(ICCOPTS) HELP1.C

HELP1.RES:                      HELP1.RC \
                                HELP1.H
        RC -r HELP1.RC HELP1.RES

HELP1.HLP:                      HELP1.IPF
        IPFC HELP1.IPF
