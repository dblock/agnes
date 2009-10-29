#include <agnespch.hpp>

//#define _S_DEBUG

CString wsLastError;

void inetSocket::wserror(const CString& cError){
	wsLastError = Name;
	wsLastError += " - ";
	wsLastError += cError;
	Error = 1;
	return;
}

inetSocket::inetSocket(int cPort, const CString& cName) {
	Port = cPort;
	Name = cName;
	S = -1;
#ifndef ANSI
	WSADATA W;
	WSAStartup (0x101, &W);
#endif
	Reopen(); 

}



void inetSocket::Reopen(void) {

	Close();

	Open();

	if (Error) return;

	if ((Port != 110) && (Port != 25)) SetBlockMode(0);

	Connect();

}

void inetSocket::Open(void){
	Close();

	Error = 0; 
	wsLastError.Free();
	if ((S = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
		Error = 1;
		wsLastError = Name;
		switch(errno){
		case EACCES: wsLastError+=" - The process does not have appropriate privileges."; break;
#ifdef ANSI
		case EAFNOSUPPORT: wsLastError+=" - The implementation does not support the specified address family."; break;
		case EPROTOTYPE: wsLastError+=" - The socket type is not supported by the protocol."; break;
		case ENOBUFS: wsLastError+=" - Insufficient resources were available in the system to perform the operation."; break;
#ifdef ENOSR
		case ENOSR: wsLastError+=" - There were insufficient STREAMS resources available for the operation to complete."; break; 
#endif
		case EPROTONOSUPPORT: wsLastError+=" - The protocol is not supported by the address family, or the protocol is not supported by the implementation."; break;
#endif
		case EMFILE: wsLastError+=" - No more file descriptors are available for this process."; break;
		case ENFILE: wsLastError+=" - No more file descriptors are available for the system."; break; 
		case ENOMEM: wsLastError+=" - Insufficient memory was available to fulfill the request."; break; 
		default: wsLastError+=" - Unable to create socket, unexpected error."; break;
		}
		return;
	}
	A.sin_family=PF_INET;
	A.sin_port = htons(Port);
	unsigned long addr;
	if ((int)(addr = inet_addr(Name.asString())) != -1) { 
		A.sin_addr.s_addr = inet_addr(Name.asString());
	} else {
		H = gethostbyname(Name.asString());
		if(!H) {
			Error = 1;
			wsLastError = Name;
			switch(h_errno){
			case HOST_NOT_FOUND: wsLastError+=" - No such host is known."; break;
			case TRY_AGAIN: wsLastError+=" - A temporary and possibly transient error occurred, such as a failure of a server to respond."; break;
			case NO_RECOVERY: wsLastError+=" - An unexpected server failure occurred which can not be recovered."; break;
			case NO_DATA: wsLastError+=" - The server recognized the request and the name but no address is available. Another type of request to the name server for the domain might return an answer."; break;
			default: wsLastError+=" - gethostbyname() has failed, unexpected error."; break;
			}
			return;
		}
		A.sin_addr.s_addr=*((unsigned long *) H->h_addr); 
	}
}



void inetSocket::Connect(void) {

	Error = 1; 
	if (connect(S,(struct sockaddr *) &A, sizeof(A)) == -1) {     
		wsLastError = Name;
#ifdef ANSI
		switch(errno) {
		case EADDRNOTAVAIL: wsLastError+=" - The specified address is not available from the local machine."; return;
		case EAFNOSUPPORT: wsLastError+=" - The specified address is not a valid address for the address family of the specified socket."; return;
		case EALREADY: wsLastError+=" - A connection request is already in progress for the specified socket."; break;
		case EBADF: wsLastError+=" - The socket argument is not a valid file descriptor."; return;
		case ECONNREFUSED: wsLastError+=" - The target address was not listening for connections or refused the connection request."; return;
		case EINPROGRESS: wsLastError+=" - O_NONBLOCK is set for the file descriptor for the socket and the connection cannot be immediately established; the connection will be established asynchronously."; break;
		case EINTR: wsLastError+=" - The attempt to establish a connection was interrupted by delivery of a signal that was caught; the connection will be established asynchronously."; break;
		case EISCONN: wsLastError+=" - The specified socket is connection-mode and is already connected."; break;
		case ENETUNREACH: wsLastError+=" - No route to the network is present."; return;
		case ENOTSOCK: wsLastError+=" - The socket argument does not refer to a socket."; return;
		case EPROTOTYPE: wsLastError+=" - The specified address has a different type than the socket bound to the specified peer address."; return;
		case ETIMEDOUT: wsLastError+=" - The attempt to connect timed out before a connection was made."; return;
		case EADDRINUSE: wsLastError+=" - Attempt to establish a connection that uses addresses that are already in use."; return;
		case ECONNRESET: wsLastError+=" - Remote host reset the connection request."; return;
		case EHOSTUNREACH: wsLastError+=" - The destination host cannot be reached (probably because the host is down or a remoterouter cannot reach it)."; return;
		case EINVAL: wsLastError+=" - The address_len argument is not a valid length for the address family; or invalid address family in sockaddr structure."; return;
		case ENAMETOOLONG: wsLastError+=" - Pathname resolution of a symbolic link produced an intermediate result whose length exceeds PATH_MAX."; return;
		case ENETDOWN: wsLastError+=" - The local interface used to reach the destination is down."; return;
		case ENOBUFS: wsLastError+=" - No buffer space is available."; return;
#ifdef ENOSR
		case ENOSR: wsLastError+=" - There were insufficient STREAMS resources available to complete the operation."; return;
#endif
		case EOPNOTSUPP: wsLastError+=" - The socket is listening and can not be connected."; return;
		default: wsLastError+=" - connect() has failed, unexpected error."; break;
		}
#else
		switch(WSAGetLastError()) {
		case WSANOTINITIALISED: wsLastError+=" - A successful WSAStartup must occur before using this function."; return;
		case WSAENETDOWN: wsLastError+=" - The network subsystem has failed."; return;
		case WSAEADDRINUSE: wsLastError+=" - The socket's local address is already in use and the socket was not marked to allow address reuse with SO_REUSEADDR. This error usually occurs when executing bind, but could be delayed until this function if the bind was to a partially wild-card address (involving ADDR_ANY) and if a specific address needs to be committed at the time of this function."; return;
		case WSAEINTR: wsLastError+=" - The (blocking) Windows Socket 1.1 call was canceled through WSACancelBlockingCall."; return;
		case WSAEINPROGRESS: wsLastError+=" - A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function."; return;
		case WSAEALREADY: wsLastError+=" - A nonblocking connect call is in progress on the specified socket."; break;
		case WSAEADDRNOTAVAIL: wsLastError+=" - The remote address is not a valid address (such as ADDR_ANY)."; return;
		case WSAEAFNOSUPPORT: wsLastError+=" - Addresses in the specified family cannot be used with this socket."; return;
		case WSAECONNREFUSED: wsLastError+=" - The attempt to connect was forcefully rejected."; return;
		case WSAEFAULT: wsLastError+=" - The name or the namelen parameter is not a valid part of the user address space, the namelen parameter is too small, or the name parameter contains incorrect address format for the associated address family."; return;
		case WSAEINVAL: wsLastError+=" - The parameter is a listening socket, or the destination address specified is not consistent with that of the constrained group the socket belongs to."; return;
		case WSAEISCONN: wsLastError+=" - The socket is already connected (connection-oriented sockets only)."; return;
		case WSAENETUNREACH: wsLastError+=" - The network cannot be reached from this host at this time."; return;
		case WSAENOBUFS: wsLastError+=" - No buffer space is available. The socket cannot be connected."; return;
		case WSAENOTSOCK: wsLastError+=" - The descriptor is not a socket."; return;
		case WSAETIMEDOUT: wsLastError+=" - Attempt to connect timed out without establishing a connection."; return;
		case WSAEWOULDBLOCK: wsLastError+=" - The socket is marked as nonblocking and the connection cannot be completed immediately."; break;
		case WSAEACCES: wsLastError+=" - Attempt to connect datagram socket to broadcast address failed because setsockopt option SO_BROADCAST is not enabled."; return;
		default: wsLastError+=" - connect() has failed, unexpected error."; return;
		}
#endif // WIN32
	}    

	if (!Poll(0, 1, 30)) {

		wsLastError = Name;
		wsLastError += " - connect() timed out";
	} else {

		wsLastError.Free();	

		Error = 0;

	}
}



int inetSocket::Poll(int Read, int Write, int Timeout) {

	fd_set fdset_r; fd_set fdset_w; struct timeval tv; 

	struct timeval * ptv = NULL; if (Timeout > 0) {

		ptv = &tv;

		tv.tv_sec=Timeout; 

		tv.tv_usec=0;

	}

	fd_set * pfdset_r = NULL; if (Read) {

		FD_ZERO(&fdset_r); 

		FD_SET(S,&fdset_r);

		pfdset_r = &fdset_r;

	}

	fd_set * pfdset_w = NULL; if (Write) {

		FD_ZERO(&fdset_w); 

		FD_SET(S,&fdset_w);

		pfdset_w = &fdset_w;

	}

	if (select(S + 1, pfdset_r, pfdset_w, NULL, ptv) <= 0) {

		return 0;

	} else return 1;

}


inetSocket::~inetSocket(void){
	Close();

#ifndef ANSI
	/*
    http://www.stardust.com/wsresource/winsock1/ws_c.html#Cleanup
    
	 It's okay to cleanup multiple times, only the last WSACleanup really does the job and
	 each WSAStartup increments the count
    */
	WSACleanup();
#endif
}


/* set socket in non-blocking mode */

void inetSocket::SetBlockMode(int Blocking) {

#ifdef ANSI

	int SocketState = fcntl(S, F_GETFL);

	if (Blocking) SocketState &= (~O_NONBLOCK);

	else SocketState |= O_NONBLOCK;

	fcntl(S, F_SETFL, SocketState);

#else

	unsigned long BlockingParameter = !Blocking;

	ioctlsocket(S, FIONBIO, &BlockingParameter);

#endif

}



void inetSocket::Close(void) {

	if (S >= 0) {

		shutdown(S, 1);

#ifdef ANSI

		close(S);

#else

		closesocket(S);

#endif

		S = -1;

	}

}