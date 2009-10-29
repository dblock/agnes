#include <agnespch.hpp>

CString Proxy;

int nextgen_v2::line_count(void){
	int Result = 0;
	int iPos = contained.Pos('\n', displacement);
	while (iPos >= 0) {
		Result++;
		iPos = contained.Pos('\n', iPos+1);
	}
	return Result;
}

int nextgen_v2::Advance(const CString& item){
	int res = contained.isSubStr(item.asString(), displacement);
	if (res) {
		displacement = res;
		//contained.StrDelete(0, res);
		return 1;
	} else return 0;
}

CString nextgen_v2::get_curdir(void) {
	CString Result;
	char Buffer[MAXPATHLEN+1];
#ifdef ANSI
	if (getcwd(Buffer, sizeof(Buffer)/sizeof(Buffer[0]) - 1)) Result = Buffer;
	Result.append_backslash();
	return Result;
#else
	int nSize = GetCurrentDirectory(sizeof(Buffer)/sizeof(Buffer[0]) - 1, Buffer);
	if (nSize <= 0)
		return "error getting current directory";
	Buffer[nSize] = 0;
	Result = Buffer;
	Result.append_backslash();
	return Result;	
#endif
}

CString nextgen_v2::get_rootdir(void) {
	CString Result;
	char Buffer[MAXPATHLEN+1];
#ifdef ANSI
	if (getcwd(Buffer, sizeof(Buffer)/sizeof(Buffer[0]) - 1)) Result = Buffer;
#else
	int nSize = GetModuleFileNameA(NULL, Buffer, MAXPATHLEN);
	if (nSize < MAXPATHLEN) {
		Buffer[nSize] = 0;
		while (--nSize) {
			if (Buffer[nSize] == '\\' || Buffer[nSize] == '/')
				break;
			Buffer[nSize] = 0;
		}
		Result = Buffer;
	}
#endif
	Result.append_backslash();
	return Result;
}

CString nextgen_v2::check_dir(const CString& dir, int HasFile) {
	CString Preserve = get_curdir();
	
	CString Result;
	CString MakeDir; 
#ifdef ANSI
	if ((dir.StrLength() == 0)||(dir[0] != PATH_SEPARATOR))
		MakeDir = get_curdir();
#else
	if (!dir.StrLength() || (dir[0] != PATH_SEPARATOR)) {
		if ((dir.StrLength() < 2)||(dir[1] != ':')) 
			MakeDir = get_curdir();
	}
#endif
	MakeDir += dir;
	if (!HasFile) MakeDir.append_backslash();


#ifdef ANSI
	// cout << "making [" << MakeDir << "]" << endl;
	int prevPos = 0;
	int curPos = MakeDir.Pos(PATH_SEPARATOR);
	while (curPos >= 0) {
		Result += MakeDir.Copy(prevPos, curPos - prevPos);
		Result.append_backslash();
		// cout << "checking [" << Result.asString() << "]" << endl;
		if (chdir(Result.asString()) == -1) {
			if (mkdir(Result.asString() 
#ifdef ANSI
				, S_IRWXU
#endif
				) == -1) {
				break;
			}
		}
		prevPos = curPos+1;
		curPos = MakeDir.Pos(PATH_SEPARATOR, prevPos);
	}
#else
	if (! CreateDirectory(MakeDir.asString(), NULL))
	{
		Result = "error creating ";
		Result += MakeDir;
		Result += ", error code=";
		Result += (int) GetLastError();		
	}
	else
	{
		Result = MakeDir;
	}
#endif

	chdir(Preserve.asString());
	return Result;
}

CString nextgen_v2::http_path(const CString& url){
	if (url.StartsWith("http://")) return(url.Copy(strlen("http://"), url.StrLength() - strlen("http://")));
	else return url;
}

void nextgen_v2::reopen(const CString& server, int port, const CString& document){
	displacement = 0;
	void_read = 0;
	fcontainer.Free();
	fserver = server;
	fport = port;
	fdocument = document;
	retreive_page(server, port, fdocument);
	if (!(contained.StrLength())) {void_read = 1; fs = 0;} else {fs = contained.StrLength();}
}

void nextgen_v2::reopen(const CString& container){
    void_read = 0;
    displacement = 0;
    fcontainer = container;
    buffer_read(container, fs);
    if (!(contained.StrLength())) void_read = 1;
}

void nextgen_v2::rewind(void){
	displacement = 0;
	//if (fcontainer.StrLength()) reopen(fcontainer);
	//else reopen(fserver, fport, fdocument);
}

long int nextgen_v2::nv2_fsize(void){
	return (fs);
}

nextgen_v2::nextgen_v2(const CString& container, const CString& Modified){
	DateModified = Modified;
	cache.Free();
	cached = 0;
	CString filename_stream;
#ifdef ANSI
	for (int i=0;i<container.StrLength();i++) if (container[i] != '\\') filename_stream+=container[i]; else if (container[i+1] == '\\') filename_stream+='\\';  
#else
	filename_stream = container;
#endif  
	common_nextgen_v2(filename_stream);
}

nextgen_v2::nextgen_v2(const CString& container){
	cache.Free();
	cached = 0;
	CString filename_stream;
#ifdef ANSI
	for (int i=0;i<container.StrLength();i++) if (container[i] != '\\') filename_stream+=container[i]; else if (container[i+1] == '\\') filename_stream+='\\';  
#else
	filename_stream = container;
#endif  
	common_nextgen_v2(filename_stream);
}

nextgen_v2::nextgen_v2(const CString& container, const CString& _cache, int _cached){
	cache = _cache;
	cached = _cached;  
	common_nextgen_v2(container);
}

void nextgen_v2::common_nextgen_v2(const CString& container){  
	Modified = 1;
	if (!container.StrLength()) return;
	if (container.StartsWith("http://")) {
		CUrl FUrl(container);    
		reopen(FUrl.GetHost(), FUrl.GetPortValue(), FUrl.GetUrlPath());
	} else reopen(container); // if doesn't start with http://
}

void nextgen_v2::common_nextgen_v2(const CString& server, int port, const CString& document){
	reopen(server, port, document);
}

nextgen_v2::nextgen_v2(const CString& server, int port, const CString& document){
	cache = 0;
	cached = 0;
	common_nextgen_v2(server, port, document);
}

nextgen_v2::nextgen_v2(const CString& server, int port, const CString& document, const CString& _cache, int _cached){
	cache = _cache;
	cached = _cached;
	common_nextgen_v2(server, port, document);
}

nextgen_v2::~nextgen_v2(void){
}

CString nextgen_v2::read_line(void) {
	CString Result;
	read_line(Result);
	return Result;
	//return contained.ExtractLine();
}

CString& nextgen_v2::read_line(CString& Result){
	//contained.ExtractLine(Target);
	int iPos = contained.Pos('\n', displacement);
	if (iPos >= 0) {
		contained.Copy(displacement, iPos-displacement, Result);
		displacement = ++iPos;
	} else {
		contained.Copy(displacement, contained.StrLength(), Result);    
		displacement = contained.StrLength();
	}
	return Result;
}

int nextgen_v2::feofc(void){
	if (!contained.StrLength()) return 1;
	else if (void_read) return 1;
	else if (displacement == contained.StrLength()) return 1;
	else return 0;
}
#define DEBUG

void nextgen_v2::buffer_read(const CString& filename, long int & fsize){
	contained.Free();
	fsize = 0;
	displacement = 0;
	int Handle = open(filename.asString(), O_RDONLY);
	if (Handle == -1) return;
	fsize = lseek(Handle, 0, SEEK_END);  
	lseek(Handle, 0, SEEK_SET);
	char * buf = new char[fsize+1];
	fsize = read(Handle, buf, fsize);  
	buf[fsize] = 0;  
	contained.SetBuf(buf, fsize); 
	close(Handle);
}

void nextgen_v2::retreive_page(const CString& _server, int port, const CString& _document){
	CString Preserve = get_curdir();

	CString server(_server);
	CString document(_document);
	CString cached_complete;
	if (cache.StrLength() && (!chdir(cache.asString()))) {
		CString cached_document(_server);
#ifdef ANSI
		if ((document.StrLength() == 0)||(document[0] != '/')) 
#else
			if ((document.StrLength() == 0)||(document[0] != '\\'))
#endif
			cached_document.append_backslash();
			cached_document+=document;
			cached_complete = cache;
			cached_complete.append_backslash();
			cached_complete+=http_path(cached_document).remove_forward_slash();
			cached_document = check_dir(cached_document, 1);
			if (cached){
				reopen(cached_complete);
				if (!void_read){
					chdir(Preserve.asString());
					return;
				}	
			}
	}
	
	CString Full(_server);
	if (port) {
		Full+=":";
		Full+=port;
	}
	if ((!Full.StrLength() || (Full[Full.StrLength()-1]!='/')) &&
            (!_document.StrLength() || (document[0] != '/'))) 
          Full+='/';
	Full+=_document;  
	CRemoteFile FRemote(Full);
	FRemote.SetParameter("Pragma","no-cache");
        FRemote.SetParameter("User-agent", "aGNeS");
	
	if (Proxy.StrLength()) FRemote.Get(Proxy); else FRemote.Get();
	contained = FRemote.GetData();
	if (FRemote.GetStatusValue() != 200) {
		CString Error("<h3>Error retrieving:</h3><b>");        
		Error+=Full;    
		Error+=" (";
		Error+=FRemote.GetStatusValue();
		Error+=")<br><h4>";
		Error+=wsLastError;    
		Error+="</h4>";
		contained = Error;
	} else {
		if (cached_complete.StrLength()) {      
			int Handle = v_open(cached_complete.asString(), O_WRONLY | O_TRUNC | O_CREAT, S_IRWXU);
			if (Handle != -1) {
				write(Handle, contained.asString(), contained.StrLength());
				close(Handle);
			}
		}
	}
	chdir(Preserve.asString()); 
}

bool nextgen_v2::FileExists(const CString& Filename) {
#ifndef ANSI
	struct _stat stats;
	return (_stat(Filename.asString(), & stats) != -1);
#else
    struct stat stats;
	return (stat(Filename.asString(), & stats) != -1);
#endif	
}
