#ifndef email_h
#define email_h

#include "../cstring/cstring.h"
#include "inetsock.h"

class CEmail{
public:
	CString from;
	CString to;
	CString name;
	CString subject;
	CString contents;

	CEmail(void);
	~CEmail(void);
	void Send(const CString&);
	void Send(inetSocket& mail_sock);

	void expect110_ok(inetSocket&);
	void expect25_ok(inetSocket&, int code);
	void send25_ok(inetSocket&, const CString& string, int code);
	void send110_ok(inetSocket&, const CString& string);

	CString conversation;
};

#endif
