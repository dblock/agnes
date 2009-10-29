#ifndef OUTSTREAM_H
#define OUTSTREAM_H

#include "../cstring/cstring.h"

#ifndef ANSI
#include <windows.h>
#endif

class cgiOutStream {
private:  
  int Socket;
  int Locked;
public:
  int Compress;
  CString Buffer;
  int SocketOutput;
  cgiOutStream(int);
  cgiOutStream(void);
  ~cgiOutStream(void);
  cgiOutStream& flush(void);
  cgiOutStream& operator<<(char c);
  cgiOutStream& operator<<(unsigned char c);
  cgiOutStream& operator<<(signed char c);
  cgiOutStream& operator<<(const char *s);
  cgiOutStream& operator<<(const unsigned char *s);
  cgiOutStream& operator<<(const signed char *s);
  cgiOutStream& operator<<(int n);
  cgiOutStream& operator<<(unsigned int n);
  cgiOutStream& operator<<(long n);
  cgiOutStream& operator<<(unsigned long n);
  cgiOutStream& operator<<(short n);
  cgiOutStream& operator<<(unsigned short n);
  cgiOutStream& operator<<(double n);
  cgiOutStream& operator<<(float n);
  cgiOutStream& operator<<(cgiOutStream *);
  cgiOutStream& operator<<(const CString&);
  void toCOUT(void);
  void toSOCK(int iSocket);
  void toSTALL(void);
  void Lock(void);
  void UnLock(void);
  cgiOutStream& flush_fork(void);
  void ContractSpaces(void);
};

#endif
