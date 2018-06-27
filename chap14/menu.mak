MENU.EXE:                       MENU.OBJ \
                                MENU.RES
        LINK386 @<<
MENU
MENU
MENU
OS2386
MENU
<<
        RC MENU.RES MENU.EXE

MENU.RES:                       MENU.RC \
                                MENU.H
        RC -r MENU.RC MENU.RES

MENU.OBJ:                       MENU.C \
                                MENU.H
        ICC -C+ -Kb+ -Ss+ MENU.C
