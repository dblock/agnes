#ifndef pParam_v2_h
#define pParam_v2_h

#include "../cgi_entry/cgi_entry.h"
#include "../cstring/cstring.h"
#include "../cvector/cvector.h"

class pParam_v2 {
private:  
  int method_post;
  cgi_entry_manager entry_manager; 
  void get_entries(void);   
  CString RawData;
  CString ReadStdin(int Size);
public:  
  CString getRawData(void) const;
  void create_cmdl(void);
  CString cmdl;
  CVector<CString> argv;
  CVector<CString> extra_params;
  int isPost(void);
  pParam_v2(int i_argc, char * i_argv[]);
  pParam_v2(const CVector<CString>& args, const CVector<CString>& params);
  ~pParam_v2(void);
  void add_entry(const CString& name, const CString& value);
  void set_entry(const CString& name, const CString& value);
  CString get_entry(const CString& name);
  CString get_entry_list(const CString& name);
  CString get_entry(int index);
  int entries_count(void);
  void entries_clear(void);
  CString get_name(int index);  
  CString c_params(void);
};

#endif
