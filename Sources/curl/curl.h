/*
  CUrl - URL implementation
  =========================
  [RFC1738] 
  "Uniform Resource Locators", T. Berners-Lee, L. Masinter, and M. McCahill, December 1994.
  Available at http://ds.internic.net/rfc/rfc1738.txt. 
  http://www.faqs.org/rfcs/rfc1738.html
  
  (c) Daniel Doubrovkine - 1998 (dblock@vestris.com)

  + implemented full general syntax analysis and particular schemes
  (note: for unimplemented schemes, Valid is set to 1 but StrError says "unimplemented scheme"
  + HTTP
  + FTP  
  */

#ifndef C_URL_H
#define C_URL_H

#include "../cstring/cstring.h"

class CUrl {
private:
  char workbuf[24];
  CString UBrute;
  
  CString UScheme;
  CString USpecific;
  CString UUser;
  CString UPassword;
  CString UPort;
  CString UHost;
  CString UUrlPath;  
  CVector<CString> UHostVector;
  CVector<CString> UFtpPath;
  char UFtpPathType;

  CString UHttpPath;
  CString UHttpSearchPath;
  CString UHttpArg;
  CString UHttpSig;

  CString UHttpDirectory;
  CString UHttpFilename;

  int Valid;
  CString ReadScheme(int& curPos, const CString& iUrl);
  CString Escape(const CString&);
  CString UnEscape(const CString&) const;
  char * MakeC2X(char c);
  char X2C(char * what) const;
  void ParseUrl(const CString&);
  int ParseUrlSpecific(int&, const CString&);
  int ParseHost(const CString&);
  CString StrError;
  int ParseFtpPath(const CString&);
  int ParseHttpPath(const CString&);
  CString ResolveDirectory(CString OtherUHttpDirectory) const;
public:
  CUrl(void);
  CUrl(const CUrl&);
  ~CUrl(void);
  CUrl(const CString&);
  void SetUrl(const CString&);
  CUrl& operator=(const CUrl&);
  int operator==(const CUrl&);
  inline int isValid(void) const { return Valid; }
  inline CString GetBrute(void) const { return UBrute; }
  inline CString GetScheme(void) const { return UScheme; }
  inline CString GetSpecific(void) const { return USpecific; }
  inline CString GetUser(void) const { return UnEscape(UUser); }
  inline CString GetPassword(void) const { return UnEscape(UPassword); }
  inline CString GetPort(void) const { return UnEscape(UPort); }
  inline int GetPortValue(void) const { return UnEscape(UPort).Val(); }
  inline CString GetHost(void) const { return UnEscape(UHost); }
  CString GetUrlPath(void) const;
  inline char GetFtpPathType(void) const { return UFtpPathType; }
  inline CVector<CString> GetHostVector(void) const { return UHostVector; }
  inline CVector<CString> GetFtpPath(void) const { return UFtpPath; }
  inline CString GetHttpPath(void) const { return UHttpPath; }
  inline CString GetHttpSearchPath(void) const { return UHttpSearchPath; }
  inline CString GetHttpArg(void) const { return UHttpArg; }
  inline CString GetHttpSig(void) const { return UHttpSig; }
  inline CString GetHttpDirectory(void) const { return UHttpDirectory; }
  inline CString GetHttpFilename(void) const { return UHttpFilename; }
  CUrl Resolve(const CString&) const;
  CUrl Resolve(const CUrl& iUrl) const;
  CString GetHttpAll(void) const;
  inline void SetBrute(const CString& Value) { UBrute = Value; }
  inline void SetScheme(const CString& Value) { UScheme = Value; }
  inline void SetSpecific(const CString& Value) { USpecific = Value; }
  inline void SetUser(const CString& Value) { UUser = Escape(Value); }
  inline void SetPassword(const CString& Value) { UPassword = Escape(Value); }
  inline void SetPort(const CString& Value) { UPort = Escape(Value); }
  inline void SetPort(int Value) { UPort.Free(); UPort += Value; }
  inline void SetHost(const CString& Value) { UHost = Escape(Value); }
  inline void SetFtpPathType(char Value) { UFtpPathType = Value; }
  inline void SetHttpPath(const CString& Value) { UHttpPath = Value; }
  inline void SetHttpSearchPath(const CString& SearchPath) { UHttpSearchPath = SearchPath; }
  inline void SetHttpArg(const CString& Value) { UHttpArg = Value; }
  inline void SetHttpSig(const CString& Value) { UHttpSig = Value; }
  inline void SetHttpDirectory(const CString& Value) { UHttpDirectory = Value; }
  inline void SetHttpFilename(const CString& Filename) { UHttpFilename = Filename; }  
  inline void SetHostVector(const CVector<CString>& Vector) { UHostVector = Vector; }
  inline void SetFtpPath(const CVector<CString>& Vector) { UFtpPath = Vector; }
  int Includes(const CUrl& iUrl) const;
};

#endif
