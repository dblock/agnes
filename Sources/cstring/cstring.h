#ifndef CSTRING_H
#define CSTRING_H

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <iostream.h>
#include <stdio.h>
#include "../platform/platform.h"
#include "../cvector/cvector.h"

#define STR_STATIC_BUFSIZE 18

class CString;
typedef void (* GetTermFunction)(CString& Term);

class CString {
 private:
  char *      m_DStr;
  char        m_SStr[STR_STATIC_BUFSIZE];
  char *      m_Str;
  int         m_Len;
  int         m_Size;
  friend istream& operator>>(istream&,CString&);
  friend ostream& operator<<(ostream&,const CString&);  
  inline void Assign(char * Buffer, int Len, int Size);
public:
  inline void SetSize(int dwSize);
  inline char * asString(void) const { return m_Str; } 
  CString& Quote(void);
  CString& Dequote(void);  
  inline int operator==(const CString& s) const { return isEqual(s); }
  inline int operator==(const char * s) const { return isEqual(s); }
  inline int operator!=(const CString& s) const { return !isEqual(s); }
  inline int operator!=(const char * s) const { return !isEqual(s); }
  CString();
  CString(const char *, int Len = -1);
  CString(const CString&);
  ~CString();
  inline int StrLength(void) const { return m_Len; }
  inline void Free(void);
  inline CString ExtractLine(void) { CString Target; ExtractLine(Target); return Target; }
  CString& ExtractLine(CString&);
  int isSubStr(const CString&) const ;
  int isSubStr(char *) const ;
  int isSubStr(const char *, int) const;
  int isSubStrI(char *) const ;
  int isSubStrI(const CString &) const ;
  int isSubStrI(const CString& s, int startpos) const ;
  int isSubStrI(const char * s, int startpos) const ;
  int isSubStr(const int) const ;
  inline int Val(void) const { return atoi(m_Str); }
  inline long ValLong(void) const { return atol(m_Str); }
  inline float ValFloat(void) const {  return (float) atof(m_Str); }
  inline int isEqual(const CString &) const;
  inline int isEqual(const char *) const;
  inline int isSame(const CString &) const;
  inline int isSame(const char *) const;
  int operator<(const CString&) const;
  int operator>(const CString&) const;
  int operator>=(const CString&) const;
  int operator<=(const CString&) const;
  const CString& operator=(const CString&);
  const CString& operator=(const char* );
  void StrF(void);
  void StrDelete(int start, int count);
  void ChrAppend(const char ch);  
  void StrAppend(const char *, int jLen = -1);
  inline CString& operator+=(const CString& s) { StrAppend(s.m_Str, s.StrLength()); return(*this); }
  CString& operator+=(const char * s) { StrAppend(s); return(*this); }
  CString& operator+=(const int i);
  CString& operator+=(const long i);
  CString& operator+=(const float i);
  CString& operator+=(const double i);
  CString& operator+=(const char i);
  inline char operator[](const int i) const { return m_Str[i]; }
  inline char& operator[](const int i) { return m_Str[i]; }
  inline char GetChar(const int i) const { return m_Str[i]; }
  inline const char& GetRef(const int i) const { return m_Str[i]; }
  inline void SetChar(const int i, const char c) { m_Str[i] = c; }
  int hash_sum(void);
  CString& remove_br(void);
  CString& add_br(void);
  CString& Flush(CString&);
  CString& StrInsert(int, const CString&);
  inline CString& StrTrim32(void);
  inline CString& StrTrim(void);
  inline CString& StrTrim(char, char);  
  CString& StrTrimRight(char = ' ', char = ' ');
  CString& StrTrimLeft(char = ' ', char = ' ');  
  CString Copy(int, int) const;
  CString& Copy(int, int, CString&) const;
  inline int Pos(const char c) const { return Pos(c, 0); }
  inline int InvPos(const char) const;
  inline int Pos(const char, int) const;  
  inline CVector<CString> CString::Tokenizer(const CString& iStr) const { return Tokenizer(iStr, 0); }
  inline CVector<CString> CString::Tokenizer(const char * str) const { return Tokenizer(str, 0); }
  inline CVector<CString>& CString::Tokenizer(const CString& iStr, CVector<CString>& retVector) const { return Tokenizer(iStr, retVector, 0); }
  CVector<CString> Tokenizer(const char c, int Sorted = 0) const;
  CVector<CString>& Tokenizer(const char c, CVector<CString>&, int Sorted = 0) const;
  CVector<CString> CString::Tokenizer(const CString&, int Sorted) const;
  inline CVector<CString> CString::Tokenizer(const char * str, int Sorted) const { CString Dummy(str); return Tokenizer(Dummy, Sorted); }
  CVector<CString>& CString::Tokenizer(const CString&, CVector<CString>&, int Sorted) const;
  CString& Encrypt(void);
  CString& Decrypt(void);
  int EndsWith(const CString&) const;
  int StartsWith(const CString &) const;
  int StartsWith(const char *) const;
  inline CString& UpCase(void);
  static inline char ToUpper(char ch);
  static inline char ToLower(char ch);
  inline CString& LowerCase(void);
  inline int HasUpcase(int Pos = 0) const;
  inline int HasLowercase(int Pos = 0) const;
  int strNStr(const CString& s) const;
  inline CString& append_backslash(void);
  inline CString& remove_backslash(void);
  inline CString& append_if_missing(char ch);
  CString& remove_forward(char ch);
  inline CString& remove_forward_slash(void) { if (m_Len) { if ((m_Str[0]=='\\')||(m_Str[0]=='/')) StrDelete(0, 1); } return(* this); }
  void fmakeword(FILE * f, char stop, int * cl);
  char x2c(char * what);
  CString& Appendc2x(char c, char e = '`');
  CString& unescape_url(void);
  CString& escape_url(void);
  CString& escape(void);
  CString& unescape(void);
  void untreat_stupid(void);
  inline CString& Replace(char c, char t);
  inline CString Map(const CVector<CString>& SVector) const { CString Tmp; Map((* this), SVector, Tmp); return Tmp; }
  CString& Map(const CString& Expression, const CVector<CString> & SVector, CString& Target) const;
  inline CString MapTerm(GetTermFunction Func) const { CString Target; MapTerm(Func, Target); return Target; }
  CString& MapTerm(GetTermFunction Func, CString& Target) const;
  CString& ContractSpaces(void);
  CString& ContractSpaces(int);
  CString& Base64Encode(void);
  CString& Base64Decode(void);
  CString& FormatC(void);
  void SetBuf(const char * Buf, int Len = 0);
};

inline CString& CString::append_backslash(void){  
#ifndef ANSI
	return append_if_missing('\\');
#else
	return append_if_missing('/');
#endif
	return (* this);
}

inline CString& CString::remove_backslash(void){  
#ifndef ANSI
	return remove_forward('\\');
#else
	return remove_forward('/');
#endif
	return (* this);
}
    
inline CString& CString::append_if_missing(char ch){  
  if ((!m_Len || m_Str[m_Len-1] != ch)) 
	  ChrAppend(ch);
  return (* this);
}

inline int CString::isEqual(const CString& s) const {
  if ((!m_Len)||(!s.m_Len)) return 0;
  return(!strcmpi(m_Str, s.m_Str));
}

inline int CString::isEqual(const char * s) const {
  if ((m_Len)&&(!s)) return 0;
  if ((!m_Len)&&(!s)) return 1;
  return(!strcmpi(m_Str, s));
}

inline int CString::isSame(const CString& s) const {
  if ((!m_Len)&&(!s.m_Len)) return 1;
  if (m_Len != s.m_Len) return 0;
  return(!strcmp(m_Str, s.m_Str));
}

inline int CString::isSame(const char * s) const {
  if ((m_Len)&&(!s)) return 0;
  if ((!m_Len)&&(!s)) return 1;
  return(!strcmp(m_Str, s));
}

inline CString& CString::Replace(char c, char t) {
  if (m_Len){
    for (int i=m_Len;i>=0;i--) 
      if (m_Str[i] == c) {
	m_Str[i] = t;
      }
  }
  return(* this);
}

inline char CString::ToUpper(char ch) {
    if ((ch <= 'z')&&(ch >= 'a'))
        ch -= ('a' - 'A');
    return ch;
}
    
inline char CString::ToLower(char ch) {
    if ((ch <= 'Z')&&(ch >= 'A'))
        ch += ('a' - 'A');
    return ch;
}
    
inline CString& CString::UpCase(void){
    if (m_Len) {
        for (int i = 0;i<m_Len;i++) 
            m_Str[i] = ToUpper(m_Str[i]);
    }
    return(* this);
}

inline int CString::HasLowercase(int Pos) const {
  if (m_Len > Pos) {
    for (int i = Pos;i<m_Len;i++) 
      if ((m_Str[i]<='z')&&(m_Str[i]>='a'))
	return 1;    
  }
  return 0;
}

inline int CString::HasUpcase(int Pos) const {
  if (m_Len > Pos) {
    for (int i = Pos;i<m_Len;i++) 
      if ((m_Str[i]<='Z')&&(m_Str[i]>='A'))
	return 1;    
  }
  return 0;
}

inline CString& CString::LowerCase(void){
    if (m_Len) {
        for (int i = 0;i<m_Len;i++) 
            m_Str[i] = ToLower(m_Str[i]);  
    }
    return(* this);
}

inline int CString::EndsWith(const CString& iStr) const {
  if (m_Len < iStr.m_Len) return 0;
  if (strcmpi(iStr.m_Str, m_Str + m_Len - iStr.m_Len)) return 0;
  return 1;
}

inline int CString::StartsWith(const char * iStr) const {   
  if (!iStr) return 1;
  if (((int)m_Len - (int)strlen(iStr)) < 0) { return 0; }   
  if (m_Str) return (!(strncmpi(m_Str, iStr, strlen(iStr)))); else return 0;
}

inline int CString::StartsWith(const CString& iStr) const {
  if (m_Len < iStr.m_Len) return 0;
  return (!(strncmpi(m_Str, iStr.asString(), iStr.m_Len)));
}

inline int CString::InvPos(const char c) const {
  if (!m_Len) return -1;
  for (int i=m_Len-1;i>=0;i--) if (m_Str[i]==c) return(i);
  return -1;
}

inline int CString::Pos(const char c, int start) const {
  if (!m_Len) return -1;
  if (start >= m_Len) return -1;
  for (int i=start;i<m_Len;i++) if (m_Str[i]==c) return i;
  return -1;
}

inline CString CString::Copy(int start, int len) const {
  CString ret;
  Copy(start, len, ret);
  return ret;
}

inline CString& CString::StrTrim32(void){
  StrTrimRight(1, 32);
  StrTrimLeft(1, 32);
  return(*this);
}

inline CString& CString::StrTrim(void){
  StrTrimRight();
  StrTrimLeft();
  return(*this);
}

inline CString& CString::StrTrim(char l, char r){
  StrTrimRight(l, r);
  StrTrimLeft(l, r);
  return(*this);
}

inline void CString::SetSize(int dwSize) {
  if (dwSize < 0) return;
  if (m_Size >= dwSize) return;

  int Size = dwSize;
  if (dwSize < 1000000) {
    Size = m_Size << 1;
    while ( Size < dwSize)
      Size <<= 1;  
  }
  
  char * NewBuffer = new char[Size];
  memcpy(NewBuffer, m_Str, m_Len + 1);
  if (m_DStr) delete[] m_DStr;
  m_DStr = NewBuffer;
  m_Str = m_DStr;      
  m_Size = dwSize;  
}

inline void CString::Assign(char * Buffer, int Len, int Size) {
  if (m_DStr) 
    delete[] m_DStr;
  m_DStr = Buffer;
  m_Size = Size;
  m_Len = Len;
  m_Str = m_DStr;
}

inline void CString::Free(void) {
  if (m_DStr) 
    delete[] m_DStr;
  
  m_Size = STR_STATIC_BUFSIZE;
  m_Len = 0;
  m_SStr[0] = 0;
  m_Str = m_SStr;
  m_DStr = 0;
}
 
 
extern char EmptyChar[];
extern const long int C1;
extern const long int C2;

#define callMemberFunction(object,ptrToMember)  ((object).*(ptrToMember))

extern void CStringQSort(CVector<CString> & vector,  int (*cmp_function) (const CString&, const CString&));
extern CString bool_to_space(int bvalue);
typedef CString& (CString::*CStringMemberFn)(void);
extern void applyCStringVector(CVector<CString>& vector, CStringMemberFn memCString);
typedef CVector<CString> CStringVector;

#endif

