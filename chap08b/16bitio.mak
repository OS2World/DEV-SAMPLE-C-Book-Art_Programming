ALL:    16BITIO.DLL 16BITIO.LIB

16BITIO.LIB:                       16BITIO.DLL
        IMPLIB 16BITIO.LIB 16BITIO.DEF

16BITIO.DLL:                       16BITIO.OBJ
        LINK /CO /NOI @<<
16BITIO
16BITIO.DLL
16BITIO

16BITIO
<<


16BITIO.OBJ:                  16BITIO.C
        cl  -c -AL -G2s -Zi -Fc 16BITIO.C

