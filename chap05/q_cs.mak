ALL:    qCLIENT.EXE qSERVER.EXE

qserver.EXE:           qserver.OBJ
        LINK386 @<<
qserver
qserver
qserver
OS2386
qserver
<<

qserver.OBJ:                      qserver.C q_CS.MAK
        ICC -C+ -Gm+ -Kb+ -Sm -Ss+ -Q qserver.C

qclient.EXE:                      qclient.OBJ
        LINK386 @<<
qclient
qclient
qclient
OS2386
qclient
<<

qclient.OBJ:                      qclient.C q_CS.MAK
        ICC -C+ -Gm+ -Kb+ -Sm -Ss+ -Q qclient.C
