# Microsoft Developer Studio Generated NMAKE File, Based on ellf.dsp
!IF "$(CFG)" == ""
CFG=ellf - Win32 Debug
!MESSAGE No configuration specified. Defaulting to ellf - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ellf - Win32 Release" && "$(CFG)" != "ellf - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ellf.mak" CFG="ellf - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ellf - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "ellf - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ellf - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\ellf.exe"


CLEAN :
	-@erase "$(INTDIR)\cmplx.obj"
	-@erase "$(INTDIR)\const.obj"
	-@erase "$(INTDIR)\ellf.obj"
	-@erase "$(INTDIR)\ellie.obj"
	-@erase "$(INTDIR)\ellik.obj"
	-@erase "$(INTDIR)\ellpe.obj"
	-@erase "$(INTDIR)\ellpj.obj"
	-@erase "$(INTDIR)\ellpk.obj"
	-@erase "$(INTDIR)\mtherr.obj"
	-@erase "$(INTDIR)\polevl.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\ellf.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\ellf.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ellf.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\ellf.pdb" /machine:I386 /out:"$(OUTDIR)\ellf.exe" 
LINK32_OBJS= \
	"$(INTDIR)\cmplx.obj" \
	"$(INTDIR)\const.obj" \
	"$(INTDIR)\ellf.obj" \
	"$(INTDIR)\ellie.obj" \
	"$(INTDIR)\ellik.obj" \
	"$(INTDIR)\ellpe.obj" \
	"$(INTDIR)\ellpj.obj" \
	"$(INTDIR)\ellpk.obj" \
	"$(INTDIR)\mtherr.obj" \
	"$(INTDIR)\polevl.obj"

"$(OUTDIR)\ellf.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ellf - Win32 Debug"

OUTDIR=.
INTDIR=.
# Begin Custom Macros
OutDir=.
# End Custom Macros

ALL : "$(OUTDIR)\ellf.exe"


CLEAN :
	-@erase "$(INTDIR)\cmplx.obj"
	-@erase "$(INTDIR)\const.obj"
	-@erase "$(INTDIR)\ellf.obj"
	-@erase "$(INTDIR)\ellie.obj"
	-@erase "$(INTDIR)\ellik.obj"
	-@erase "$(INTDIR)\ellpe.obj"
	-@erase "$(INTDIR)\ellpj.obj"
	-@erase "$(INTDIR)\ellpk.obj"
	-@erase "$(INTDIR)\mtherr.obj"
	-@erase "$(INTDIR)\polevl.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ellf.exe"
	-@erase "$(OUTDIR)\ellf.ilk"
	-@erase "$(OUTDIR)\ellf.pdb"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\ellf.pch" /YX /FD /GZ  /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ellf.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\ellf.pdb" /debug /machine:I386 /out:"$(OUTDIR)\ellf.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\cmplx.obj" \
	"$(INTDIR)\const.obj" \
	"$(INTDIR)\ellf.obj" \
	"$(INTDIR)\ellie.obj" \
	"$(INTDIR)\ellik.obj" \
	"$(INTDIR)\ellpe.obj" \
	"$(INTDIR)\ellpj.obj" \
	"$(INTDIR)\ellpk.obj" \
	"$(INTDIR)\mtherr.obj" \
	"$(INTDIR)\polevl.obj"

"$(OUTDIR)\ellf.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("ellf.dep")
!INCLUDE "ellf.dep"
!ELSE 
!MESSAGE Warning: cannot find "ellf.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "ellf - Win32 Release" || "$(CFG)" == "ellf - Win32 Debug"
SOURCE=.\cmplx.c

"$(INTDIR)\cmplx.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\const.c

"$(INTDIR)\const.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ellf.c

"$(INTDIR)\ellf.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ellie.c

"$(INTDIR)\ellie.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ellik.c

"$(INTDIR)\ellik.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ellpe.c

"$(INTDIR)\ellpe.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ellpj.c

"$(INTDIR)\ellpj.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ellpk.c

"$(INTDIR)\ellpk.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\mtherr.c

"$(INTDIR)\mtherr.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\polevl.c

"$(INTDIR)\polevl.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

