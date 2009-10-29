#ifndef cgi_aid_h
#define cgi_aid_h

//  #define _DEBUG
//  #define _DEBUG_CPP
  
#ifdef _DEBUG
extern int debug;
#endif

#ifdef _DEBUF_CPP
extern int debug_cpp;
#endif

#include "../cstring/cstring.h"
#include "../cvector/cvector.h"

#ifdef ANSI
extern char **environ;
extern char ** _environ;
#else
#include <windows.h>
#endif

#include "../cgiout/cgiout.h"

extern CString exec_name;
extern CString init_dir;
extern int error_raised;
extern CString int_to_str(int i);
extern void make_directory(const CString& dir);
extern void movefile(const CString& source, const CString& target);
extern void cgi_error(cgiOutStream& CGIStream, const CString&);
extern void cgi_moved(cgiOutStream& CGIStream, const CString& link);
extern void common_exit(cgiOutStream& CGIStream);
extern char * protect(char * iStr);
extern int DayOfWeek (int year, int month, int day);
extern char * DayEnglish(int);
extern char * MonthEnglish(int Month);

#include <time.h>

extern void common_footer(cgiOutStream& CGIStream, int par);
extern void cgi_init(cgiOutStream& CGIStream, int argc, char * argv[]);

#endif
