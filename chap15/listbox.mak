# listbox.mak
# Created by IBM WorkFrame/2 MakeMake at 21:18:58 on 18 May 1997
#
# The actions included in this make file are:
#  Compile::Resource Compiler
#  Compile::C++ Compiler
#  Link::Linker
#  Bind::Resource Bind

.SUFFIXES: .c .obj .rc .res 

.all: \
    .\listbox.exe

.rc.res:
    @echo " Compile::Resource Compiler "
    rc.exe -r %s %|dpfF.RES

{E:\ARTOFOS2\CHAP15\LISTBOX}.rc.res:
    @echo " Compile::Resource Compiler "
    rc.exe -r %s %|dpfF.RES

.c.obj:
    @echo " Compile::C++ Compiler "
    icc.exe /Ti /C %s

{E:\ARTOFOS2\CHAP15\LISTBOX}.c.obj:
    @echo " Compile::C++ Compiler "
    icc.exe /Ti /C %s

.\listbox.exe: \
    .\listbox.obj \
    .\listbox.res
    @echo " Link::Linker "
    @echo " Bind::Resource Bind "
    icc.exe @<<
     /B" /de /pmtype:pm"
     /Felistbox.exe 
     .\listbox.obj
<<
    rc.exe .\listbox.res listbox.exe

.\listbox.res: \
    E:\ARTOFOS2\CHAP15\LISTBOX\listbox.rc \
    {E:\ARTOFOS2\CHAP15\LISTBOX;$(INCLUDE)}listbox.h

.\listbox.obj: \
    E:\ARTOFOS2\CHAP15\LISTBOX\listbox.c \
    {E:\ARTOFOS2\CHAP15\LISTBOX;$(INCLUDE);}listbox.h
