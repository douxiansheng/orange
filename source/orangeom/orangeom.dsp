# Microsoft Developer Studio Project File - Name="orangeom" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=orangeom - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "orangeom.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "orangeom.mak" CFG="orangeom - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "orangeom - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "orangeom - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "orangeom - Win32 Release_Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "orangeom - Win32 Python 24" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "orangeom - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "obj/Release"
# PROP Intermediate_Dir "obj/Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ORANGEOM_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "../orange" /I "../include" /I "px" /I "$(PYTHON)\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ORANGEOM_EXPORTS" /D "NO_PIPED_COMMANDS" /YX /FD /Zm700 /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 orange.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"obj/release/orangeom.pyd" /libpath:"../../lib" /libpath:"$(PYTHON)\libs"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=del ..\..\orangeom.pyd	"c:\program files\upx" "obj\release\orangeom.pyd" -o "..\..\orangeom.pyd"	copy obj\Release\orangeom.lib ..\..\lib\orangeom.lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "orangeom - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "obj/Debug"
# PROP Intermediate_Dir "obj/Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ORANGEOM_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "../orange" /I "../include" /I "px" /I "$(PYTHON)\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ORANGEOM_EXPORTS" /D "NO_PIPED_COMMANDS" /YX /FD /GZ /Zm700 /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 orange_d.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"..\..\orangeom_d.pyd" /pdbtype:sept /libpath:"$(PYTHON)\libs" /libpath:"../../lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy obj\Debug\orangeom_d.lib ..\..\lib\orangeom_d.lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "orangeom - Win32 Release_Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "orangeom___Win32_Release_Debug"
# PROP BASE Intermediate_Dir "orangeom___Win32_Release_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "obj/Release_Debug"
# PROP Intermediate_Dir "obj/Release_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GR /GX /O2 /I "../orange" /I "../include" /I "px" /I "$(PYTHON)\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ORANGEOM_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "../orange" /I "../include" /I "px" /I "$(PYTHON)\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ORANGEOM_EXPORTS" /D "NO_PIPED_COMMANDS" /YX /FD /Zm700 /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 orange.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"obj/release/orangeom.pyd" /libpath:"$(PYTHON)\libs" /libpath:"../../lib"
# ADD LINK32 orange.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:none /debug /machine:I386 /out:"obj/release_debug/orangeom.pyd" /libpath:"$(PYTHON)\libs" /libpath:"../../lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=del ..\..\orangeom.pyd	copy "obj\release_debug\orangeom.pyd" "..\..\orangeom.pyd"	copy obj\Release_debug\orangeom.lib ..\..\lib\orangeom.lib
# End Special Build Tool

!ELSEIF  "$(CFG)" == "orangeom - Win32 Python 24"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "orangeom___Win32_Python_24"
# PROP BASE Intermediate_Dir "orangeom___Win32_Python_24"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "obj/Release24"
# PROP Intermediate_Dir "obj/Release24"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GR /GX /O2 /I "../orange" /I "../include" /I "px" /I "$(PYTHON)\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ORANGEOM_EXPORTS" /D "NO_PIPED_COMMANDS" /YX /FD /Zm700 /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "../orange" /I "../include" /I "px" /I "$(PYTHON24)\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ORANGEOM_EXPORTS" /D "NO_PIPED_COMMANDS" /YX /FD /Zm700 /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 orange.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"obj/release/orangeom.pyd" /libpath:"$(PYTHON)\libs" /libpath:"../../lib"
# ADD LINK32 orange.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"obj/release24/orangeom.pyd" /libpath:"../../24/lib" /libpath:"$(PYTHON24)\libs"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=del ..\..\24\orangeom.pyd*	"c:\program files\upx" "obj\release24\orangeom.pyd" -o "..\..\24\orangeom.pyd"	copy obj\Release24\orangeom.lib ..\..\24\lib\orangeom.lib
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "orangeom - Win32 Release"
# Name "orangeom - Win32 Debug"
# Name "orangeom - Win32 Release_Debug"
# Name "orangeom - Win32 Python 24"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "wml"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\wml\WmlDelaunay2a.cpp
# End Source File
# Begin Source File

SOURCE=.\wml\WmlDelaunay2a.h
# End Source File
# Begin Source File

SOURCE=.\wml\WmlMath.cpp
# End Source File
# Begin Source File

SOURCE=.\wml\WmlMath.h
# End Source File
# Begin Source File

SOURCE=.\wml\WmlMath.inl
# End Source File
# Begin Source File

SOURCE=.\wml\WmlSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\wml\WmlSystem.h
# End Source File
# Begin Source File

SOURCE=.\wml\WmlSystem.inl
# End Source File
# Begin Source File

SOURCE=.\wml\WmlVector.h
# End Source File
# Begin Source File

SOURCE=.\wml\WmlVector.inl
# End Source File
# Begin Source File

SOURCE=.\wml\WmlVector2.cpp
# End Source File
# Begin Source File

SOURCE=.\wml\WmlVector2.h
# End Source File
# Begin Source File

SOURCE=.\wml\WmlVector2.inl
# End Source File
# Begin Source File

SOURCE=.\wml\WmlVector3.h
# End Source File
# Begin Source File

SOURCE=.\wml\WmlVector3.inl
# End Source File
# End Group
# Begin Group "som"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\som\datafile.c
# End Source File
# Begin Source File

SOURCE=.\som\fileio.c
# End Source File
# Begin Source File

SOURCE=.\som\labels.c
# End Source File
# Begin Source File

SOURCE=.\som\lvq_pak.c
# End Source File
# Begin Source File

SOURCE=.\som\som_rout.c
# End Source File
# Begin Source File

SOURCE=.\som\version.c
# End Source File
# End Group
# Begin Source File

SOURCE=.\graphDrawing.cpp
# End Source File
# Begin Source File

SOURCE=.\graphoptimization.cpp
# End Source File
# Begin Source File

SOURCE=.\mds.cpp

!IF  "$(CFG)" == "orangeom - Win32 Release"

# ADD CPP /D "NO_PIPED_COMANDS"

!ELSEIF  "$(CFG)" == "orangeom - Win32 Debug"

!ELSEIF  "$(CFG)" == "orangeom - Win32 Release_Debug"

!ELSEIF  "$(CFG)" == "orangeom - Win32 Python 24"

# ADD BASE CPP /D "NO_PIPED_COMANDS"
# ADD CPP /D "NO_PIPED_COMANDS"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\optimizeAnchors.cpp

!IF  "$(CFG)" == "orangeom - Win32 Release"

# ADD CPP /D "NO_PIPED_COMANDS"

!ELSEIF  "$(CFG)" == "orangeom - Win32 Debug"

!ELSEIF  "$(CFG)" == "orangeom - Win32 Release_Debug"

# ADD CPP /Zi /Od

!ELSEIF  "$(CFG)" == "orangeom - Win32 Python 24"

# ADD BASE CPP /D "NO_PIPED_COMANDS"
# ADD CPP /D "NO_PIPED_COMANDS"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\orangeom.cpp

!IF  "$(CFG)" == "orangeom - Win32 Release"

# ADD CPP /D "NO_PIPED_COMANDS"
# SUBTRACT CPP /nologo /YX

!ELSEIF  "$(CFG)" == "orangeom - Win32 Debug"

!ELSEIF  "$(CFG)" == "orangeom - Win32 Release_Debug"

# SUBTRACT BASE CPP /nologo /YX
# SUBTRACT CPP /nologo /YX

!ELSEIF  "$(CFG)" == "orangeom - Win32 Python 24"

# ADD BASE CPP /D "NO_PIPED_COMANDS"
# SUBTRACT BASE CPP /nologo /YX
# ADD CPP /D "NO_PIPED_COMANDS"
# SUBTRACT CPP /nologo /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\som.cpp
# End Source File
# Begin Source File

SOURCE=.\triangulate.cpp

!IF  "$(CFG)" == "orangeom - Win32 Release"

# ADD CPP /D "NO_PIPED_COMANDS"

!ELSEIF  "$(CFG)" == "orangeom - Win32 Debug"

!ELSEIF  "$(CFG)" == "orangeom - Win32 Release_Debug"

!ELSEIF  "$(CFG)" == "orangeom - Win32 Python 24"

# ADD BASE CPP /D "NO_PIPED_COMANDS"
# ADD CPP /D "NO_PIPED_COMANDS"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\graphoptimization.h
# End Source File
# Begin Source File

SOURCE=.\mds.hpp
# End Source File
# Begin Source File

SOURCE=.\som.hpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
