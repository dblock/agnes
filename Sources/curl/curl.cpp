/*
  CUrl.cpp - URL implementation 
  (c) Daniel Doubrovkine - dblock@vestris.com
*/

#include <agnespch.hpp>

CUrl::CUrl(void){
  Valid = 0;
}

CUrl::CUrl(const CString& iUrl){  
  SetUrl(iUrl);  
}

CUrl::CUrl(const CUrl& Other) {
  (* this) = Other;
}

CUrl::~CUrl(void) {
}

CUrl& CUrl::operator=(const CUrl& Other) {
  UBrute = Other.UBrute;
  UScheme = Other.UScheme;
  USpecific = Other.USpecific;
  UUser = Other.UUser;
  UPassword = Other.UPassword;
  UPort = Other.UPort;
  UHost = Other.UHost;
  UUrlPath = Other.UUrlPath;
  UHostVector = Other.UHostVector;
  UFtpPath = Other.UFtpPath;
  UFtpPathType = Other.UFtpPathType;
  UHttpPath = Other.UHttpPath;
  UHttpSearchPath = Other.UHttpSearchPath;
  UHttpArg = Other.UHttpArg;
  UHttpSig = Other.UHttpSig;
  UHttpDirectory = Other.UHttpDirectory;
  UHttpFilename = Other.UHttpFilename;
  Valid = Other.Valid;
  return (* this);
}

void CUrl::SetUrl(const CString& iUrl){
  if (UBrute.StrLength()) {
    UScheme.Free();
    USpecific.Free();
    UUser.Free();
    UPassword.Free();
    UPort.Free();
    UHost.Free();
    UUrlPath.Free();  
    UHostVector.Clear();
    UFtpPath.Clear();
    UFtpPathType = 0;    
    UHttpPath.Free();
    UHttpSearchPath.Free();
    UHttpArg.Free();
    UHttpSig.Free();    
    UHttpDirectory.Free();
    UHttpFilename.Free();
  }
  UBrute = iUrl;  
  Valid = 0;
  ParseUrl(UBrute);
}

CString CUrl::ReadScheme(int& curPos, const CString& iUrl){
  /*
    scheme must start at zero, known schemes are
    (implemented) ftp       File Transfer protocol
    (implemented) http      Hypertext Transfer Protocol
    gopher                  The Gopher protocol
    mailto                  Electronic mail address
    news                    USENET news
    nntp                    USENET news using NNTP access
    telnet                  Reference to interactive sessions
    wais                    Wide Area Information Servers
    file                    Host-specific file names
    prospero                Prospero Directory Service
    */
  int curPosLocal = 0;
  while (curPosLocal < iUrl.StrLength()) {
    switch(iUrl[curPosLocal]) {   
    case ':':
      curPos = curPosLocal+1;     
      return iUrl.Copy(0, curPosLocal);
    case '+':
    case '.':
    case '-': 
      break;
    default:
      if ((iUrl[curPosLocal] >= 'a')&&(iUrl[curPosLocal] <= 'z')) break; else return "";
    }
    curPosLocal++;
  }
  return "";
}

void CUrl::ParseUrl(const CString& iUrl){
  // URL general syntax: <scheme>:<scheme-specific-part> 
  int curPos = 0;
  UScheme = ReadScheme(curPos, iUrl);
  if (UScheme.StrLength()) {
    if (iUrl.Copy(curPos, 2) != "//") {
      UScheme = "http";
      curPos = 0;
    } else curPos+=2;
  } else {
    if (!UScheme.StrLength()) UScheme = "http";
  }
  USpecific = iUrl.Copy(curPos, iUrl.StrLength());
  /*
    <user>:<password>@<host>:<port>/<url-path>
    parse terms of dilimiters
    */  
  if (ParseUrlSpecific(curPos, USpecific)) {
    if (!UHost.StrLength()) UHost = "localhost";
    if (!UPort.StrLength()) UPort = "80";
    
    if (ParseHost(UHost)) {      
      UUrlPath = USpecific.Copy(curPos, USpecific.StrLength());
      if (UScheme == "HTTP") Valid = ParseHttpPath(UUrlPath);
      else if (UScheme == "FTP") Valid = ParseFtpPath(UUrlPath);            
      else {
	Valid = 1;
	StrError = "scheme not implemented";
      }
      
    }
  }
}

int CUrl::ParseHttpPath(const CString& iUrlPath){

  /*
    UHttpDirectory = directory with no parameters, cannot be empty
    UHttpFilename = filename with no parameters
    UHttpArg = everything except the location inside the page defined by #
    UHttpSig = location name defined in the page
    UHttpSearchPath = everything after the parameter (?) without the location inside page
    UHttpPath = everything before the parameter (?)
    */

  
  int sPos = iUrlPath.Pos('#');
  if (sPos >= 0) {
    UHttpSig = iUrlPath.Copy(sPos+1, iUrlPath.StrLength());    
    UHttpArg = iUrlPath.Copy(0, sPos);
  } else UHttpArg = iUrlPath;

  int qPos = UHttpArg.Pos('?');
  if (qPos >= 0) {
    UHttpPath = UHttpArg.Copy(0, qPos);
    UHttpSearchPath = UHttpArg.Copy(qPos+1, UHttpArg.StrLength());    
  } else UHttpPath = UHttpArg;     
  
  int ePos = UHttpPath.InvPos('/');
  if (ePos >= 0) {
    UHttpDirectory = UHttpPath.Copy(0, ePos);
    UHttpFilename = UHttpPath.Copy(ePos+1, UHttpPath.StrLength());
  } else UHttpFilename = UHttpPath;
  if (!UHttpDirectory.StrLength() || (UHttpDirectory[0] != '/')) {
    CString NewUHttpDirectory("/");
    NewUHttpDirectory += UHttpDirectory;
    UHttpDirectory = NewUHttpDirectory;
  }
  if (UHttpDirectory[UHttpDirectory.StrLength()-1] != '/') UHttpDirectory += '/';
  return 1;
}

int CUrl::ParseFtpPath(const CString& iUrlPath){
  int curPos = 0;  
  int prevPos = 0;
  UFtpPathType = 0;
  while (curPos < iUrlPath.StrLength()) {
    switch(iUrlPath[curPos]){
    case '/':
      UFtpPath+=iUrlPath.Copy(prevPos, curPos-prevPos);
      prevPos = curPos+1;
      break;
    case ';':
      UFtpPath+=iUrlPath.Copy(prevPos, curPos-prevPos);
      prevPos = curPos+1;
      if (iUrlPath.Copy(prevPos, strlen("type=")) == "type=") {
	curPos+=(strlen("type=")+1);
	UFtpPathType = iUrlPath[curPos];
	if (iUrlPath.StrLength() > (curPos+1)){
	  StrError = "FTP type too long";
	  return 0;
	}	
	return 1;
      } else {
	StrError = "invalid FTP type";
	return 0;
      }
    }
    curPos++;
  }
  return 1;
}

int CUrl::ParseUrlSpecific(int& curPos, const CString& iUrl){
  curPos = 0;
  int prevPos = 0;
  CString curVector;
  while (curPos < iUrl.StrLength()) {
    switch(iUrl[curPos]){
    case ':':
      if (curVector.StrLength()) {
	StrError = "invalid host/username";
	return 0;
      }
      curVector=iUrl.Copy(prevPos, curPos-prevPos);
      prevPos=curPos+1;
      break;
    case '@':
      if (curVector.StrLength()) {
	UUser = curVector;
	UPassword = iUrl.Copy(prevPos, curPos-prevPos);
	curVector.Free();
      } else UUser = iUrl.Copy(prevPos, curPos-prevPos);
      prevPos = curPos+1;
      break;
    case '/':
      if (curVector.StrLength()) {
	UHost = curVector;
	UPort = iUrl.Copy(prevPos, curPos-prevPos);
	if (!UPort.Val()) {
	  StrError = "invalid port";
	  return 0;
	}
	curVector.Free();	
      } else UHost = iUrl.Copy(prevPos, curPos-prevPos);      
      curPos++;
      return 1;
    }
    curPos++;
  }
  if (curVector.StrLength()) {
    UHost = curVector;
    UPort = iUrl.Copy(prevPos, curPos-prevPos);   
  } else UHost = iUrl.Copy(prevPos, curPos-prevPos);
  return 1;
}

char CUrl::X2C(char * what) const {
  char digit;
  digit = (what[0] >= 'A' ? ((what[0] & 0xdf) - 'A')+10 : (what[0] - '0'));
  digit *= 16;
  digit += (what[1] >= 'A' ? ((what[1] & 0xdf) - 'A')+10 : (what[1] - '0'));
  return(digit);
}

char * CUrl::MakeC2X(char c){
  workbuf[0]='%';
  sprintf(workbuf+1, "%02X", c);
  workbuf[3] = 0; 
  return workbuf;
}

CString CUrl::Escape(const CString& Brute) {
  CString Tmp;
  if (Brute.StrLength()) {
    int prev = 0;
    for (int i=0;i<Brute.StrLength();i++){
      if (!(isalnum(Brute[i]))&&(Brute[i]!=' ')&&(Brute[i]>0)){
	if (i>prev) Tmp+=Brute.Copy(prev, i-prev);
	prev = i+1;
	Tmp += MakeC2X(Brute[i]);
      }
    }
    Tmp+=Brute.Copy(prev, Brute.StrLength()-prev);        
  }
  return Tmp;
}

CString CUrl::UnEscape(const CString& Brute) const {
  CString Result;
  if (Brute.StrLength()){
    char * cBrute = strdup(Brute.asString());    
    int x,y;
    for(x=0,y=0;cBrute[y];++x,++y) {
      if(((cBrute[x] = cBrute[y]) == '%')) {
        cBrute[x] = X2C(&cBrute[y+1]);
        y+=2;
      }
    }
    cBrute[x] = 0;
    Result += cBrute;    
    delete cBrute;
  }
  return Result;
}

int CUrl::ParseHost(const CString& iHost){
  int curPos = 0;
  int prevPos = 0;
  while (isalnum(iHost[curPos]) || (iHost[curPos]=='-') || (iHost[curPos]=='.')) {
    switch(iHost[curPos]) {
    case '.':
      UHostVector+=iHost.Copy(prevPos, curPos - prevPos);
      prevPos = curPos+1;
    }
    curPos++;
  }
  if (curPos != iHost.StrLength()-1) {
    StrError = "invalid host";
    return 1;
  } else return 0;
}

int CUrl::operator==(const CUrl& iUrl){  
  return (
	  (UScheme == iUrl.UScheme)&&
	  (UUser == iUrl.UUser)&&
	  (UPassword == iUrl.UPassword)&&
	  (UPort == iUrl.UPort)&&
	  (UHost == iUrl.UHost)&&
	  (UUrlPath == iUrl.UUrlPath)&&
	  (UFtpPathType == iUrl.UFtpPathType)&&
	  (UHttpPath == iUrl.UHttpPath)&&
	  (UHttpArg == iUrl.UHttpArg)&&
	  (UHttpSig == iUrl.UHttpSig)&&
	  (Valid)&&(iUrl.Valid));	  			       
}

CString CUrl::GetUrlPath(void) const { 
  CString Result(UHttpDirectory);
  Result += UHttpFilename;
  if (UHttpSearchPath.StrLength()) {
    Result += '?';
    Result += UHttpSearchPath;
  }
  return UnEscape(Result);  
}

CString CUrl::ResolveDirectory(CString OtherUHttpDirectory) const {  
  CString CurrentDirectory = UHttpDirectory;  
  if (CurrentDirectory.StrLength() && (CurrentDirectory[CurrentDirectory.StrLength()-1] == '/')) CurrentDirectory.StrDelete(CurrentDirectory.StrLength()-1, CurrentDirectory.StrLength());
  while (CurrentDirectory.StartsWith("./")) CurrentDirectory.StrDelete(0, strlen("./"));
  while (OtherUHttpDirectory.StartsWith("./")) OtherUHttpDirectory.StrDelete(0, strlen("./"));
  while (OtherUHttpDirectory.StartsWith("../")) {    
    int bPos = CurrentDirectory.InvPos('/');
    if (bPos >= 0) {
      CurrentDirectory.StrDelete(bPos, CurrentDirectory.StrLength());
      OtherUHttpDirectory.StrDelete(0, strlen("../"));
      while (OtherUHttpDirectory.StartsWith("./")) OtherUHttpDirectory.StrDelete(0, strlen("./"));
    } else break;
  }    
  /*
    resolver suppresses forward slash, but server names might have one appended already
    */  
  int sFlag = 1;
  if (CurrentDirectory.StrLength()&&(CurrentDirectory[CurrentDirectory.StrLength()-1] == '/')) sFlag = 0;
  else if (OtherUHttpDirectory.StrLength() && (OtherUHttpDirectory[0] == '/')) sFlag = 0;
  if (sFlag) CurrentDirectory+='/';
  CurrentDirectory += OtherUHttpDirectory;  
  /*
    look for eventual ../.. in the returned directory
    */  
  
  while (1) {
    int dotPos = CurrentDirectory.isSubStr("/../");
    if (dotPos == -1) break;
    // find the element on the left of the found /../
    int eltPos = (dotPos?dotPos-1:0);
    for (;eltPos >= 0;eltPos--) if (CurrentDirectory[eltPos] == '/') break;    
    CurrentDirectory.StrDelete(eltPos, dotPos + strlen("/..") - eltPos);
  }    
  return CurrentDirectory;
}

CUrl CUrl::Resolve(const CUrl& iUrl) const { 
  CUrl Result(iUrl);  
  if (Result.UScheme != UScheme) return Result;  
  if ((Result.UHost == "localhost")||(Result.UHost.StrLength() == 0)) {
    Result.UHost = UHost;
  } else return Result;  
  Result.UHttpDirectory = ResolveDirectory(Result.UHttpDirectory);    
  return Result;
}

CUrl CUrl::Resolve(const CString& iUrl) const {
  CUrl Result;

  int colPos = iUrl.Pos(':');
  if (colPos >= 0){
    CString colStart = iUrl.Copy(0, colPos); 
    if ((colStart == "HTTP")||
	(colStart == "FILE")||
	(colStart == "FTP")||
	(colStart == "GOPHER")||
	(colStart == "NEWS")||
	(colStart == "NNTP")||
	(colStart == "TELNET")||
	(colStart == "WAIS")||
	(colStart == "PROSPERO")	
	) {
      Result.SetUrl(iUrl);
      return Resolve(Result);
    }
  }

  Result = (* this);  
  if (iUrl[0] == '/'){   
    Result.ParseHttpPath(iUrl);
  } else if (iUrl.Pos(':') == -1) {    
    Result.ParseHttpPath(ResolveDirectory(iUrl));        
  } else {    
    CUrl NewResult;
    NewResult.UHttpDirectory = iUrl;
    return NewResult;           
  }
  return Result;  
}

CString CUrl::GetHttpAll(void) const {  
  CString Result(UScheme);
  if (Result.StrLength()) {
    Result += "://";
    Result += UHost;
    if (UPort.Val() != 80) {
      Result +=":"; 
      Result += UPort;
    }
  }
  Result += UHttpDirectory;
  Result += UHttpFilename;  
  if (UHttpSearchPath.StrLength()) {
    Result += '?';
    Result += UHttpSearchPath;
  }    
  return Result;
}

/*
  Current URL contains the other URL?
  */
int CUrl::Includes(const CUrl& iUrl) const {  
  // compare schemes (http://)    
  if (GetScheme() != iUrl.GetScheme()) return 0;
  // compare host names (todo: IPs) and ports
  if (!GetHost().EndsWith(iUrl.GetHost())) return 0;
  if (GetPortValue() != iUrl.GetPortValue()) return 0;  
  // everything seems okay
  return 1;
}


