/*
  CRemoteFile - HTTP/FTP
  */

#include <agnespch.hpp>

CRemoteFile::CRemoteFile(void){

}

int CRemoteFile::SetTimeout(int Value){
  return HTTPRequest.SetTimeout(Value);
}

CRemoteFile::CRemoteFile(const CString& iFile){
  SetFile(iFile);  
}

int CRemoteFile::SetLimit(int iSize){
  return HTTPRequest.SetLimit(iSize);
}

void CRemoteFile::SetURL(const CUrl& i_Url){
  Url = i_Url;
  HTTPRequest.ClearParameters();
}

void CRemoteFile::SetFile(const CString& iFile){
  Url.SetUrl(iFile);
  HTTPRequest.ClearParameters();
}

void CRemoteFile::SetParameter(const CString& Name, const CString& Value){
  HTTPRequest.SetParameter(Name, Value);
}

int CRemoteFile::Get(const CString& Proxy){  
  if (Proxy.StrLength()) HTTPRequest.SetProxy(Proxy);
  HTTPRequest.Execute(Url);
  return 1;
}

int CRemoteFile::Get(CVector<CString>& Urls){
  return Get("", Urls);
}

int CRemoteFile::Get(const CString& Proxy, CVector<CString>& Urls){
  if (Proxy.StrLength()) HTTPRequest.SetProxy(Proxy);
  HTTPRequest.Execute(Url);
  Urls = HTTPRequest.GetRedirectVector();
  return 1;
}

int CRemoteFile::Get(void){
  return Get("");
}

int CRemoteFile::Put(const CString& Data){
  /*
    shall be available for FTP and HTTP upload, not implemented yet
    */
  Contents = Data;
  return 0;
}

CString CRemoteFile::GetData(void) const {
  return HTTPRequest.GetData();
}

CString CRemoteFile::GetStatus(void) const {
  return HTTPRequest.GetStatus();
}

int CRemoteFile::GetStatusValue(void) const {
  return HTTPRequest.GetStatusValue();
}

CString CRemoteFile::GetVersion(void) const {
  return HTTPRequest.GetVersion();
}

CString CRemoteFile::GetValue(const CString& Name) const {
  return HTTPRequest.GetValue(Name);
}

int CRemoteFile::GetDataSize(void) const {
  return HTTPRequest.GetDataSize();
}

CString CRemoteFile::GetBrute(void) const { return Url.GetBrute(); }
CString CRemoteFile::GetScheme(void) const { return Url.GetScheme(); }
CString CRemoteFile::GetSpecific(void) const { return Url.GetSpecific(); }
CString CRemoteFile::GetUser(void) const { return Url.GetUser(); }
CString CRemoteFile::GetPassword(void) const { return Url.GetPassword(); }
CString CRemoteFile::GetPort(void) const { return Url.GetPort(); }
int CRemoteFile::GetPortValue(void) const { return Url.GetPortValue(); }
CString CRemoteFile::GetHost(void) const { return Url.GetHost(); }
CString CRemoteFile::GetUrlPath(void) const { return Url.GetUrlPath(); }
char CRemoteFile::GetFtpPathType(void) const { return Url.GetFtpPathType(); }
CVector<CString> CRemoteFile::GetHostVector(void) const { return Url.GetHostVector(); }
CVector<CString> CRemoteFile::GetFtpPath(void) const { return Url.GetFtpPath(); }
CString CRemoteFile::GetHttpPath(void) const { return Url.GetHttpPath(); }
CString CRemoteFile::GetHttpSearchPath(void) const { return Url.GetHttpSearchPath(); }
CString CRemoteFile::GetHttpArg(void) const { return Url.GetHttpArg(); }
CString CRemoteFile::GetHttpSig(void) const { return Url.GetHttpSig(); }
CString CRemoteFile::GetHttpDirectory(void) const { return Url.GetHttpDirectory(); };
CString CRemoteFile::GetHttpFilename(void) const { return Url.GetHttpFilename(); };
