/*
  CHttpRequest - (c) Daniel Doubrovkine
  handles old-style direct request recovery and HTTP 1.0/1.1 redirections
  */

#ifndef C_HTTP_REQUEST_H
#define C_HTTP_REQUEST_H

#define HTTPR_USER 1000

#include <platform/platform.h>

#ifdef ANSI
#include <sys/time.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include <inetsock/inetsock.h>
#include <curl/curl.h>
#include <cgi_entry/cgi_entry.h>
#include <fcntl.h>
#include <sys/types.h>

class CHttpRequest {
private:
  int FollowRedirections;
  fd_set fdset;
  struct timeval tv;
  CString RHeader;
  CString RData;
  CString ProxyURL;
  CString RStatus;
  int RStatusValue;
  int RTimeout;
  int RLimit;
  CString RVersion;
  cgi_entry_manager RHeaderParams;
  cgi_entry_manager RHeaderResponse;
  CUrl Proxy;
  void ProcessHeader(inetSocket& Sock);
  void ProcessData(inetSocket& Sock);
  CString ReadRemaining(inetSocket& Sock);
  CString ReadLine(inetSocket& Sock);
  void ProcessRStatus(const CString& Line);
  void ProcessOneLine(const CString& Line);
  int GetHTTP10(CUrl& iUrl, const CString&, inetSocket&);
  int GetHTTP09(CUrl& iUrl, const CString&, inetSocket&);
  int PostHTTP(CUrl& iUrl, const CString& RawData, const CString& iE, inetSocket& Sock);
  int Send(inetSocket& Socket, const CString& Request);
  CVector<CString> RedirectVector;
public:  
  int SetFollowRedirections(int Value);
  int GetFollowRedirections(void) const;
  static CString MapError(int Error);
  int SetTimeout(int);
  int SetLimit(int);
  CHttpRequest(void);
  void SetProxy(const CString&);
  int Execute(CUrl&);
  CVector<CString> GetRedirectVector(void) const;
  int Post(CUrl& iUrl, const CString& RawData);
  void SetParameter(const CString&, const CString&);
  void ClearParameters(void);
  CString GetData(void) const;
  CString GetStatus(void) const;
  int GetStatusValue(void) const;
  CString GetVersion(void) const;
  CString GetValue(const CString&) const;
  int GetDataSize(void) const;
};

#endif

