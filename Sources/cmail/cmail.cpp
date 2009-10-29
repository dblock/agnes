/*
  CMail - email syntax (maybe semantics)

  RFC 822
  
     address     =  mailbox                      ; one addressee
                 /  group                        ; named list
     group       =  phrase ":" [#mailbox] ";"
     mailbox     =  addr-spec                    ; simple address
                 /  phrase route-addr            ; name & addr-spec
     route-addr  =  "<" [route] addr-spec ">"
     route       =  1#("@" domain) ":"           ; path-relative         

     addr-spec   =  local-part "@" domain        ; global address
     local-part  =  word *("." word)             ; uninterpreted
                                                 ; case-preserved
     domain      =  sub-domain *("." sub-domain)
     sub-domain  =  domain-ref / domain-literal
     domain-ref  =  atom                         ; symbolic reference
     */


#include <agnespch.hpp>

#define CR 13
#define LF 10
#define HTAB 9
#define SPACE 32

//#define M_DEBUG

#include <inetsock/inetsock.h>

const CString Specials = "()/<>@,;:\\\".[]";

CMail::~CMail(void) {

}

CMail::CMail(void){
  FGood = 0;
}

CMail::CMail(const CString& cMail){
  Set(cMail);
}

void CMail::Set(const CString& cMail){
  FMail = cMail;
  FMail.StrTrim();
  FError.Free();
  FGood = Parse();  
}

CString CMail::GetError(void){
  return FError;
}

int CMail::isGoodInternet(void){
  return FGood;
  /*
    if (FGood) {
    #ifdef M_DEBUG
    cout << "verifying host: " << FHost <<  endl;    
    #endif            
    if ((int)(inet_addr(FDomain.asString())) != -1) {   
    
    } else {
    if (!gethostbyname(FDomain.asString())){      
    FError = FDomain;
    switch(h_errno){
    case HOST_NOT_FOUND: FError+=" - No such host is known."; break;
    case TRY_AGAIN: FError+=" - A  temporary  and  possibly  transient  error occurred,  such  as  a failure of a server to respond."; break;
    case NO_RECOVERY: FError+=" - An unexpected server failure  occurred  which can not be recovered."; break;
    case NO_DATA: FError+=" - The server recognized  the  request  and  the name  but  no  address is available.  Another type of request to the name  server  for  the domain might return an answer."; break;
    default: FError+=" - gethostbyname() has failed, unexpected error."; break;
    }
    return 0;
    }
    }
    return 1;
    } else return 0;  
    */
}

int CMail::Parse(void){
  int curPos=0;
  CString ParseResult = MPAddress(FMail, curPos);  
  return (ParseResult.StrLength() > 0) && (curPos == FMail.StrLength());
}

int CMail::isGood(void){
  return FGood;
}

CString CMail::MPAddress(const CString& iStr, int& curPos){
  /*
    address     =  mailbox                      ; one addressee
                /  group                        ; named list
		*/
#ifdef M_DEBUG
  cout << "MPAddress() - " << curPos << endl;
#endif

  if (curPos < iStr.StrLength()) {
    CString Mailbox = MPMailbox(iStr, curPos);
    if (Mailbox.StrLength()) return Mailbox;
    CString Group = MPGroup(iStr, curPos);
    if (Group.StrLength()) return Group;
    return "";
  } else return "";
}


CString CMail::MPGroup(const CString& iStr, int& curPos){
/*
  group       =  phrase ":" [#mailbox] ";"
  */  

#ifdef M_DEBUG
  cout << "MPGroup() - " << curPos << endl;
#endif

  CString Result;
  CString Phrase = MPPhrase(iStr, curPos);
  if (!Phrase.StrLength()) return "";
  Result+=Phrase;
  if (iStr[curPos]!=':') return "";
  Result+=':';
  int workingPos = curPos+1;
  CString MBox;
  if (iStr[workingPos]!=',') do {
    if (iStr[workingPos] == ',') {
      workingPos++;
      Result+=',';
    }
    MBox = MPMailbox(iStr, curPos);
    if (MBox.StrLength()) Result+=MBox; else break;    
  } while (iStr[workingPos] == ',');
  if (iStr[workingPos] == ';'){
    Result+=';';
    workingPos++;
    curPos = workingPos;
    return Result;
  } else return "";
}


CString CMail::MPMailbox(const CString& iStr, int& curPos){
/*
  mailbox     =  addr-spec                    ; simple address
              /  phrase route-addr            ; name & addr-spec
  */

#ifdef M_DEBUG
  cout << "MPMailBox() - " << curPos << endl;
#endif

  if (curPos < iStr.StrLength()) {
    CString ASpec = MPAddressSpec(iStr, curPos);
    if (!ASpec.StrLength()) {
      CString Phrase = MPPhrase(iStr, curPos);
      if (Phrase.StrLength()) {
	CString RAddr = MPRouteAddr(iStr, curPos);
	if (RAddr.StrLength()) {
	  CString Result = Phrase;
	  Result+=RAddr;
	  return Result;
	} else return "";
      } else return "";
    } else return ASpec;
  } else return "";
}

CString CMail::MPPhrase(const CString& iStr, int& curPos){
  /*
    1*word
    */

#ifdef M_DEBUG
  cout << "MPPhrase() - " << curPos << endl;
#endif

  CString Result;
  CString Word;
  Word = MPWord(iStr, curPos);
  while (Word.StrLength()) {    
    Result+=Word;
    Word = MPWord(iStr, curPos);    
  }
  return Result;
}

CString CMail::MPRouteAddr(const CString& iStr, int& curPos){
/*
  route-addr  =  "<" [route] addr-spec ">"
  */

#ifdef M_DEBUG
  cout << "MPRouteAddr() - " << curPos << endl;
#endif

  if (iStr[curPos] == '<') {
    int workingPos=curPos+1;
    CString Route = MPRoute(iStr, workingPos);
    CString AddrSpec = MPAddressSpec(iStr, workingPos);
    if (AddrSpec.StrLength()) {
      CString Result(Route);
      Result+=AddrSpec;
      curPos = workingPos;
      return Result;
    } else return "";
  } else return "";
}

CString CMail::MPRoute(const CString& iStr, int& curPos){
/*
  route       =  1#("@" domain) ":"           ; path-relative         
  means 1 at least "@:" or "domain:"
  separated eventually by commas
  */

#ifdef M_DEBUG
  cout << "MPRoute() - " << curPos << endl;
#endif

  if (curPos < iStr.StrLength()) {
    int workingPos = curPos;
    if (iStr[workingPos] != '@') {
      CString FDomain = MPDomain(iStr, workingPos);      
      if (!FDomain.StrLength()) return "";
    } else workingPos++;
    if (iStr[workingPos] != ':') return "";
    CString Result = iStr.Copy(curPos, workingPos-curPos);
    curPos = workingPos+1;
    if (iStr[curPos] == ',') return (Result+=MPRoute(iStr, curPos));
    else return Result;
  } else return "";
}

CString CMail::GetUser(void){
  return FUser;
}

CString CMail::GetHost(void){
  return FDomain;
}

CString CMail::MPAddressSpec(const CString& iStr, int& curPos){
  /*
    addr-spec   =  local-part "@" domain        ; global address
    */  

#ifdef M_DEBUG
  cout << "MPAddressSpec() - " << curPos << endl;
#endif

  if (curPos < iStr.StrLength()) {    
    FUser = MPLocalPart(iStr, curPos);    
    if (!FUser.StrLength()) return "";
    if (iStr[curPos] == '@') {
      int workingPos = curPos+1;
      FDomain = MPDomain(iStr, workingPos);      
      if (!FDomain.StrLength()) return "";
      CString Result(FUser);
      Result+='@';
      Result+=FDomain;
      curPos = workingPos;      
      return Result;
    } else return "";
  } else return "";
}

CString CMail::MPLocalPart(const CString& iStr, int& curPos){
  /*
    addr-spec   =  local-part "@" domain        ; global address
    local-part  =  word *("." word)             ; uninterpreted  
    */

#ifdef M_DEBUG
  cout << "MPLocalPart() - " << curPos << endl;
#endif

  CString Result;
  CString Word;
  int workingPos = curPos;
  Word = MPWord(iStr, workingPos);    
  while ((iStr[workingPos] == '.') && Word.StrLength() && (curPos < iStr.StrLength())){    
    workingPos++;
    if (Result.StrLength()) Result+=".";
    Result+=Word;
    Word = MPWord(iStr, workingPos);
  }
  if (Word.StrLength()) Result+=Word;
  if (Result.StrLength()) curPos = workingPos;
  return Result;
}

CString CMail::MPWord(const CString& iStr, int& curPos){  

#ifdef M_DEBUG
  cout << "MPWord() - " << curPos << endl;
#endif

  CString Result;   
  Result = MPAtomSequence(iStr, curPos);
  if (!Result.StrLength()) {
    Result = MPQuotedString(iStr, curPos);    
  }
  return Result;
}

CString CMail::MPQuotedString(const CString& iStr, int& curPos){  
  /*
    quoted-string = <"> *(qtext/quoted-pair) <">; Regular qtext or quoted chars.
    */

#ifdef M_DEBUG
  cout << "MPQuotedString() - " << curPos << endl;
#endif
  if (curPos < iStr.StrLength()) {
    if (iStr[curPos] == '\"'){
      int workingPos = curPos+1;   
      while (workingPos < iStr.StrLength()){
	if (!MPQText(iStr, workingPos))
	  if (!MPQuotedPair(iStr, workingPos)) 
	    break;
      }
      if (iStr[workingPos] == '\"') {
	CString Result = iStr.Copy(curPos, workingPos - curPos);
	curPos = workingPos+1;
	return Result;
      } else return "";
    } else return "";
  } else return "";
}

char CMail::MPQText(const CString& iStr, int& curPos){
  /*
    <any CHAR excepting <">,     ; => may be folded
    "\" & CR, and including
    linear-white-space>
    */

#ifdef M_DEBUG
  cout << "MPQText() - " << curPos << endl;
#endif

  if (curPos < iStr.StrLength()) {
    char c = iStr[curPos]; 
    switch(c){
    case '\"':
    case '[':
    case ']':
    case '\\':
    case CR:    
      return 0;
    }    
    char d;
    if ((d = MPLinearWhiteSpace(iStr, curPos))) return d;
    curPos++;
    return c;
  } else return 0;
}

CString CMail::MPDomain(const CString& iStr, int& curPos){
  /*
    domain      =  sub-domain *("." sub-domain)                           
    */

#ifdef M_DEBUG
  cout << "MPDomain() - " << curPos << endl;
#endif

  CString Result;
  CString SubDomain;
  int workingPos = curPos;
  SubDomain = MPSubDomain(iStr, workingPos);    
  while ((iStr[workingPos] == '.') && SubDomain.StrLength() && (curPos < iStr.StrLength())){    
    workingPos++;    
    if (Result.StrLength()) Result+=".";
    Result+=SubDomain;
    SubDomain = MPSubDomain(iStr, workingPos);
  }
  if (SubDomain.StrLength()) {
    if (Result.StrLength()) Result+=".";
    Result+=SubDomain;  
  }
  if (Result.StrLength()) curPos = workingPos; 
  return Result;
}

CString CMail::MPSubDomain(const CString& iStr, int& curPos){
  /*
    sub-domain  =  domain-ref / domain-literal
    domain-ref  =  atom 
    */

#ifdef M_DEBUG
  cout << "MPSubDomain() - " << curPos << endl;
#endif

  CString Result = MPDomainRef(iStr, curPos);  
  if (!Result.StrLength()) Result = MPDomainLiteral(iStr, curPos);
  return Result;
}

char CMail::MPAtom(const CString& iStr, int& curPos){
  /*
    atom =  1*<any CHAR except specials, SPACE and CTLs>
    */

#ifdef M_DEBUG
  cout << "MPAtom() - " << curPos << endl;
#endif

  char c = iStr[curPos];
  if ((c >= 0)&&(c <= 31)) return 0;
  else if (c == 127) return 0;
  else if (c == ' ') return 0;
  else if (In(c, Specials)) return 0;
  else {    
    curPos++;
    return c;
  }
}

CString CMail::MPAtomSequence(const CString& iStr, int& curPos){

#ifdef M_DEBUG
  cout << "MPAtomSequence() - " << curPos << endl;
#endif

  CString Result;
  char c;
  do {
    c = MPAtom(iStr, curPos);
    if (c) Result+=c;
  } while(c);
  return Result;
}

int CMail::In(const char& What, const CString& Where){
  for (int i=0;i<Where.StrLength();i++)
    if (Where[i] == What) return 1;
  return 0;
}

CString CMail::MPDomainRef(const CString& iStr, int& curPos){
  return MPAtomSequence(iStr, curPos);  
}

char CMail::MPDtext(const CString& iStr, int& curPos){
  /*
    dtext       =  <any CHAR excluding "[",     ; => may be folded
    "]", "\" & CR, & including
    linear-white-space>
    */

#ifdef M_DEBUG
  cout << "MPDText() - " << curPos << endl;
#endif

  if (curPos < iStr.StrLength()) {
    char c = iStr[curPos]; 
    switch(c){
    case '[':
    case ']':
    case '\\':
    case CR:    
      return 0;
    }
    char d;
    if ((d = MPLinearWhiteSpace(iStr, curPos))) return d;
    return c;
  } else return 0;
}

char CMail::MPLinearWhiteSpace(const CString& iStr, int& curPos){
  /*
    linear-white-space =  1*([CRLF] LWSP-char)  ; semantics = SPACE
    ; CRLF => folding
    */ 

#ifdef M_DEBUG
  cout << "MPLinearWhiteSpace() - " << curPos << endl;
#endif

  if (curPos < iStr.StrLength()) {
    int workingPos = curPos;
    if ((iStr[workingPos] == 13)&&(iStr[workingPos+1] == 10)) workingPos+=2;
    char c = iStr[workingPos];
    if ((c==SPACE)||(c==HTAB)) {
      curPos = workingPos+1;
      return c;
    } else return 0;
  } else return 0;
}

CString CMail::MPDomainLiteral(const CString& iStr, int& curPos){
  /*
    domain-literal =  "[" *(dtext / quoted-pair) "]"
    */

#ifdef M_DEBUG
  cout << "MPDomainLiteral() - " << curPos << endl;
#endif

  if (curPos < iStr.StrLength()) {        
    int workingPos = curPos;
    if (iStr[workingPos] == '[') {            
      workingPos++;
      while (workingPos < iStr.StrLength()) {
	if (MPDtext(iStr, workingPos)) workingPos++;
	else if (MPQuotedPair(iStr, workingPos)) workingPos++;
	else break;	
      }
      if (iStr[workingPos] == ']') {
	CString Result = iStr.Copy(curPos, workingPos - curPos);
	curPos = workingPos+1;
	return Result;
      } else return "";
    } else return "";
  } else return "";
}

char CMail::MPQuotedPair(const CString& iStr, int& curPos){

#ifdef M_DEBUG
  cout << "MPQuotedPair() - " << curPos << endl;
#endif

  if (curPos < iStr.StrLength()) {
    if (iStr[curPos] == '\\') {
      curPos++;
      return iStr[curPos];
    } else return 0;
  } else return 0;
}
