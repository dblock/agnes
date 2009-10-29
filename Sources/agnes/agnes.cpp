#include <agnespch.hpp>

#ifdef ANSI
void KillAgnes(int Signal){
	exit(Signal);
}
#endif

int main(int argc, char * argv[]) {
#ifdef ANSI
	alarm(120);
	signal(SIGALRM, &KillAgnes);
#endif
	cgiOutStream CGIStream;
	
	cgi_init(CGIStream, argc, argv);
	pParam_v2 params(argc, argv);
	cgi_equiv_manager equivs("agnes");
	
	if ((argc < 3) || ((argc>=2)&&(!strcmp(argv[1],M_HOST_EQUIV)))) {
	  MHost SHost(CGIStream);
	  SHost.Operate(CGIStream, params, equivs);
	  CString Victim;
	  if (params.get_entry("ACTION") != "GADELETE") {
	    if (SHost.Accept(CGIStream, params, equivs, Victim)) {
	      CString Parent(Victim); Parent += "Parent";	
	      CUserManager UserManager(equivs.get_value(CGIStream, Parent));	
	      UserManager.Show(CGIStream, params, equivs, Victim);	  
	    } 
	  }
	} else {      
	  if (argc<3) cgi_error(CGIStream, "invalid number of parameters");      
	  
	  CCookieManager CookieManager(getenv("HTTP_COOKIE"));
	  
	  /*
	    user management/login
	  */
	  
	  CString Victim(argv[1]);
	  CString Parent(Victim); Parent+="Parent";
	  CUserManager UserManager(equivs.get_value(CGIStream, Parent));
	  
	  /*
	    cookie/post
	  */      
	  if (params.isPost() == 1) {       
	    CString UserName(Victim); UserName += "-NAME";
	    CString UserPass(Victim); UserPass += "-PASS";
	    CString UserNameLogin = params.get_entry(UserName);
	    CString UserPassLogin = params.get_entry(UserPass); UserPassLogin.Encrypt();
	    if (UserNameLogin.StrLength()) CookieManager.Add(UserName, UserNameLogin);
	    if (UserPassLogin.StrLength()) CookieManager.Add(UserPass, UserPassLogin);
	    CString Name = params.get_entry("NAME");
	    CString Email = params.get_entry("MAIL");
	    
	    if (Name.StrLength()) CookieManager.Add("NAME", Name);
			if (Email.StrLength()) CookieManager.Add("MAIL", Email);
	  }
	  
	  UserManager.Login(CGIStream, Victim, CookieManager, params);
	  
	  if (params.isPost() == 1) {
	    CString cgiNewBuffer;
	    cgiNewBuffer = "Content-type: text/html"; cgiNewBuffer+=(char) 10;
	    cgiNewBuffer += CookieManager.UpdateString();
	    cgiNewBuffer +=(char) 10; cgiNewBuffer +=(char) 10;
	    CGIStream.Buffer = cgiNewBuffer;
	  }
	  
	  // look at other options
	  // Registered -> NoRotate
	  if (UserManager.GetOption("Registered").StrLength()) {
            equivs.set_value("NOROTATE", "1");
	  }
	  
	  CString Argv1 = argv[1];
	  CString Argv2 = argv[2];
	  
	  // security: disallow data file
	  if (equivs.get_admin_manager().admin(equivs.get_admin_class(), Argv2)) {
	    cgi_error(CGIStream, "invalid template parameter");
	  }
	  
	  article_manager article_contents(CGIStream, equivs.get_value_admin(CGIStream, Argv1), &params, &equivs);     
	  article_contents.set_user_manager(&UserManager);
	  html_base<article_manager> agnes_forum(CGIStream, equivs.get_value(CGIStream, Argv2), &article_contents, params.argv);
	}
	common_exit(CGIStream);
	
	return 0;
}
