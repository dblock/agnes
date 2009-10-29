/*
  CMail.h - email parser / RFC 822
 */

#ifndef C_EMAIL_H_RFC822
#define C_EMAIL_H_RFC822

#include "../cstring/cstring.h"

class CMail {
private:
  int FGood;
  CString FMail;  
  CString FError;
  CString FDomain;
  CString FUser;
  int In(const char& What, const CString& Where);
  int Parse(void);
  CString MPAddress(const CString& iStr, int& curPos);
  CString MPGroup(const CString& iStr, int& curPos);
  CString MPMailbox(const CString& iStr, int& curPos);
  CString MPPhrase(const CString& iStr, int& curPos);
  CString MPRouteAddr(const CString& iStr, int& curPos);
  CString MPRoute(const CString& iStr, int& curPos);
  CString MPAddressSpec(const CString& iStr, int& curPos);
  CString MPLocalPart(const CString& iStr, int& curPos);
  CString MPWord(const CString& iStr, int& curPos);
  CString MPQuotedString(const CString& iStr, int& curPos);
  char MPQuotedPair(const CString& iStr, int& curPos);
  char MPQText(const CString& iStr, int& curPos);
  CString MPDomain(const CString& iStr, int& curPos);
  CString MPSubDomain(const CString& iStr, int& curPos);
  CString MPDomainRef(const CString&, int&);
  char MPAtom(const CString& iStr, int& curPos);
  CString MPAtomSequence(const CString& iStr, int& curPos);  
  char MPDtext(const CString& iStr, int& curPos);
  char MPLinearWhiteSpace(const CString& iStr, int& curPos);
  CString MPDomainLiteral(const CString& iStr, int& curPos);
public:
  int isGoodInternet(void);
  CString GetError(void);
  CMail(void);
  virtual ~CMail(void);
  CMail(const CString&);
  CString GetUser(void);
  CString GetHost(void);
  void Set(const CString&);
  int isGood(void);
};

#endif
