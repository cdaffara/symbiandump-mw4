# Microsoft Developer Studio Generated NMAKE File, Based on CdlCompilerToolkit.dsp
!IF "$(CFG)" == ""
CFG=CdlCompilerToolkit - Win32 Debug
!MESSAGE No configuration specified. Defaulting to CdlCompilerToolkit - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "CdlCompilerToolkit - Win32 Release" && "$(CFG)" != "CdlCompilerToolkit - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CdlCompilerToolkit.mak" CFG="CdlCompilerToolkit - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CdlCompilerToolkit - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "CdlCompilerToolkit - Win32 Debug" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "CdlCompilerToolkit - Win32 Release"

OUTDIR=\epoc32\BUILD\S60\CDL\CDLCOMPILERTOOLKIT\GROUP\CDLCOMPILERTOOLKIT\TOOLS\REL
INTDIR=\epoc32\BUILD\S60\CDL\CDLCOMPILERTOOLKIT\GROUP\CDLCOMPILERTOOLKIT\TOOLS\REL
# Begin Custom Macros
OutDir=\epoc32\BUILD\S60\CDL\CDLCOMPILERTOOLKIT\GROUP\CDLCOMPILERTOOLKIT\TOOLS\REL
# End Custom Macros

ALL : "..\..\..\epoc32\release\tools\rel\CdlCompilerToolkit.lib" "$(OUTDIR)\CdlCompilerToolkit.bsc"


CLEAN :
	-@erase "$(INTDIR)\CdlTkClientHeader.obj"
	-@erase "$(INTDIR)\CdlTkClientHeader.sbr"
	-@erase "$(INTDIR)\CdlTkDll.obj"
	-@erase "$(INTDIR)\CdlTkDll.sbr"
	-@erase "$(INTDIR)\CdlTkInstance.obj"
	-@erase "$(INTDIR)\CdlTkInstance.sbr"
	-@erase "$(INTDIR)\CdlTkInterface.obj"
	-@erase "$(INTDIR)\CdlTkInterface.sbr"
	-@erase "$(INTDIR)\CdlTkInterfaceCheck.obj"
	-@erase "$(INTDIR)\CdlTkInterfaceCheck.sbr"
	-@erase "$(INTDIR)\CdlTkPackage.obj"
	-@erase "$(INTDIR)\CdlTkPackage.sbr"
	-@erase "$(INTDIR)\CdlTkParser.obj"
	-@erase "$(INTDIR)\CdlTkParser.sbr"
	-@erase "$(INTDIR)\CdlTkProcess.obj"
	-@erase "$(INTDIR)\CdlTkProcess.sbr"
	-@erase "$(INTDIR)\CdlTkScript.obj"
	-@erase "$(INTDIR)\CdlTkScript.sbr"
	-@erase "$(INTDIR)\CdlTkSourceFileWriter.obj"
	-@erase "$(INTDIR)\CdlTkSourceFileWriter.sbr"
	-@erase "$(INTDIR)\CdlTkSyntaxCheck.obj"
	-@erase "$(INTDIR)\CdlTkSyntaxCheck.sbr"
	-@erase "$(INTDIR)\CdlTkUtil.obj"
	-@erase "$(INTDIR)\CdlTkUtil.sbr"
	-@erase "$(INTDIR)\CdlTkWriteCdlFile.obj"
	-@erase "$(INTDIR)\CdlTkWriteCdlFile.sbr"
	-@erase "$(INTDIR)\CdlTkWriteDll.obj"
	-@erase "$(INTDIR)\CdlTkWriteDll.sbr"
	-@erase "$(INTDIR)\CdlTkWriteInstance.obj"
	-@erase "$(INTDIR)\CdlTkWriteInstance.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\CdlCompilerToolkit.bsc"
	-@erase "..\..\..\epoc32\release\tools\rel\CdlCompilerToolkit.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "." /I "..\inc" /I "..\..\inc" /I "\epoc32\include" /I "\epoc32\include\CdlCompilerToolkit" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\CdlCompilerToolkit.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\CdlTkClientHeader.sbr" \
	"$(INTDIR)\CdlTkDll.sbr" \
	"$(INTDIR)\CdlTkInstance.sbr" \
	"$(INTDIR)\CdlTkInterface.sbr" \
	"$(INTDIR)\CdlTkInterfaceCheck.sbr" \
	"$(INTDIR)\CdlTkPackage.sbr" \
	"$(INTDIR)\CdlTkParser.sbr" \
	"$(INTDIR)\CdlTkProcess.sbr" \
	"$(INTDIR)\CdlTkSourceFileWriter.sbr" \
	"$(INTDIR)\CdlTkSyntaxCheck.sbr" \
	"$(INTDIR)\CdlTkUtil.sbr" \
	"$(INTDIR)\CdlTkWriteCdlFile.sbr" \
	"$(INTDIR)\CdlTkWriteDll.sbr" \
	"$(INTDIR)\CdlTkWriteInstance.sbr" \
	"$(INTDIR)\CdlTkScript.sbr"

"$(OUTDIR)\CdlCompilerToolkit.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"\epoc32\release\tools\rel\CdlCompilerToolkit.lib" 
LIB32_OBJS= \
	"$(INTDIR)\CdlTkClientHeader.obj" \
	"$(INTDIR)\CdlTkDll.obj" \
	"$(INTDIR)\CdlTkInstance.obj" \
	"$(INTDIR)\CdlTkInterface.obj" \
	"$(INTDIR)\CdlTkInterfaceCheck.obj" \
	"$(INTDIR)\CdlTkPackage.obj" \
	"$(INTDIR)\CdlTkParser.obj" \
	"$(INTDIR)\CdlTkProcess.obj" \
	"$(INTDIR)\CdlTkSourceFileWriter.obj" \
	"$(INTDIR)\CdlTkSyntaxCheck.obj" \
	"$(INTDIR)\CdlTkUtil.obj" \
	"$(INTDIR)\CdlTkWriteCdlFile.obj" \
	"$(INTDIR)\CdlTkWriteDll.obj" \
	"$(INTDIR)\CdlTkWriteInstance.obj" \
	"$(INTDIR)\CdlTkScript.obj"

"..\..\..\epoc32\release\tools\rel\CdlCompilerToolkit.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
   if not exist \epoc32\tools\lib\. md \epoc32\tools\lib
	 $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

SOURCE="$(InputPath)"

!ELSEIF  "$(CFG)" == "CdlCompilerToolkit - Win32 Debug"

OUTDIR=\epoc32\BUILD\S60\CDL\CDLCOMPILERTOOLKIT\GROUP\CDLCOMPILERTOOLKIT\TOOLS\DEB
INTDIR=\epoc32\BUILD\S60\CDL\CDLCOMPILERTOOLKIT\GROUP\CDLCOMPILERTOOLKIT\TOOLS\DEB

ALL : "..\..\..\epoc32\release\tools\deb\CdlCompilerToolkit.lib"


CLEAN :
	-@erase "$(INTDIR)\CdlTkClientHeader.obj"
	-@erase "$(INTDIR)\CdlTkDll.obj"
	-@erase "$(INTDIR)\CdlTkInstance.obj"
	-@erase "$(INTDIR)\CdlTkInterface.obj"
	-@erase "$(INTDIR)\CdlTkInterfaceCheck.obj"
	-@erase "$(INTDIR)\CdlTkPackage.obj"
	-@erase "$(INTDIR)\CdlTkParser.obj"
	-@erase "$(INTDIR)\CdlTkProcess.obj"
	-@erase "$(INTDIR)\CdlTkScript.obj"
	-@erase "$(INTDIR)\CdlTkSourceFileWriter.obj"
	-@erase "$(INTDIR)\CdlTkSyntaxCheck.obj"
	-@erase "$(INTDIR)\CdlTkUtil.obj"
	-@erase "$(INTDIR)\CdlTkWriteCdlFile.obj"
	-@erase "$(INTDIR)\CdlTkWriteDll.obj"
	-@erase "$(INTDIR)\CdlTkWriteInstance.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "..\..\..\epoc32\release\tools\deb\CdlCompilerToolkit.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "." /I "..\inc" /I "..\..\inc" /I "\epoc32\include" /I "\epoc32\include\CdlCompilerToolkit" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\CdlCompilerToolkit.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"\epoc32\release\tools\deb\CdlCompilerToolkit.lib" 
LIB32_OBJS= \
	"$(INTDIR)\CdlTkClientHeader.obj" \
	"$(INTDIR)\CdlTkDll.obj" \
	"$(INTDIR)\CdlTkInstance.obj" \
	"$(INTDIR)\CdlTkInterface.obj" \
	"$(INTDIR)\CdlTkInterfaceCheck.obj" \
	"$(INTDIR)\CdlTkPackage.obj" \
	"$(INTDIR)\CdlTkParser.obj" \
	"$(INTDIR)\CdlTkProcess.obj" \
	"$(INTDIR)\CdlTkSourceFileWriter.obj" \
	"$(INTDIR)\CdlTkSyntaxCheck.obj" \
	"$(INTDIR)\CdlTkUtil.obj" \
	"$(INTDIR)\CdlTkWriteCdlFile.obj" \
	"$(INTDIR)\CdlTkWriteDll.obj" \
	"$(INTDIR)\CdlTkWriteInstance.obj" \
	"$(INTDIR)\CdlTkScript.obj"

"..\..\..\epoc32\release\tools\deb\CdlCompilerToolkit.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
   if not exist \epoc32\tools\lib\. md \epoc32\tools\lib
	 $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

SOURCE="$(InputPath)"

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
!IF EXISTS("CdlCompilerToolkit.dep")
!INCLUDE "CdlCompilerToolkit.dep"
!ELSE 
!MESSAGE Warning: cannot find "CdlCompilerToolkit.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "CdlCompilerToolkit - Win32 Release" || "$(CFG)" == "CdlCompilerToolkit - Win32 Debug"
SOURCE=..\src\CdlTkClientHeader.cpp

!IF  "$(CFG)" == "CdlCompilerToolkit - Win32 Release"


"$(INTDIR)\CdlTkClientHeader.obj"	"$(INTDIR)\CdlTkClientHeader.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "CdlCompilerToolkit - Win32 Debug"


"$(INTDIR)\CdlTkClientHeader.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\CdlTkDll.cpp

!IF  "$(CFG)" == "CdlCompilerToolkit - Win32 Release"


"$(INTDIR)\CdlTkDll.obj"	"$(INTDIR)\CdlTkDll.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "CdlCompilerToolkit - Win32 Debug"


"$(INTDIR)\CdlTkDll.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\CdlTkInstance.cpp

!IF  "$(CFG)" == "CdlCompilerToolkit - Win32 Release"


"$(INTDIR)\CdlTkInstance.obj"	"$(INTDIR)\CdlTkInstance.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "CdlCompilerToolkit - Win32 Debug"


"$(INTDIR)\CdlTkInstance.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\CdlTkInterface.cpp

!IF  "$(CFG)" == "CdlCompilerToolkit - Win32 Release"


"$(INTDIR)\CdlTkInterface.obj"	"$(INTDIR)\CdlTkInterface.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "CdlCompilerToolkit - Win32 Debug"


"$(INTDIR)\CdlTkInterface.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\CdlTkInterfaceCheck.cpp

!IF  "$(CFG)" == "CdlCompilerToolkit - Win32 Release"


"$(INTDIR)\CdlTkInterfaceCheck.obj"	"$(INTDIR)\CdlTkInterfaceCheck.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "CdlCompilerToolkit - Win32 Debug"


"$(INTDIR)\CdlTkInterfaceCheck.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\CdlTkPackage.cpp

!IF  "$(CFG)" == "CdlCompilerToolkit - Win32 Release"


"$(INTDIR)\CdlTkPackage.obj"	"$(INTDIR)\CdlTkPackage.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "CdlCompilerToolkit - Win32 Debug"


"$(INTDIR)\CdlTkPackage.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\CdlTkParser.cpp

!IF  "$(CFG)" == "CdlCompilerToolkit - Win32 Release"


"$(INTDIR)\CdlTkParser.obj"	"$(INTDIR)\CdlTkParser.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "CdlCompilerToolkit - Win32 Debug"


"$(INTDIR)\CdlTkParser.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\CdlTkProcess.cpp

!IF  "$(CFG)" == "CdlCompilerToolkit - Win32 Release"


"$(INTDIR)\CdlTkProcess.obj"	"$(INTDIR)\CdlTkProcess.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "CdlCompilerToolkit - Win32 Debug"


"$(INTDIR)\CdlTkProcess.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\CdlTkScript.cpp

!IF  "$(CFG)" == "CdlCompilerToolkit - Win32 Release"


"$(INTDIR)\CdlTkScript.obj"	"$(INTDIR)\CdlTkScript.sbr" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "CdlCompilerToolkit - Win32 Debug"


"$(INTDIR)\CdlTkScript.obj" : $(SOURCE) "$(INTDIR)"


!ENDIF 

SOURCE=..\src\CdlTkSourceFileWriter.cpp

!IF  "$(CFG)" == "CdlCompilerToolkit - Win32 Release"


"$(INTDIR)\CdlTkSourceFileWriter.obj"	"$(INTDIR)\CdlTkSourceFileWriter.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "CdlCompilerToolkit - Win32 Debug"


"$(INTDIR)\CdlTkSourceFileWriter.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\CdlTkSyntaxCheck.cpp

!IF  "$(CFG)" == "CdlCompilerToolkit - Win32 Release"


"$(INTDIR)\CdlTkSyntaxCheck.obj"	"$(INTDIR)\CdlTkSyntaxCheck.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "CdlCompilerToolkit - Win32 Debug"


"$(INTDIR)\CdlTkSyntaxCheck.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\CdlTkUtil.cpp

!IF  "$(CFG)" == "CdlCompilerToolkit - Win32 Release"


"$(INTDIR)\CdlTkUtil.obj"	"$(INTDIR)\CdlTkUtil.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "CdlCompilerToolkit - Win32 Debug"


"$(INTDIR)\CdlTkUtil.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\CdlTkWriteCdlFile.cpp

!IF  "$(CFG)" == "CdlCompilerToolkit - Win32 Release"


"$(INTDIR)\CdlTkWriteCdlFile.obj"	"$(INTDIR)\CdlTkWriteCdlFile.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "CdlCompilerToolkit - Win32 Debug"


"$(INTDIR)\CdlTkWriteCdlFile.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\CdlTkWriteDll.cpp

!IF  "$(CFG)" == "CdlCompilerToolkit - Win32 Release"


"$(INTDIR)\CdlTkWriteDll.obj"	"$(INTDIR)\CdlTkWriteDll.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "CdlCompilerToolkit - Win32 Debug"


"$(INTDIR)\CdlTkWriteDll.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\src\CdlTkWriteInstance.cpp

!IF  "$(CFG)" == "CdlCompilerToolkit - Win32 Release"


"$(INTDIR)\CdlTkWriteInstance.obj"	"$(INTDIR)\CdlTkWriteInstance.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "CdlCompilerToolkit - Win32 Debug"


"$(INTDIR)\CdlTkWriteInstance.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

