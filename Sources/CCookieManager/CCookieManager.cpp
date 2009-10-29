#include <agnespch.hpp>

CCookieManager * CurrentCookieManager = 0;

CCookieManager::CCookieManager(void){
  CurrentCookieManager = this;
}

CCookieManager::CCookieManager(const CString& CookieString){
  CurrentCookieManager = this;
  Read(CookieString);
}

CCookieManager::CCookieManager(char * CookieString){
  CurrentCookieManager = this;
  Read(CookieString);
}

CCookieManager::~CCookieManager(void){
}

void CCookieManager::Clear(void){
  Cookies.Clear();
}

CCookie * CCookieManager::Find(const CString& Name){
  for (int i=0;i<Cookies.Count();i++)
    if (Cookies[i].Name == Name) return &Cookies[i];
  return 0;
}

void CCookieManager::Del(const CString& Name){
  CCookie * Current = Find(Name);
  if (Current) {
    Current->Value.Free();
    Current->HasChanged = 1;
  }
}

CString CCookieManager::Get(const CString& Name){
  CCookie * Current = Find(Name);
  if (Current) return Current->Value; else return "";
}



void CCookieManager::Add(const CString& Name, const CString& Value){
  CCookie * Current = Find(Name);
  if (!Current) {
    CCookie Cookie(Name, Value);
    Cookie.HasChanged = 1;
    Cookies+=Cookie;
  } else {
	Current->Value = Value;
	Current->HasChanged = 1;
  }
}

void CCookieManager::Read(char * Full){
  Clear();
  if (Full) {
    CString Tmp(Full);
    Read(Tmp);
  }
}

void CCookieManager::Read(const CString& Full) {
  Clear();
  if (!Full.StrLength()) return;
  CVector<CString> Tokens;
  Full.Tokenizer(';', Tokens);
  for (int i=0;i<Tokens.Count();i++) {
    int ePos = Tokens[i].Pos('=');
    if (ePos > 0) {
      CString Name = Tokens[i].Copy(0, ePos).StrTrim32();
      CString Value = Tokens[i].Copy(ePos+1, Tokens[i].StrLength()).StrTrim32();
      Add(Name, Value);
    }
  }
}

CString CCookieManager::UpdateString(void){
  CString tmp;
  for (int i=0;i<Cookies.Count();i++) 
    if (Cookies[i].HasChanged) {
      tmp+="Set-cookie: ";      
      tmp+=Cookies[i].GetName();
      tmp+="="; 
      tmp+=Cookies[i].GetValue();
      tmp+=";path=/;\n";
//expires=Wednesday, 1-Jan-1977 00:00:00 GMT;";            
    }
  //CString Another(tmp);
  //cout << "\nUpdateCookie[" << Another.unescape_url() << "]\n";
  return tmp;    
}

#include <cvector/cvector.cpp>
template class CVector<CCookie>;
template class ICVector<CCookie>;
