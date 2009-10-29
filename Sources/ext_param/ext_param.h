#ifndef ext_param_h
#define ext_param_h

#include <stdarg.h>
#include <cstring/cstring.h>
#include <cvector/cvector.h>
#include <base_entry/entry.h>
#include <cgi_equiv/cgi_equiv.h>
#include <cgi_access/cgi_access.h>
#include <cgi_admin/cgi_admin.h>
#include <mozilla/mozilla_v2.h>

class ext_param : public entry_manager {
public:
  void Reset(void);
  cgi_equiv_manager * equivs;
  pParam_v2 * params;
  void traverse_options(const CString& s);
  virtual void traverse_post(cgiOutStream&) {};
  virtual void traverse_cline(cgiOutStream&, const CString&) {};
  virtual int traverse_tags(cgiOutStream&, const CString&) {return 0;};
  virtual void traverse_after_options(cgiOutStream& ) {};
  void traverse_cline_internal(cgiOutStream&, const CString& arg);
  CString get_option(const CString& name);
  CString get_option(const CString& class_name, const CString& name);
  CString get_option(const CString& name, int index);
  ext_param(char * name, char * value, ...);
  virtual ~ext_param(void);	
};

#endif
