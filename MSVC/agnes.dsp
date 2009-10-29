# Microsoft Developer Studio Project File - Name="agnes" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=agnes - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "agnes.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "agnes.mak" CFG="agnes - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "agnes - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "agnes - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "agnes - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\Sources" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"agnespch.hpp" /FD /c
# ADD BASE RSC /l 0x100c /d "NDEBUG"
# ADD RSC /l 0x100c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib wsock32.lib libci.lib libcimt.lib /nologo /subsystem:console /machine:I386 /out:"..\bin\Agnes.4.3\agnes.exe"

!ELSEIF  "$(CFG)" == "agnes - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\Sources" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x100c /d "_DEBUG"
# ADD RSC /l 0x100c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib wsock32.lib libci.lib libcimt.lib /nologo /subsystem:console /debug /machine:I386 /out:"..\bin\Agnes.4.3\agnes.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "agnes - Win32 Release"
# Name "agnes - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\Sources\agnes\agnes.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\agnespch.cpp
# ADD CPP /Yc"agnespch.hpp"
# End Source File
# Begin Source File

SOURCE=..\Sources\article_manager\article_archive.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\article_class\article_class.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\article_manager\article_generator.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\article_manager\article_manager.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\article_manager\article_post.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\agnes\bldver.rc
# End Source File
# Begin Source File

SOURCE=..\Sources\ccontainer\ccontainer.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\CCookieManager\CCookie.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\CCookieManager\CCookieManager.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\cdate\cdate.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\ccontainer\centrymultiple.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\cgi_access\cgi_access.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\cgi_admin\cgi_admin.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\cgi_entry\cgi_entry.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\cgi_equiv\cgi_equiv.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\platform\cgi_extern.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\cgiout\cgiout.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\chashtable\chashtable.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\chttprequest\chttprequest.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\cmail\cmail.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\cremotefile\cremotefile.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\cstring\cstring.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\curl\curl.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\cusermanager\CUserManager.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\cvector\cvector.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\inetsock\email.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\ext_param\ext_param.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\face_class\face_class.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\face_class\face_manager.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\html_base\html_base.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\html_form\html_tag.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\inetsock\inetsock.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\mhost\MHost.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\mozilla\mozilla_v2.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\nextgen\nextgen_v2.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\recent_class\recent_class.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\regexp\regexp.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\slist\slist.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\slist\slist_v2.cpp
# End Source File
# Begin Source File

SOURCE=..\Sources\html_form\tag_base.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\Sources\agnespch.hpp
# End Source File
# Begin Source File

SOURCE=..\Sources\article_manager\article_archive.h
# End Source File
# Begin Source File

SOURCE=..\Sources\article_class\article_class.h
# End Source File
# Begin Source File

SOURCE=..\Sources\article_manager\article_manager.h
# End Source File
# Begin Source File

SOURCE=..\Sources\agnes\bldver.hpp
# End Source File
# Begin Source File

SOURCE=..\Sources\ccontainer\ccontainer.h
# End Source File
# Begin Source File

SOURCE=..\Sources\CCookieManager\CCookie.h
# End Source File
# Begin Source File

SOURCE=..\Sources\CCookieManager\CCookieManager.h
# End Source File
# Begin Source File

SOURCE=..\Sources\cdate\cdate.h
# End Source File
# Begin Source File

SOURCE=..\Sources\ccontainer\centrymultiple.h
# End Source File
# Begin Source File

SOURCE=..\Sources\cgi_access\cgi_access.h
# End Source File
# Begin Source File

SOURCE=..\Sources\cgi_admin\cgi_admin.h
# End Source File
# Begin Source File

SOURCE=..\Sources\platform\cgi_aid.h
# End Source File
# Begin Source File

SOURCE=..\Sources\cgi_entry\cgi_entry.h
# End Source File
# Begin Source File

SOURCE=..\Sources\cgi_equiv\cgi_equiv.h
# End Source File
# Begin Source File

SOURCE=..\Sources\platform\cgi_extern.h
# End Source File
# Begin Source File

SOURCE=..\Sources\cgiout\cgiout.h
# End Source File
# Begin Source File

SOURCE=..\Sources\chashtable\chashtable.h
# End Source File
# Begin Source File

SOURCE=..\Sources\chttprequest\chttprequest.h
# End Source File
# Begin Source File

SOURCE=..\Sources\cmail\cmail.h
# End Source File
# Begin Source File

SOURCE=..\Sources\cremotefile\cremotefile.h
# End Source File
# Begin Source File

SOURCE=..\Sources\cstring\cstring.h
# End Source File
# Begin Source File

SOURCE=..\Sources\curl\curl.h
# End Source File
# Begin Source File

SOURCE=..\Sources\cusermanager\CUserManager.h
# End Source File
# Begin Source File

SOURCE=..\Sources\cvector\cvector.h
# End Source File
# Begin Source File

SOURCE=..\Sources\inetsock\email.h
# End Source File
# Begin Source File

SOURCE=..\Sources\base_entry\entry.h
# End Source File
# Begin Source File

SOURCE=..\Sources\ext_param\ext_param.h
# End Source File
# Begin Source File

SOURCE=..\Sources\face_class\face_class.h
# End Source File
# Begin Source File

SOURCE=..\Sources\face_class\face_manager.h
# End Source File
# Begin Source File

SOURCE=..\Sources\html_base\html_base.h
# End Source File
# Begin Source File

SOURCE=..\Sources\html_form\html_classes.h
# End Source File
# Begin Source File

SOURCE=..\Sources\inetsock\inetsock.h
# End Source File
# Begin Source File

SOURCE=..\Sources\mhost\MHost.h
# End Source File
# Begin Source File

SOURCE=..\Sources\mozilla\mozilla_v2.h
# End Source File
# Begin Source File

SOURCE=..\Sources\nextgen\nextgen_v2.h
# End Source File
# Begin Source File

SOURCE=..\Sources\platform\platform.h
# End Source File
# Begin Source File

SOURCE=..\Sources\recent_class\recent_class.h
# End Source File
# Begin Source File

SOURCE=..\Sources\regexp\regexp.h
# End Source File
# Begin Source File

SOURCE=..\Sources\slist\slist.h
# End Source File
# Begin Source File

SOURCE=..\Sources\slist\slist_v2.h
# End Source File
# Begin Source File

SOURCE=..\Sources\html_form\tag_base.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
