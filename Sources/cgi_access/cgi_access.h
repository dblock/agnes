#ifndef cgi_access_h
#define cgi_access_h

#include <cgiout/cgiout.h>

extern void output_current_access(cgiOutStream& CGIStream);

#include <cstring/cstring.h>
#include <base_entry/entry.h>

extern int check_root_access(const CString&);

class cgi_access_manager : public entry_manager {
private:
  CString equiv_file;
  void cgi_access_manager::add_access(const CString& source, const CString& target);
  void fill_access(void);
  void fill_access(const CString& container);
public:
  void output_access(cgiOutStream&);
  cgi_access_manager(const CString& container);
  cgi_access_manager(void);
  ~cgi_access_manager(void);
  CString get_value(cgiOutStream& CGIStream, const CString& name);
  CString get_value(cgiOutStream& CGIStream, const CString& name, int fatal);
  void check_access(cgiOutStream& CGIStream, const CString& what, const CString& id);
  int write_full_access(const CString& Equiv, const CString& Password);
  int cgi_access_write(void);
  int cgi_access_write(const CString&);
};

extern cgi_access_manager * current_cgi_access_manager;

#endif





