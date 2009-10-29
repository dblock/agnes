#include <agnespch.hpp>

CString CCookie::GetName(void) const {
  return Name;
}

CString CCookie::GetValue(void) const {
  return Value;
}

CCookie::CCookie(void){
}

CCookie::CCookie(const CString& cName, const CString& cValue){
  Name = cName;
  Value = cValue;
  HasChanged = 0;  
}

CCookie::CCookie(const CCookie& Other){
  Clone(Other);
}

CCookie& CCookie::Clone(const CCookie& Other){
  HasChanged = Other.HasChanged;
  Name = Other.Name;
  Value = Other.Value;
  return (* this);
}

CCookie::~CCookie(void){
}

CCookie& CCookie::operator=(const CCookie& Other) {
  return Clone(Other);
}

int CCookie::operator!=(const CCookie& Other) const { 
  return (Name != Other.Name);
}

int CCookie::operator==(const CCookie& Other) const {
  return (Name == Other.Name);
}

int CCookie::operator>=(const CCookie& Other) const {
  return (Name >= Other.Name);
}

int CCookie::operator<=(const CCookie& Other) const {
  return (Name <= Other.Name);
}

int CCookie::operator>(const CCookie& Other) const {
  return (Name > Other.Name);
}

int CCookie::operator<(const CCookie& Other) const {
  return (Name < Other.Name);
}

ostream& operator<<(ostream& os, const CCookie& Cookie){
  os << Cookie.asString();
  return os;
}

CString CCookie::asString(void) const {
  CString tmp;
  tmp+=Name;
  tmp+="=";
  tmp+=Value;
  tmp+=";";
  return tmp;
}




