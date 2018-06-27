# chkea.mak
# Created by IBM WorkFrame/2 MakeMake at 12:04:14 on 9 Dec 1996
#
# The actions included in this make file are:
#  Compile::C++ Compiler
#  Link::Linker

.SUFFIXES: .c .obj 

.all: \
    .\chkea.exe

.c.obj:
    @echo " Compile::C++ Compiler "
    icc.exe /Ss /Tx /Ti /W2 /Tm /Ms /C %s

{E:\ARTOFOS2\CHAP4}.c.obj:
    @echo " Compile::C++ Compiler "
    icc.exe /Ss /Tx /Ti /W2 /Tm /Ms /C %s

.\chkea.exe: \
    .\chkea.obj
    @echo " Link::Linker "
    icc.exe @<<
     /B" /de"
     /Fechkea.exe 
     .\chkea.obj
<<

.\chkea.obj: \
    E:\ARTOFOS2\CHAP4\chkea.c
