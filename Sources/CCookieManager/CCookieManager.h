/*
  CCookieManager
  */

#ifndef C_COOKIE_H
#define C_COOKIE_H

#include "CCookie.h"
#include <cstring/cstring.h>
#include <cvector/cvector.h>

class CCookieManager {
public:
  CCookieManager(void);
  CCookieManager(const CString&);
  CCookieManager(char *);
  void Clear(void); 
  void Add(const CString&, const CString&);
  void Del(const CString&);
  void Read(const CString&);
  void Read(char *);
  ~CCookieManager(void);
  CCookie * Find(const CString&);
  CString UpdateString(void);
  CString Get(const CString&);
private:
  CVector<CCookie> Cookies;
};

#endif
