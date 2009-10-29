///////////////////////////////////////////////////////////////////////
// RegExp.h
//
// This code has been derived from work by Henry Spencer. 
// The main changes are
// 1. All char variables and functions have been changed to TCHAR
//    counterparts
// 2. Added GetFindLen() & GetReplaceString() to enable search
//    and replace operations.
// 3. And of course, added the C++ Wrapper
//
// The original copyright notice follows:
//
// Copyright (c) 1986, 1993, 1995 by University of Toronto.
// Written by Henry Spencer.  Not derived from licensed software.
//
// Permission is granted to anyone to use this software for any
// purpose on any computer system, and to redistribute it in any way,
// subject to the following restrictions:
//
// 1. The author is not responsible for the consequences of use of
// this software, no matter how awful, even if they arise
// from defects in it.
//
// 2. The origin of this software must not be misrepresented, either
// by explicit claim or by omission.
//
// 3. Altered versions must be plainly marked as such, and must not
// be misrepresented (by explicit claim or omission) as being
// the original software.
//
// 4. This notice must not be removed or altered.
/////////////////////////////////////////////////////////////////////////////

#define NSUBEXP  10

#include "../platform/platform.h"

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#ifdef ANSI
#define TCHAR char
#define size_t int
#define BOOL int
#define TRUE 1
#define FALSE 0
#define _tcslen strlen
#define _tcscspn strcspn
#define _tcsspn strspn
#define _tcsstr strstr
#define _tcsncpy strncpy
#define _tcschr strchr
#define _tcsncmp strncmp
#define _T(x) (char)x
#else
#include <tchar.h>
#include <windows.h>
#endif

class CRegExp{
public:
  CRegExp();
  ~CRegExp();
  TCHAR * LastError;
  CRegExp *RegComp( const TCHAR *re );
  int RegFind(const TCHAR *str);
  TCHAR* GetReplaceString( const TCHAR* sReplaceExp );
  int GetFindLen();    
private:
  TCHAR *regnext(TCHAR *node);
  void reginsert(TCHAR op, TCHAR *opnd);
  int regtry(TCHAR *string);
  int regmatch(TCHAR *prog);
  size_t regrepeat(TCHAR *node);
  TCHAR *reg(int paren, int *flagp);
  TCHAR *regbranch(int *flagp);
  void regtail(TCHAR *p, TCHAR *val);
  void regoptail(TCHAR *p, TCHAR *val);
  TCHAR *regpiece(int *flagp);
  TCHAR *regatom(int *flagp);
  TCHAR OP(TCHAR *p) {return *p;};
  TCHAR *OPERAND( TCHAR *p) {return (TCHAR*)((short *)(p+1)+1); };
  void regc(TCHAR b); /* regc - emit (if appropriate) a byte of code */
  TCHAR * regnode(TCHAR op); /* regnode - emit a node */ 
  
  BOOL bEmitCode;
  BOOL bCompiled;
  TCHAR *sFoundText;  
  TCHAR *startp[NSUBEXP];
  TCHAR *endp[NSUBEXP];
  TCHAR regstart;
  TCHAR reganch;
  TCHAR *regmust;
  int regmlen;
  TCHAR *program;

  TCHAR *regparse;        /* Input-scan pointer. */
  int regnpar;            /* () count. */
  TCHAR *regcode;         /* Code-emit pointer; &regdummy = don't. */
  TCHAR regdummy[3];      /* NOTHING, 0 next ptr */
  long regsize;           /* Code size. */

  TCHAR *reginput;        /* String-input pointer. */
  TCHAR *regbol;          /* Beginning of input, for ^ check. */
	
};


