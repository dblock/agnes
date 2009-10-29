#ifndef _mozilla_platform
#define _mozilla_platform

#ifndef __cplusplus
#error C++ Compiler Required
#endif

#define elf "<br>\n";

#if !defined(_WIN32) && !defined(_MAC)
#define ANSI
#else
#undef ANSI
#endif

#include <iostream.h>
#include <fstream.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#ifdef ANSI
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream.h>
#include <fcntl.h>
#include <fstream.h>
#include <dirent.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <sys/types.h>

#define strcmpi strcasecmp
#define strncmpi strncasecmp
#define _chdir chdir
#define v_open(x,y,z) open((x),(y),(z))
#define PATH_SEPARATOR '/'

#else

#include <time.h>
#include <process.h>
#include <sys/stat.h>
#include <winsock.h>

#pragma warning( disable : 4660 )
#define PATH_SEPARATOR '\\'

#define MAXPATHLEN _MAX_PATH
#define getpid _getpid
#include <io.h>	
#include <direct.h>
#define strncmpi strnicmp
#define v_open(x,y,z) open((x),(y))
#endif

class cgiOutStream;

#ifdef ANSI
#include "../../platform/config.hpp"
#endif

#endif

