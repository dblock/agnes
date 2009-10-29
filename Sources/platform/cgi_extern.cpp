#include <agnespch.hpp>

CString exec_name;
CString init_dir;
CDate Today;

#include "../cvector/cvector.cpp"
#include "../slist/slist_v2.cpp"
#include "../chashtable/chashtable.cpp"

template class ilist<char *>;
template class ilist<int *>;
template class ilist<void *>;

template class CVector<CString>;
template class CVector<long>;
template class CVector<int>;
template class CVector<CStringVector>;
template class ICVector<CString>;
template class ICVector<long>;
template class ICVector<int>;
template class ICVector<CStringVector>;

template class CHashTable<CString>;
template class CHashTable<CEntryMultiple>;

template ostream& operator<<(ostream& out, const CVector<CString>& rhs);

#define lib_version "MV4 Lib v4.1"

#ifdef ANSI
  char ** _environ = environ;
#endif

int debug = 0;
int debug_cpp = 0;
int error_raised = 0;

char * DayEnglish(int Day){
  if (Day == 0) return "Monday";
  else if (Day == 1) return "Tuesday";
  else if (Day == 2) return "Wednesday";
  else if (Day == 3) return "Thursday";
  else if (Day == 4) return "Friday";
  else if (Day == 5) return "Saturday";
  else if (Day == 6) return "Sunday";
  else return "";
}

char * MonthEnglish(int Month){
  switch(Month) {
  case 0: return "January";
  case 1: return "February";
  case 2: return "March";
  case 3: return "April";
  case 4: return "May";
  case 5: return "June";
  case 6: return "July";
  case 7: return "August";
  case 8: return "September";
  case 9: return "October";
  case 10: return "November";
  case 11: return "December";
  default: return "";
  }
}

int DayOfWeek (int year, int month, int day){
  static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
  year -= month < 3;
  return (year + year/4 - year/100 + year/400 + t[month-1] + day)%7;
}

char * protect(char * iStr) {
  if (iStr) return iStr; else return "(void)";
}

void common_exit(cgiOutStream& CGIStream){  
  if (CGIStream.SocketOutput) {
    CGIStream << "\4\n";
    CGIStream.toSTALL();
  } else {
    CGIStream.flush();
    exit(0);
  }
}

#ifdef linux
int max(int first, int second){
  if (first > second) return(first); else return(second);
}
#endif

#include "../cgi_equiv/cgi_equiv.h"

void common_footer(cgiOutStream& CGIStream, int par){
    if (current_cgi_equiv_manager) {    
        if (current_cgi_equiv_manager->get_value(CGIStream, "NOLOGO", 0).StrLength() == 0)
            CGIStream << "<center><a href=\"http://www.vestris.com/bbs/en/\" target=\"_new\">"
                      << "<img src=\"http://www.vestris.com/images/vestris/powered.gif\" border=\"0\"></a></center>\n";
    }  
}

void cgi_moved(cgiOutStream& CGIStream, const CString& link){    
  cout << "Location: " << link << "\n\n";  
  exit(0);
}

void cgi_error(cgiOutStream& CGIStream, const CString& strerror) { 
  error_raised = 1;
  CGIStream << "<EM><FONT SIZE=4><B>" << strerror << "</EM></FONT>"
	    << "<BR><font size=-1>Please use the <EM>Back</EM> button to return to previous page.</font>\n";        
  common_footer(CGIStream, 1);
  common_exit(CGIStream);
}

void movefile(const CString& source, const CString& target){  
#ifdef ANSI
  char ch;
  ifstream infile;
  ofstream outfile;
  filebuf *inbuf, *outbuf;
  
  infile.open (source.asString());
  outfile.open (target.asString());
  
  inbuf = infile.rdbuf();
  outbuf = outfile.rdbuf();
  
  ch = inbuf->sgetc();
  while ( ch != EOF) {
    outbuf->sputc (ch);
    ch= inbuf->snextc();
  }
  
  outfile.close();
  infile.close();
  
  unlink(source.asString());
#else
  MoveFile(source.asString(), target.asString());
#endif
}

void make_directory(const CString& dir){
  CString aDir(dir);
#ifdef ANSI    
#ifdef linux
  aDir.remove_backslash();
#endif
  mkdir(aDir.asString(), S_IRWXU);	
#else
  mkdir(aDir.asString());
#endif	  
}

void cgi_init(cgiOutStream& CGIStream, int argc, char * argv[]){
  CGIStream.Lock();
  exec_name = argv[0];
  init_dir = getcwd(NULL, MAXPATHLEN);  
#ifdef ANSI
  ios::sync_with_stdio(0);
#endif
#ifdef _DEBUG
  for(int i=1;i<argc;i++)
    if(!strcmp(argv[i], "debug")) debug = 1;
#endif
  CGIStream << "Content-type: text/html" << (char) 10 << (char) 10;
}

CString int_to_str(int i) { 
  CString iVal; 
  iVal+=i; 
  return iVal; 
}

CString float_to_str(float i){
  char tstr[24]; sprintf(tstr, "%.2f", i);
  return tstr;
}
