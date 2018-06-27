POPUP.EXE:                      POPUP.OBJ \
                                POPUP.RES
        LINK386 @<<
POPUP
POPUP
POPUP
OS2386
POPUP
<<
        RC POPUP.RES POPUP.EXE

POPUP.RES:                      POPUP.RC \
                                POPUP.H
        RC -r POPUP.RC POPUP.RES

POPUP.OBJ:                      POPUP.C \
                                POPUP.H
        ICC -C+ -Kb+ -Ss+ POPUP.C
