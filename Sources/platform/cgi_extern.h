#ifndef cgi_aid_h
#define cgi_aid_h

#include "../cstring/cstring.h"
#include "../cvector/cvector.h"

#ifdef ANSI
extern char **environ;
#endif

extern int error_raised;
CString int_to_str(int i);
CString float_to_str(float i);
void make_directory(const CString& dir);
void movefile(const CString& source, const CString& target);
void cgi_error(cgiOutStream& CGIStream, const CString&);
void cgi_moved(cgiOutStream& CGIStream, const CString& link);
void common_exit(cgiOutStream& CGIStream);
char * protect(char * iStr);
char * DayEnglish(int);
char * MonthEnglish(int Month);
int DayOfWeek (int year, int month, int day);

#ifdef linux
int max(int first, int second);
#endif

#endif
