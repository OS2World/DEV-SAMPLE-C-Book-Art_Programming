ALL:    32_TERM.EXE

32_TERM.EXE:                       32_TERM.OBJ
        LINK386 /NOI @<<
32_TERM
32_TERM
32_TERM
OS2386
32_TERM
<<


32_TERM.OBJ:                       32_TERM.C
        ICC -C+ -Gm+ -Kb+ -Sm -Ss+ 32_TERM.C

