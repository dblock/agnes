#ifndef nextgen_v2_h
#define nextgen_v2_h

#define MAXLONGCHAR 12

#include "../cstring/cstring.h"
#include "../cvector/cvector.h"

extern CString proxy;

#define MAX_RECVBUF 12

class nextgen_v2{
public:	
	CString fserver;
	CString root;
	CString contained;
	CString cache;
	int cached;
	nextgen_v2(const CString& container);
	nextgen_v2(const CString& container, const CString&, int);
	nextgen_v2(const CString& server, int port, const CString& document);
	nextgen_v2(const CString& server, int port, const CString& document, const CString&, int);
	nextgen_v2(const CString& container, const CString& DateModified);
	void common_nextgen_v2(const CString&, int, const CString&);
	void common_nextgen_v2(const CString&);
	~nextgen_v2(void);
	int feofc(void);
	CString read_line(void);
	CString& read_line(CString&);
	long int nv2_fsize(void);
	void rewind(void);
	int void_read;
	int Advance(const CString&);
	CString fdocument;
	int displacement;
	int Modified;
	int line_count(void);
	static CString check_dir(const CString&, int HasFile = 0);  
	static CString get_curdir(void);
	static CString get_rootdir(void);
    static bool FileExists(const CString&);
private:
	CString DateModified;
	int fport;
	long int fs;
	void buffer_read(const CString& filename, long int & fs);
	CString fcontainer;
	long int read_integer(void);
	void reopen(const CString& container);
	void reopen(const CString& server, int port, const CString& document);
	void retreive_page(const CString& server, int port, const CString& document);
	CString http_path(const CString&);
};

#endif
