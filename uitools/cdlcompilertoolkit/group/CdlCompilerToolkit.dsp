# Microsoft Developer Studio Project File - Name="CdlCompilerToolkit" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=CdlCompilerToolkit - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CdlCompilerToolkit.mak".
!MESSAGE 
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

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "CdlCompilerToolkit"
# PROP Scc_LocalPath ".."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CdlCompilerToolkit - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "\epoc32\BUILD\S60\CDL\CDLCOMPILERTOOLKIT\GROUP\CDLCOMPILERTOOLKIT\TOOLS\REL"
# PROP Intermediate_Dir "\epoc32\BUILD\S60\CDL\CDLCOMPILERTOOLKIT\GROUP\CDLCOMPILERTOOLKIT\TOOLS\REL"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "." /I "..\inc" /I "..\..\inc" /I "\epoc32\include" /I "\epoc32\include\CdlCompilerToolkit" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"\epoc32\release\tools\rel\CdlCompilerToolkit.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Cmds=if not exist \epoc32\tools\lib\. md \epoc32\tools\lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "CdlCompilerToolkit - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "\epoc32\BUILD\S60\CDL\CDLCOMPILERTOOLKIT\GROUP\CDLCOMPILERTOOLKIT\TOOLS\DEB"
# PROP Intermediate_Dir "\epoc32\BUILD\S60\CDL\CDLCOMPILERTOOLKIT\GROUP\CDLCOMPILERTOOLKIT\TOOLS\DEB"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "." /I "..\inc" /I "..\..\inc" /I "\epoc32\include" /I "\epoc32\include\CdlCompilerToolkit" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"\epoc32\release\tools\deb\CdlCompilerToolkit.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PreLink_Cmds=if not exist \epoc32\tools\lib\. md \epoc32\tools\lib
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "CdlCompilerToolkit - Win32 Release"
# Name "CdlCompilerToolkit - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\src\CdlTkClientHeader.cpp
# End Source File
# Begin Source File

SOURCE=..\src\CdlTkDll.cpp
# End Source File
# Begin Source File

SOURCE=..\src\CdlTkInstance.cpp
# End Source File
# Begin Source File

SOURCE=..\src\CdlTkInterface.cpp
# End Source File
# Begin Source File

SOURCE=..\src\CdlTkInterfaceCheck.cpp
# End Source File
# Begin Source File

SOURCE=..\src\CdlTkPackage.cpp
# End Source File
# Begin Source File

SOURCE=..\src\CdlTkParser.cpp
# End Source File
# Begin Source File

SOURCE=..\src\CdlTkProcess.cpp
# End Source File
# Begin Source File

SOURCE=..\src\CdlTkSourceFileWriter.cpp
# End Source File
# Begin Source File

SOURCE=..\src\CdlTkSyntaxCheck.cpp
# End Source File
# Begin Source File

SOURCE=..\src\CdlTkUtil.cpp
# End Source File
# Begin Source File

SOURCE=..\src\CdlTkWriteCdlFile.cpp
# End Source File
# Begin Source File

SOURCE=..\src\CdlTkWriteCommonDefs.cpp
# End Source File
# Begin Source File

SOURCE=..\src\CdlTkWriteDll.cpp
# End Source File
# Begin Source File

SOURCE=..\src\CdlTkWriteInstance.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\inc\CdlTkInstance.h
# End Source File
# Begin Source File

SOURCE=..\inc\CdlTkInterface.h
# End Source File
# Begin Source File

SOURCE=..\inc\CdlTkPriv.h
# End Source File
# Begin Source File

SOURCE=..\inc\CdlTkProcess.h
# End Source File
# Begin Source File

SOURCE=..\inc\CdlTkStdTrans.h
# End Source File
# Begin Source File

SOURCE=..\inc\CdlTkUtil.h
# End Source File
# End Group
# End Target
# End Project
