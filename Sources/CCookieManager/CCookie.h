/*
  CCookie
  */

#ifndef C_COOKIE_CLASS_H
#define C_COOKIE_CLASS_H

#include <cstring/cstring.h>
#include <cvector/cvector.h>

class CCookie {
public:
  int HasChanged;
  CCookie(void);
  CCookie(const CString&, const CString&);
  CCookie(const CCookie&);
  ~CCookie(void);
  CCookie& operator=(const CCookie&);
  int operator!=(const CCookie&) const;
  int operator==(const CCookie&) const;
  int operator>(const CCookie&) const;
  int operator<(const CCookie&) const;
  int operator>=(const CCookie&) const;
  int operator<=(const CCookie&) const;
  friend ostream& operator<<(ostream&, const CCookie&);
  CString asString(void) const;
  friend class CCookieManager;
  CString GetName(void) const;
  CString GetValue(void) const;
private:
  CCookie& Clone(const CCookie&);
  CString Name;
  CString Value;
};

#endif
