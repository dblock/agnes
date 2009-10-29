#ifndef inetsock_h
#define inetsock_h

#include "../platform/platform.h"

#ifdef ANSI
#include <sys/time.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#else
#include <io.h>
#include <winsock.h>
#endif

#include "../cstring/cstring.h"

extern CString wsLastError;

class inetSocket {
private:
	short int Port;
	CString Name;
	struct sockaddr_in A;
	struct hostent *H;
public:
	int Error;
	int S;
	void Reopen(void);
	void Open(void);
	void Connect(void);
	void Close(void);
	int Poll(int Read, int Write, int Timeout);
	void wserror(const CString& error);
	inetSocket(int port, const CString& name);
	~inetSocket(void);
	void SetBlockMode(int BlockMode = 1);
};

#endif
