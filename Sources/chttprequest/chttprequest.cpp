/*
  CHttpRequest
  */

#include <agnespch.hpp>
//#define _U_DEBUG

extern CString wsLastError;

CHttpRequest::CHttpRequest(void){
  RTimeout = 10;
  RLimit = -1;
  FollowRedirections = 1;
}

int CHttpRequest::SetFollowRedirections(int Value){
  if (Value != FollowRedirections) {
    FollowRedirections = Value;
    return 1;
  } else return 0;
}

int CHttpRequest::GetFollowRedirections(void) const {
  return FollowRedirections;
}

int CHttpRequest::SetTimeout(int Value){
  if (RTimeout != Value){
    RTimeout = Value;
    return 1;
  } else return 0;
}

void CHttpRequest::SetProxy(const CString& iProxy){
  ProxyURL = iProxy;
  Proxy.SetUrl(iProxy);
}

void CHttpRequest::SetParameter(const CString& Name, const CString& Value){
  RHeaderParams.set_value(Name, Value);
}

void CHttpRequest::ClearParameters(void){
  RHeaderParams.clear();
}

int CHttpRequest::Post(CUrl& iUrl, const CString& RawData){
  CString Server;
  int Port;  		       
  if (ProxyURL.StrLength() && Proxy.isValid()) {
    Server = Proxy.GetHost();
    Port = Proxy.GetPortValue();
  } else if (iUrl.isValid()) {
    Server = iUrl.GetHost();
    Port = iUrl.GetPortValue();
  } else return 0;
  inetSocket Sock(Port, Server);
  if (wsLastError.StrLength()) return 0;
  char iE[] = "xxxx\0"; sprintf(iE, "%c%c", 13, 10);  
  return PostHTTP(iUrl, RawData, iE, Sock);
}

CVector<CString> CHttpRequest::GetRedirectVector(void) const {
  return RedirectVector;
}

int CHttpRequest::Execute(CUrl& iUrl){
#ifdef _U_DEBUG
    cout << "CHttpRequest::Execute()" << endl;
#endif

    RHeader.Free();
    RData.Free();
    RStatus.Free();
    RStatusValue = -1;
    RedirectVector.Clear();

    CString Server;
    int Port;       
    if (ProxyURL.StrLength() && Proxy.isValid()) {
      Server = Proxy.GetHost();
      Port = Proxy.GetPortValue();
    } else if (iUrl.isValid()) {
      Server = iUrl.GetHost();
      Port = iUrl.GetPortValue();
    } else {
      RStatusValue = HTTPR_USER + 1;
      return 0;
    }
#ifdef _U_DEBUG
    cout << "CHttpRequest::Execute() - creating inetSocket" << endl;
#endif
    inetSocket Sock(Port, Server);
    if (wsLastError.StrLength()) {
#ifdef _U_DEBUG
      cout << "CHttpRequest::Execute() - ERROR at inetSocket - " << wsLastError << endl;
#endif
      RStatusValue = HTTPR_USER + 2;      
      return 0;
    }
#ifdef _U_DEBUG
    cout << "CHttpRequest::Execute() - inetSocket created" << endl;
#endif
    char iE[] = "xxxx\0"; sprintf(iE, "%c%c", 13, 10);  
    
    if (!GetHTTP10(iUrl, iE, Sock)) {
      if (!GetHTTP09(iUrl, iE, Sock)) {
	return 0;
      } else return 1;
    }
    else return 1;
}

int CHttpRequest::PostHTTP(CUrl& iUrl, const CString& RawData, const CString& iE, inetSocket& Sock){
  CString Request;
  if (ProxyURL.StrLength() && Proxy.isValid()) {
    Request += "POST "; Request += iUrl.GetScheme(); Request+="://";
    Request += iUrl.GetHost(); 
    if (iUrl.GetPortValue() != 80) {
      Request+=":"; 
      Request += iUrl.GetPort();
    }
    Request += iUrl.GetUrlPath(); Request += " HTTP/1.0"; Request+=iE;    
  } else {
    Request += "POST "; Request += iUrl.GetUrlPath(); Request += " HTTP/1.0"; Request+=iE;
    Request += "Host: "; Request += iUrl.GetHost(); Request+=iE;
  }
  for (int i=0;i<RHeaderParams.entries_count();i++) {
    Request+=RHeaderParams.get_name(i);
    Request+=": ";
    Request+=RHeaderParams.get_value(i);
    Request += iE;
  }
  Request += iE;
  Request += RawData;
  if (!Send(Sock, Request)) return 0;
  ProcessHeader(Sock);
  ProcessData(Sock);
  if (RData.StrLength()) {
    RHeaderResponse.clear();
    RStatusValue = 200;
    RStatus = "200";
    return 1;
  } else return 0;
}

int CHttpRequest::GetHTTP09(CUrl& iUrl, const CString& iE, inetSocket& Sock){
  /*
    attempt a retrieval of HTTP/0.9
    */  
  CString Request;  
  if (RLimit) Request += "GET "; else Request+="HEAD ";
  Request += iUrl.GetScheme(); Request+="://";
  Request += iUrl.GetHost();
  if (iUrl.GetPortValue() != 80) {
    Request+=":"; 
    Request += iUrl.GetPort();
  }
  Request += iUrl.GetUrlPath(); Request+=iE;  
  for (int i=0;i<RHeaderParams.entries_count();i++) {
    Request+=RHeaderParams.get_name(i);
    Request+=": ";
    Request+=RHeaderParams.get_value(i);
    Request += iE;
  }
  Request += iE;
#ifdef _U_DEBUG
  cout << "# HTTP 0.9 Request: =====" << endl;
  cout << Request;
  cout << "=====================" << endl;
#endif
  /*
    issue request
    */
  if (!Send(Sock, Request)) return 0;
  RHeader.Free();
  RData.Free();
  RStatus.Free();
  RStatusValue = -1;
  ProcessData(Sock);
  if (RData.StrLength()) {
    RHeaderResponse.clear();
    RStatusValue = 200;
    RStatus = "200";
    return 1;
  } else if (RStatusValue != -1) {
    return 0;
  } else {
    RStatusValue = HTTPR_USER + 3;
    return 0;
  }
}

int CHttpRequest::Send(inetSocket& Sock, const CString& Request){
  if (send(Sock.S,Request.asString(),Request.StrLength(),0) == -1) {
    switch(errno) {
    case EBADF: Sock.wserror("(sending to port) The socket  argument  is  not  a  valid  file descriptor."); break;
#ifdef ANSI
    case ECONNRESET: Sock.wserror("(sending to port) A connection was forcibly closed by a peer."); break;
    case EDESTADDRREQ: Sock.wserror("(sending to port) The socket is not connection-mode and no peer address is set."); break;
    case EMSGSIZE: Sock.wserror("(sending to port) The message is too large be sent all at once, as the socket requires."); break;
    case ENOTCONN: Sock.wserror("(sending to port) The socket is not connected or otherwise  has not had the peer prespecified."); break;
    case ENOTSOCK: Sock.wserror("(sending to port) The socket  argument  does  not  refer  to  a socket."); break;
    case EOPNOTSUPP: Sock.wserror("(sending to port) The socket  argument  is  associated  with  a socket  that  does not support one or more of the values set in flags."); break;      
    case ENETDOWN: Sock.wserror("(sending to port) The local interface used to reach the  destination is down."); break;      
    case ENETUNREACH: Sock.wserror("(sending to port) No route to the network is present."); break;      
    case ENOBUFS: Sock.wserror("(sending to port) Insufficient resources were available in  the system to perform the operation."); break;      
#ifdef ENOSR
    case ENOSR: Sock.wserror("(sending to port) There  were  insufficient  STREAMS  resources available for the operation to complete."); break;      
#endif
#endif
    case EINTR: Sock.wserror("(sending to port) A signal interrupted send() before  any  data was transmitted."); break;
    case EPIPE: Sock.wserror("(sending to port) The socket is shut down for writing,  or  the socket  is  connection-mode  and  the peer is closed or shut  down  for  reading.   In  the latter  case,  and  if  the socket is of type SOCK_STREAM, the SIGPIPE signal is  generated to the calling process."); break;      
    case EAGAIN: Sock.wserror("(sending to port) The  socket's  file  descriptor   is   marked O_NONBLOCK  and the requested operation would block."); break;      
    case EIO: Sock.wserror("(sending to port) An I/O error occurred while reading  from  or writing to the file system."); break;      
    default : Sock.wserror("(sending to port) Unexpected error."); break;
    }
    RStatusValue = HTTPR_USER + 2;
    return 0;
  } else return 1;
}

int CHttpRequest::GetHTTP10(CUrl& iUrl, const CString& iE, inetSocket& Sock){
  /*
    attempt a retrieval of HTTP/1.0
    */  
  CString Request;
  if (ProxyURL.StrLength() && Proxy.isValid()) {
    if (RLimit) Request += "GET "; else Request+="HEAD ";
    Request += iUrl.GetScheme(); Request+="://";
    Request += iUrl.GetHost();
    if (iUrl.GetPortValue() != 80) {
      Request+=":"; 
      Request += iUrl.GetPort();
    }
    Request += iUrl.GetUrlPath(); Request += " HTTP/1.0"; Request+=iE;    
  } else {
    if (RLimit) Request += "GET "; else Request+="HEAD ";
    Request += iUrl.GetUrlPath(); Request += " HTTP/1.0"; Request+=iE;
    Request += "Host: "; Request += iUrl.GetHost(); Request+=iE;
  }
  for (int i=0;i<RHeaderParams.entries_count();i++) {
    Request+=RHeaderParams.get_name(i);
    Request+=": ";
    Request+=RHeaderParams.get_value(i);
    Request += iE;
  }
  Request += iE;
#ifdef _U_DEBUG
  cout << "# HTTP Request: =====" << endl;
  cout << Request;
  cout << "=====================" << endl;
#endif
  /*
    issue request
    */
  CString RLoc;
  if (!Send(Sock, Request)) return 0;    
  ProcessHeader(Sock);
  ProcessData(Sock);
  switch(RStatusValue) {
  case 200: return 1;    
  case 301:
  case 302:
  case 303:
  case 307:  
    if (!FollowRedirections) return RStatusValue;
    RLoc = RHeaderResponse.get_value("Location");
    if (RLoc.StrLength()) {      
      /*
	HTTP 1.1 - Temporary Redirect is 302 and 307
	RedirectVector is relevant for final URL address
	that could be retrieved
	*/
      if (!RedirectVector.Contains(RLoc)) {
	RedirectVector+=RLoc; 
	CUrl NewURL(RLoc);      
	if (!Proxy.isValid()) {
	  inetSocket Sock2(NewURL.GetPortValue(), NewURL.GetHost());
	  return GetHTTP10(NewURL, iE, Sock2);
	} else {
	  Sock.Reopen();
	  return GetHTTP10(NewURL, iE, Sock);
	}      
      }
    }  
    return RStatusValue;
  case 305: /* use proxy */    
    RLoc = RHeaderResponse.get_value("Location");
    if (RLoc.StrLength()) {
      CUrl ProxyURL(RLoc);
      if (ProxyURL.isValid()) {
	inetSocket ProxySock(ProxyURL.GetPortValue(), ProxyURL.GetHost());
	if (wsLastError.StrLength()) return RStatusValue;	
	return GetHTTP10(iUrl, iE, ProxySock);
      }
    }
    return RStatusValue;  
  default: return RStatusValue;   
  }
}

CString CHttpRequest::ReadLine(inetSocket& Sock){
  CString Result;
  char c;  
  FD_ZERO(&fdset);
  FD_SET(Sock.S,&fdset);
  tv.tv_sec=RTimeout; tv.tv_usec=0;
  while (select(Sock.S+1, &fdset, NULL, NULL, &tv) > 0) {
    if ((FD_ISSET(Sock.S,&fdset))&&(recv(Sock.S, &c, 1, 0))){
      if ((c==10)/*||(c == EOF)*/) break;   
      else if (c!=13) Result+=c;
    } else break;    
  }
  return Result;
} 

#define MAX_BUFFER 1024

CString CHttpRequest::ReadRemaining(inetSocket& Sock){
  char Buffer[MAX_BUFFER+1];
  int Count = 0;
  CString Result;
  char c;  
  FD_ZERO(&fdset);
  FD_SET(Sock.S,&fdset);  
  tv.tv_sec=RTimeout; tv.tv_usec=0;
  while (select(Sock.S+1, &fdset, NULL, NULL, &tv) >= 0) {
    if ((FD_ISSET(Sock.S,&fdset))&&(recv(Sock.S, &c, 1, 0)))
      Buffer[Count++] = c; else break;
    if (Count == MAX_BUFFER) {
      Buffer[Count] = 0;
      Result.StrAppend(Buffer, Count);
      Count = 0;
      if ((RLimit > 0)&&(Result.StrLength() >= RLimit)) return Result;
    }
    //if (c == EOF) break;
  }
  if (Count) {
    Buffer[Count] = 0;
    Result.StrAppend(Buffer, Count);
  }
  return Result;
}

void CHttpRequest::ProcessRStatus(const CString& Line){
  if (!RStatus.StrLength()) {
#ifdef _U_DEBUG
    cout << "HTTPREQUEST::RStatusLine:[" <<  Line << "]" << endl;
#endif
    RStatus = Line;     
    int sPos = RStatus.Pos(' ');
    if (sPos >= 0) {
      RVersion = RStatus.Copy(0, sPos);
#ifdef _U_DEBUG
      cout << "HTTPREQUEST::RVersion:[" <<  RVersion << "]" << endl;
#endif
      int rPos = RStatus.Pos(' ', sPos+1);
      if (rPos >= 0) {
#ifdef _U_DEBUG
	cout << "HTTPREQUEST::RStatus:[" <<  RStatus.Copy(sPos+1, rPos-sPos-1) << "]" << endl;
#endif
	RStatusValue = RStatus.Copy(sPos+1, rPos-sPos-1).Val();
      } else RStatusValue = -1;
    } else {
      RStatusValue = HTTPR_USER + 4;
      RVersion.Free();
    }
  }
}

void CHttpRequest::ProcessOneLine(const CString& Line){
  int sPos = Line.Pos(':');
  if (sPos > 0) {
    RHeaderResponse.set_value(Line.Copy(0, sPos), Line.Copy(sPos+1, Line.StrLength()).StrTrim());
#ifdef _U_DEBUG
    cout << "CHTTPREQUEST::Header:[" << Line.Copy(0, sPos) << "]:[" << Line.Copy(sPos+1, Line.StrLength()).StrTrim() << "]" << endl;
#endif
  } else {
#ifdef _U_DEBUG
    cout << "CHTTPREQUEST::Header (???):[" << Line << "]" << endl;
#endif
  }
}

void CHttpRequest::ProcessHeader(inetSocket& Sock){
  CString Line;
  RHeaderResponse.clear();
  RStatus.Free();
  ProcessRStatus((Line = ReadLine(Sock)));
  if (Line.StrLength()) do {
    Line = ReadLine(Sock);    
    if (Line.StrLength()) {
      RHeader+=Line;
      ProcessOneLine(Line);
    }
  } while (Line.StrLength());
}

void CHttpRequest::ProcessData(inetSocket& Sock){
  if (RLimit != 0) RData = ReadRemaining(Sock);
  else RData.Free();
#ifdef _U_DEBUG
  cout << "HTTP_REQUEST::PROCESSDATA:[" << RData.StrLength() << "]" << endl;
#endif
}

int CHttpRequest::SetLimit(int iSize) {
  if (RLimit != iSize){
    RLimit = iSize;
    return 1;
  } else return 0;
}

CString CHttpRequest::GetData(void) const {
  return RData;
}

CString CHttpRequest::GetStatus(void) const {
  return RStatus;
}

int CHttpRequest::GetStatusValue(void) const {
  return RStatusValue;
}

CString CHttpRequest::GetVersion(void) const {
  return RVersion;
}

CString CHttpRequest::GetValue(const CString& Name) const {
  return RHeaderResponse.get_value(Name);
}

int CHttpRequest::GetDataSize(void) const {
  return RData.StrLength();
}

CString CHttpRequest::MapError(int Error) {
  CString Result;
  switch(Error) {
  case 100: return "100/Continue";
  case 101: return "101/Switching Protocols";
  case 200: return "200/OK";
  case 201: return "201/Created";
  case 202: return "202/Accepted";
  case 203: return "203/Non-Authoritative Information";
  case 204: return "204/No Content";
  case 205: return "205/Reset Content";
  case 206: return "206/Partial Content";
  case 300: return "300/Multiple Choices";
  case 301: return "301/Moved Permanently";
  case 302: return "302/Moved Temporarily";
  case 303: return "303/See Other";
  case 304: return "304/Not Modified";
  case 305: return "305/Use Proxy";
  case 307: return "307/Temporary Redirect"; //HTTP 1.1 Section 10.3.8
  case 400: return "400/Bad Request";
  case 401: return "401/Unauthorized";
  case 402: return "402/Payment Required";
  case 403: return "403/Forbidden";
  case 404: return "404/Not Found";
  case 405: return "405/Method Not Allowed";
  case 406: return "406/Not Acceptable";
  case 407: return "407/Proxy Authentication Required";
  case 408: return "408/Request Time-out";
  case 409: return "409/Conflict";
  case 410: return "410/Gone";
  case 411: return "411/Length Required";
  case 412: return "412/Precondition Failed";
  case 413: return "413/Request Entity Too Large";
  case 414: return "414/Request-URI Too Large";
  case 415: return "415/Unsupported Media Type";
  case 416: return "416/Requested range not satisfiable"; //HTTP 1.1 Section 10.4.17
  case 417: return "417/Expectation Failed"; //HTTP 1.1 Section 10.4.18
  case 500: return "500/Internal Server Error";
  case 501: return "501/Not Implemented";
  case 502: return "502/Bad Gateway";
  case 503: return "503/Service Unavailable";
  case 504: return "504/Gateway Time-out";
  case 505: return "505/HTTP Version Not Supported";
  case HTTPR_USER+1: return "U1/Invalid Hostname";
  case HTTPR_USER+2: Result = "USOCK/"; Result+=wsLastError; return Result;   
  case HTTPR_USER+3: return "U3/Document Contains No Data";
  case HTTPR_USER+4: return "U4/Connexion Timed Out";
  default: Result.Free(); Result+=Error; Result+="/Undocumented Error"; return Result;
  }
}
