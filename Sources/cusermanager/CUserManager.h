/*
  aGNeS User Manager
  */

#ifndef C_USER_MANAGER
#define C_USER_MANAGER

#include <html_form/html_classes.h>
#include <platform/platform.h>
#include <platform/cgi_aid.h>
#include <mozilla/mozilla_v2.h>
#include <cgi_equiv/cgi_equiv.h>
#include <cgi_access/cgi_access.h>
#include <ext_param/ext_param.h>
#include <CCookieManager/CCookieManager.h>

typedef CVector<CString> CStringVector;

class CUserManager : private ext_param {
 public:
  CUserManager(const CString&);
  ~CUserManager(void);
  CString GetDirectory(void) const;
  int GetUsermode(void);
  CString GetUsername(void);
  CString GetUserpass(void);
  int SetDirectory(const CString& Directory);  
  void Add(const CString&, const CString&);
  void Set(const CString&, const CString&);
  int Validate(const CString&, const CString&);
  void Show(cgiOutStream& CGIStream, pParam_v2&, cgi_equiv_manager&, const CString&);
  void Login(cgiOutStream& CGIStream, const CString&, CCookieManager& CookieManager, pParam_v2& Params);
  inline CString GetOption(const CString& Name) const { return ext_param :: get_value(Name); }
 private:
  int m_OptionsModified;
  int m_UsersModified;
  cgi_access_manager * m_AccessManager;
  CString m_UserFilePath;
  CString m_OptionsFilePath;
  CString m_Directory;
  CString m_OptionsFile;
  CString m_UserFile;
  void LoadOptions(void);
  void LoadUsers(void);
  void SaveOptions(void);
  void SaveUsers(void);
  void UpdateFilenames(void);
  void CreateUser(cgiOutStream& CGIStream, CString Name, CString Password, int = 0);
  void DeleteUser(cgiOutStream& CGIStream, CString);
};

#endif
