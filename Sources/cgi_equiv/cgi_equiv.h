#ifndef cgi_equiv_h
#define cgi_equiv_h

#include <cgiout/cgiout.h>

extern void output_current_equivs(cgiOutStream& CGIStream);

#include <cstring/cstring.h>
#include <cvector/cvector.h>
#include <base_entry/entry.h>
#include <cgi_admin/cgi_admin.h>
#include <cgi_entry/cgi_entry.h>

class cgi_equiv_manager : public entry_manager {
private:  
  void add_equiv(const CString& source, const CString& target);
  void fill_equiv(void);
  cgi_admin_manager admin_manager;
  CString admin_class;
  CString equiv_file; 
public:
  cgi_entry_manager EAssocs;
  cgi_entry_manager EAlias;
  cgi_entry_manager EEmail;
  CVector<CString> EObvious;
  CVector<CString> Email;

  CVector<CString> make_asocs(void);
  void make_oce(void);
  void make_obvious(int nArticleAge = -1, int GenSelection = 0);

  int l_agnes;
  int l_guest;
  int l_forum;
  int l_links;

  void sort(void);
  cgi_equiv_manager(const CString& _class);
  cgi_equiv_manager(void);
  ~cgi_equiv_manager(void);
  inline const CString& get_admin_class(void) const { return admin_class; }
  inline cgi_admin_manager& get_admin_manager(void) { return admin_manager; }

  CString get_value(cgiOutStream&, const CString& name);
  CString get_value(cgiOutStream&, const CString& name, int fatal);
  CString get_value_admin(cgiOutStream&, const CString& name);
  void output_equivs(cgiOutStream&);
  int write_full_equiv(const CString& Name, const CString& EMail, const CVector<CString>& FirstSetNames, const CVector<CString>& FirstSetValues, const CVector<CString>& SecondSetNames, const CVector<CString>& SecondSetValues);  
  void set_value_oce(cgi_entry_manager& EAlias, const CString& source, CString target);
  void add_value_oce(cgi_entry_manager& EAlias, const CString& source, const CString& target, const CString EMail); 
  int write_email(const CString&, const CString&, const CString&);
  int write_template(const CString&, const CString&, const CString&);
  int replace_equiv(const CString& Name, const CString& Value, int Append);
  int delete_template(cgiOutStream& CGIStream, const CString& Alias);
};

extern cgi_equiv_manager * current_cgi_equiv_manager;

#endif


