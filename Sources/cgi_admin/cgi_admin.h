#ifndef cgi_admin_h
#define cgi_admin_h

#include <cstring/cstring.h>
#include <base_entry/entry.h>
#include <cgiout/cgiout.h>

void output_current_admins(cgiOutStream& CGIStream);

class cgi_admin_manager : public entry_manager {
private:
  CString equiv_file;
  void add_admin(const CString&, const CString&);
  void fill_admin(void);
public:
  void output_admins(cgiOutStream&);
  cgi_admin_manager(void);
  ~cgi_admin_manager(void);
  int admin(const CString&, const CString&);
  int write_full_admin(const CString& AdminClass, const CString& Equiv);
  int cgi_admin_write(void);
  int cgi_admin_write(const CString&);
};

extern cgi_admin_manager * current_cgi_admin_manager;

#endif
