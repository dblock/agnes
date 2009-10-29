#include <agnespch.hpp>

CEmail::CEmail(void){
	
}

CEmail::~CEmail(void){
}

void CEmail::Send(inetSocket& mail_sock){
	CString it;
	it="mail from: "; it+=from; it+="\r\n"; send25_ok(mail_sock, it, 250);
	it="rcpt to: "; it+=to; it+="\r\n"; send25_ok(mail_sock, it, 250);
	send25_ok(mail_sock, "data\r\n", 354);
	it = "From: "; it+=name; it+=" <"; it+=from; it+=">\r\n"; send25_ok(mail_sock, it, -1);
	it = "To: "; it+=to; it+="\r\n"; send25_ok(mail_sock, it, -1);
	it = "Subject: "; it+=subject; it+="\r\n"; send25_ok(mail_sock, it, -1);
	send25_ok(mail_sock, "\r\n", -1);
	send25_ok(mail_sock, contents, -1);
	send25_ok(mail_sock, "\r\n.\r\n", 250);
}

void CEmail::Send(const CString& Server){
        conversation.Free();
#ifndef ANSI
	WSADATA W;
	WSAStartup (0x101, &W);
#endif
	inetSocket mail_sock(25, Server);

	expect25_ok(mail_sock , 220);
	send25_ok(mail_sock, "helo www.vestris.com\r\n", 250);
	Send(mail_sock);
	send25_ok(mail_sock, "quit\n\r", 221);
#ifndef ANSI
	WSACleanup();
#endif
	if (mail_sock.Error) {
	  conversation += "error: ";
	  conversation += wsLastError;
	}
	
	return;
}


#define RBUFSIZE 255



void CEmail::expect110_ok(inetSocket& Socket){

	char T[12]; char R[RBUFSIZE];

	int i; if ((i = recv(Socket.S,R,RBUFSIZE,0))< 0) Socket.wserror("error waiting for 110 ack"); R[i]='\0';
	
	conversation += R;

	strncpy(T, R, 3); T[3]='\0';

	if (strncmp(T, "+OK", 3)!= 0) Socket.wserror(R+5);

}



void CEmail::expect25_ok(inetSocket& Socket, int code) {

	char T[12]; char R[RBUFSIZE];

	int i; if ((i = recv(Socket.S,R,RBUFSIZE,0))< 0) Socket.wserror("error receiving port reply"); R[i]='\0';

	conversation += R;

	strncpy(T, R, 3); T[3]='\0';

	if ((code >= 0)&&(atoi(T) != code)) Socket.wserror(R+3);

}



void CEmail::send25_ok(inetSocket& Socket, const CString& string, int code){

        conversation += string;

	if (send(Socket.S, string.asString(), string.StrLength(), 0)<0) Socket.wserror("error sending to port 25");

	if (code >= 0) expect25_ok(Socket, code);

}



void CEmail::send110_ok(inetSocket& Socket, const CString& string){

  	conversation += string;

	if (send(Socket.S,string.asString(), string.StrLength(), 0)<0) Socket.wserror("error sending to port 110");

	expect110_ok(Socket);

}

