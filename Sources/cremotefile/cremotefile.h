/*
  CRemoteFile - HTTP/FTP
  */

#ifndef C_REMOTE_FILE_H
#define C_REMOTE_FILE_H

#include "../curl/curl.h"
#include "../chttprequest/chttprequest.h"

class CRemoteFile {
public:
  int SetTimeout(int);
  CHttpRequest HTTPRequest;
  CRemoteFile(void);
  CRemoteFile(const CString&);
  void SetFile(const CString&);  
  void SetURL(const CUrl&);
  void SetParameter(const CString& Name, const CString& Value);
  int Get(void);
  int Get(const CString& Proxy);
  int Get(const CString& Proxy, CVector<CString>& Urls);
  int Get(CVector<CString>& Urls);
  int Put(const CString&);
  CString GetData(void) const;
  CString GetStatus(void) const;
  int GetStatusValue(void) const;
  CString GetVersion(void) const;
  CString GetValue(const CString&) const;
  int GetDataSize(void) const;  
  CString GetBrute(void) const;
  CString GetScheme(void) const;
  CString GetSpecific(void) const;
  CString GetUser(void) const;
  CString GetPassword(void) const;
  CString GetPort(void) const;
  int GetPortValue(void) const;
  CString GetHost(void) const;
  CString GetUrlPath(void) const;
  char GetFtpPathType(void) const;
  CVector<CString> GetHostVector(void) const;
  CVector<CString> GetFtpPath(void) const;
  CString GetHttpPath(void) const;
  CString GetHttpSearchPath(void) const;
  CString GetHttpArg(void) const;
  CString GetHttpSig(void) const;
  int SetLimit(int);
  CString GetHttpDirectory(void) const;
  CString GetHttpFilename(void) const;
private:
  CString Contents;
  CUrl Url;  
};

#endif
