#include <agnespch.hpp>

#ifndef ANSI
	#define write(x,y,z) send(x,y,z,0)
#endif

//#define cgflush if (SocketOutput == 1) flush()

cgiOutStream::cgiOutStream(void){
  Socket = 0;
  Locked = 1;
  SocketOutput = 0;  
  Compress = 1;
#ifdef ANSI
  signal(SIGPIPE, SIG_IGN);
#endif
}

cgiOutStream::cgiOutStream(int iSocket){
#ifdef ANSI  
  signal(SIGPIPE, SIG_IGN);
#endif  
  Socket = iSocket;
  Locked = 1;
  SocketOutput = 1;
  Compress = 0;
}

cgiOutStream::~cgiOutStream(void){
  flush();
}

cgiOutStream& cgiOutStream::operator<<(char c){
  if ((!SocketOutput)&&(!Locked)) {
    cout << c; 
#ifdef _DEBUG
    if (debug) cout.flush();
#endif
  } else Buffer+=c;
#ifdef cgflush
  cgflush;
#endif
  return (* this);
}

cgiOutStream& cgiOutStream::operator<<(unsigned char c) { 
  return (*this) << (char)c; 
}

cgiOutStream& cgiOutStream::operator<<(signed char c) { 
  return (*this) << (char)c; 
}

cgiOutStream& cgiOutStream::operator<<(cgiOutStream *){
  return (* this);
}

cgiOutStream& cgiOutStream::operator<<(const CString& iStr){  
  if ((!SocketOutput)&&(!Locked)) {
    cout << iStr; 
#ifdef _DEBUG
    if (debug) cout.flush();
#endif
  }
  else Buffer+=iStr;
#ifdef cgflush
  cgflush;
#endif
  return (* this);
}

cgiOutStream& cgiOutStream::operator<<(const char * s){  
  if ((!SocketOutput)&&(!Locked)) {
    cout << s;
#ifdef _DEBUG
    if (debug) cout.flush();
#endif
  }
  else Buffer+=s;
#ifdef cgflush
  cgflush;
#endif
  return (* this);
}

cgiOutStream& cgiOutStream::operator<<(const unsigned char *s){   
  return (*this) << (const char*)s; 
}

cgiOutStream& cgiOutStream::operator<<(const signed char *s){  
  return (*this) << (const char*)s; 
}

cgiOutStream& cgiOutStream::operator<<(int n){  
  if ((!SocketOutput)&&(!Locked)) {
    cout << n;
#ifdef _DEBUG
    if (debug) cout.flush();
#endif
  }
  else Buffer+=n;
#ifdef cgflush
  cgflush;
#endif
  return (* this);
}

cgiOutStream& cgiOutStream::operator<<(unsigned int n){  
  if ((!SocketOutput)&&(!Locked)) {
    cout << n;
#ifdef _DEBUG
    if (debug) cout.flush();
#endif
  }
  else Buffer+=(int) n;
#ifdef cgflush
  cgflush;
#endif
  return (* this);
}

cgiOutStream& cgiOutStream::operator<<(long n){  
  if ((!SocketOutput)&&(!Locked)) {
    cout << n;
#ifdef _DEBUG
    if (debug) cout.flush();
#endif
  }
  else Buffer+=n;
#ifdef cgflush
  cgflush;
#endif
  return (* this);
}

cgiOutStream& cgiOutStream::operator<<(unsigned long n){  
  if ((!SocketOutput)&&(!Locked)) {
    cout << n;
#ifdef _DEBUG
    if (debug) cout.flush();
#endif
  }
  else Buffer+=(long) n;
#ifdef cgflush
  cgflush;
#endif
  return (* this);
}

cgiOutStream& cgiOutStream::operator<<(short n) {  
  return operator<<((int)n);
}

cgiOutStream& cgiOutStream::operator<<(unsigned short n) {  
  return operator<<((unsigned int)n);
}

cgiOutStream& cgiOutStream::operator<<(double n){  
  if ((!SocketOutput)&&(!Locked)) {
    cout << (int) n;
#ifdef _DEBUG
    if (debug) cout.flush();
#endif
  }
  else Buffer+=(int) n;
#ifdef cgflush
  cgflush;
#endif
  return (* this);
}

cgiOutStream& cgiOutStream::operator<<(float n) {   
  return operator<<((double)n); 
}

cgiOutStream& cgiOutStream::flush_fork(void){
  if (Compress) Buffer.ContractSpaces();  
  write(Socket, Buffer.asString(), Buffer.StrLength());
  Buffer.Free();
  return (* this);
}

cgiOutStream& cgiOutStream::flush(void){  
  if (Buffer.StrLength()){
    if (!SocketOutput) {
      cout << Buffer;
      cout.flush();
      Buffer.Free();
      Locked = 0;
    } else if (SocketOutput == 1) {
      if (Compress) Buffer.ContractSpaces();
      if ((Buffer.StrLength())&&(write(Socket, Buffer.asString(), Buffer.StrLength()) < 0)) {
	//cerr << "CRStreamSock&::flush() - error sending to socket, class instance flush shall be ignored" << endl;
	SocketOutput = -1;
	Buffer.Free();
      } else Buffer.Free();
    } else if (SocketOutput == -1) {
      Buffer.Free();
    }
  }
  return (* this);
}

void cgiOutStream::toCOUT(void){
  flush();
  SocketOutput = 0;
}

void cgiOutStream::toSOCK(int iSocket){
  flush();
  SocketOutput = 1;
  Socket = iSocket;
}

void cgiOutStream::toSTALL(void){
  flush();
  SocketOutput = -1;
}

void cgiOutStream::Lock(void){
  Locked = 1;
}

void cgiOutStream::UnLock(void){
  Locked = 0;
  flush();
}

void cgiOutStream::ContractSpaces(void){
  Buffer.ContractSpaces();
}
