ALL:    CLIENT.EXE SERVER.EXE

CLIENT.EXE:                       CLIENT.OBJ
        LINK386  @<<
CLIENT
CLIENT
CLIENT
OS2386
CLIENT
<<

SERVER.EXE:                       SERVER.OBJ
        LINK386  @<<
SERVER
SERVER
SERVER
OS2386
SERVER
<<


CLIENT.OBJ:                       CLIENT.C
        ICC -C+ -Gm+ -Kb+ -Sm -Ss+ -Q CLIENT.C

SERVER.OBJ:                       SERVER.C
        ICC -C+ -Gm+ -Kb+ -Sm -Ss+ -Q SERVER.C
