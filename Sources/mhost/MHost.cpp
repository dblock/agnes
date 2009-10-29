/*
management host
*/

#include <agnespch.hpp>
#include <agnes/bldver.hpp>

#define M_SIZE "40"
#define M_TWIDTH "100%"
#define TD1WIDTH "25%"
#define EUndefinedRoot "unable to determine"

MHost::MHost(cgiOutStream& CGIStream){	
	CGIStream << "<html>\n<head>\n<title>" << HostId() << " " << RC_VERSION_STRING << 
		" scripting host</title>\n</head>\n<body bgcolor=\"white\">\n";
	MakePaths(CGIStream);
}

MHost::~MHost(void) {
	
}

CString MHost::HostId(void){
	return "aGNeS News Forum";	
}

void MHost::StrOp(cgiOutStream& CGIStream, const CString& Str) {
	CGIStream << Str << " ... \n"; CGIStream.flush(); 
}

void MHost::StrOp(cgiOutStream& CGIStream, const CString& StrC, const CString& StrV) {
	CString Tmp(StrC);  
	Tmp+=" [<b>"; Tmp+=StrV; Tmp+="</b>]";
	StrOp(CGIStream, Tmp);
}

void MHost::StrOk(cgiOutStream& CGIStream, const CString& Str) {
	CGIStream << "<font color=\"blue\">" << Str << "!</font>" << elf;
}

void MHost::StrOk(cgiOutStream& CGIStream) {
	StrOk(CGIStream, "ok");
}

void MHost::StrError(cgiOutStream& CGIStream, const CString& Str) {
	CGIStream << "<font color=\"red\">" << Str << "!</font>" << elf;
}

void MHost::StrError(cgiOutStream& CGIStream) {
	StrError(CGIStream, "invalid");
}

int MHost::CheckAdminPassword(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs){
	StrOp(CGIStream, "checking administrative password");
	CString Password = Access.get_value(CGIStream, "manage-add",0);
	if (Password.StrLength() && (Params.get_entry("PASSWORD")!=Password)) {
		StrError(CGIStream);
		return 0;
	} else StrOk(CGIStream);
	return 1;
}

void MHost::ProcessPost(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs){
	CString Preserve = nextgen_v2::get_curdir();
	CString Action = Params.get_entry("ACTION");
	if (Action == "AGNES-ADD") ProcessPostAgnes(CGIStream, Params, Equivs);
	else  if (Action == "GAGENERAL") GAPostGeneral(CGIStream, Params, Equivs);
	else if (Action == "GAENTRY") GAPostEntry(CGIStream, Params, Equivs);
	else if (Action == "GADELETE") GAPostDelete(CGIStream, Params, Equivs);
	else if (Action == "GACACHE") GAPostCache(CGIStream, Params, Equivs);
	else if (Action == "GAEMAIL") GAPostEmail(CGIStream, Params, Equivs);
	else if (Action == "GATEMPLATE") GAPostTemplate(CGIStream, Params, Equivs);    
	else if (Action == "PASSWD-CHANGE") ProcessPostPasswd(CGIStream, Params, Equivs);
	else if (Action == "PASSWD-REMINDER") ProcessPostPasswdReminder(CGIStream, Params, Equivs);	
	else if (Action == "GAPURGE") ProcessPostPurge(CGIStream, Params, Equivs);
	else if (Action == "TESTMAILEXCHANGER") ProcessPostTestMailExchanger(CGIStream, Params, Equivs);
	chdir((const char *) Preserve.asString());
	CGIStream << hr_;
}

void MHost::ProcessPostTestMailExchanger(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs) {
    StrOp(CGIStream, "testing mail exchanger");

    CString RootEmail(Equivs.get_value(CGIStream, "ROOT-EMAIL", 0)); 
    if (! RootEmail.StrLength()) 
      RootEmail = "root";

    CString MailServer(Equivs.get_value(CGIStream, "MailServer", 0)); 
    if (! MailServer.StrLength()) 
      MailServer = "mail";

    CString DestOp;
    
    DestOp = "<br>email: ";
    DestOp += RootEmail;
    StrOp(CGIStream, DestOp);

    DestOp = "<br>server: ";
    DestOp += MailServer;
    StrOp(CGIStream, DestOp);
    
    CEmail SEmail;

    SEmail.from = RootEmail;
    SEmail.to = RootEmail;
    SEmail.subject = "Testing mail exchanger.";
    SEmail.name = "aGNeS Admin";
    SEmail.contents = "This is a mail exchanger test.\n";
    SEmail.contents += "Attempted to send mail from ";
    SEmail.contents += getenv("SERVER_NAME");
    SEmail.contents += " via ";
    SEmail.contents += MailServer;
    SEmail.contents += "\n";
    
    SEmail.Send(MailServer);	
    
    CGIStream << "<blockquote><pre>";
    CGIStream << SEmail.conversation;
    CGIStream << "</pre></blockquote>";
    
    StrOp(CGIStream, "test completed, look for errors above");
}

void MHost::ProcessPostPurge(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs) {
    StrOp(CGIStream, "checking for selected aliases");
    
	CString Password(Params.get_entry("PASSWORD"));
	CString RootPassword(Access.get_value(CGIStream, "root", 0));
    
	StrOp(CGIStream, "checking for root password");
	if (RootPassword.StrLength()) {
		StrOk(CGIStream);
		StrOp(CGIStream, "checking root password (entire group administrator)");    
		if (RootPassword != Password) {
			StrError(CGIStream);
			return;
		} else StrOk(CGIStream);
	} else StrError(CGIStream, "none");
    
	for (int i=0; i< Params.entries_count();i++) {
		if (Params.get_name(i) == "VICTIM") {
			CString ce = Equivs.get_value(CGIStream, Params.get_entry(i), 0);
			if (ce.StrLength()) {	
				CGIStream << "Deleting " << b_ << Params.get_entry(i) << !b_ << elf;
                GAPostDelete(CGIStream, Params.get_entry(i), Params, Equivs);
			}
			CGIStream.flush();
		}
    }
}
    
void MHost::ProcessPostPasswdReminder(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs){
	StrOp(CGIStream, "checking for a selected script alias");
	CString Victim(Params.get_entry("VICTIM"));
	if (!(Victim.StrLength())) {
		StrError(CGIStream, "none chosen - please select your script alias");
		return;
	} else StrOk(CGIStream);
	StrOp(CGIStream, "generating email");
	
	CEmail SEmail;
	CString RootEmail(Equivs.get_value(CGIStream, "ROOT-EMAIL", 0)); if (!RootEmail.StrLength()) RootEmail = "root";
	SEmail.from = RootEmail;		
	SEmail.subject = "Password reminder.";
	SEmail.name = "Script Admin";
	SEmail.contents = "You have requested a password reminder.\n\nServer URL: ";
	SEmail.contents+= getenv("SERVER_NAME");  
	SEmail.contents+= getenv("SCRIPT_NAME");
	SEmail.contents+="\nAlias: ";
	SEmail.contents+=Victim;
	SEmail.contents+="\nPassword: ";
	SEmail.contents+=Access.get_value(CGIStream, Victim, 0);
	SEmail.contents+="\n\nVestris Inc. Script Administrator";
	
	Equivs.make_asocs();    
	Equivs.make_obvious();
	CString CurEmail(Equivs.EEmail.get_value(Victim)); CurEmail.StrTrim();  
	CString CurName;
	int iPos = CurEmail.Pos('<');
	if (iPos > 0) CurName = CurEmail.Copy(0, iPos).StrTrim();
	CurEmail.StrDelete(0, iPos+1);
	iPos = CurEmail.Pos('>');
	if (iPos > 0) CurEmail.StrDelete(iPos, CurEmail.StrLength());   
	StrOk(CGIStream);  
	SEmail.to = CurEmail;
	CString MServer(Equivs.get_value(CGIStream, "MailServer", 0)); if (!MServer.StrLength()) MServer = "mail";  
	CString DestOp("sending password to "); DestOp+=CurEmail;
	StrOp(CGIStream, DestOp);
	SEmail.Send(MServer);
	StrOk(CGIStream); 
}

void MHost::ProcessPostPasswd(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs){
	StrOp(CGIStream, "checking for a selected script alias");
	CString Victim(Params.get_entry("VICTIM"));
	if (!(Victim.StrLength())) {
		StrError(CGIStream, "none chosen - please select your script alias");
		return;
	} else StrOk(CGIStream);   
	CString OldPass(Params.get_entry("PASS-OLD"));  
	StrOp(CGIStream, "checking administrative password");
	
	if ((Access.get_value(CGIStream, Victim, 0) != OldPass)||(!OldPass.StrLength())) {
		StrError(CGIStream);
		StrOp(CGIStream, "checking root password");    
		if ((Access.get_value(CGIStream, "root", 0) != OldPass)||(!OldPass.StrLength())) {
			StrError(CGIStream);
			return;
		} else StrOk(CGIStream);
	} else StrOk(CGIStream);   
	CString NewPass(Params.get_entry("PASS-NEW"));
	StrOp(CGIStream, "checking new password", "*****");
	if ((NewPass.StrLength() < 4)||(NewPass.isSubStr(' ') != -1)) {
		StrError(CGIStream, "password too short (must be at least 4 chars) or contains spaces");
		return;
	} else StrOk(CGIStream);
	
	StrOp(CGIStream, "checking if confirmation password matches");  
	if (NewPass != Params.get_entry("PASS-NEW2")) {
		StrError(CGIStream);
		return;
	} else StrOk(CGIStream);  
	Access.set_value(Victim, NewPass);
	StrOp(CGIStream, "flushing data to access.struct");
	if (Access.cgi_access_write()) StrOk(CGIStream); else StrError(CGIStream);
}

void MHost::ProcessPostAgnes(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs){  
	if (!CheckAdminPassword(CGIStream, Params, Equivs)) return;    
	
	CString NA_Name = Params.get_entry("NAME").StrTrim32();
	CString NA_Email = Params.get_entry("EMAIL").StrTrim32();
	CString NA_Alias = Params.get_entry("ALIAS").StrTrim32();
	CString NA_Url = Params.get_entry("URL").StrTrim32();
	CString NA_Pass = Params.get_entry("PASS").StrTrim32();
	CString NA_Pass2 = Params.get_entry("PASS2").StrTrim32();
	CString NA_HeaderHTML = Params.get_entry("header.html").StrTrim32();
	CString NA_ArticleHTML = Params.get_entry("article.html").StrTrim32();
	CString NA_FollowHTML = Params.get_entry("follow.html").StrTrim32();
	CString NA_PostNewHTML = Params.get_entry("postnew.html").StrTrim32();
	CString NA_SearchHTML = Params.get_entry("search.html").StrTrim32();
	CString NA_PreviewHTML = Params.get_entry("preview.html").StrTrim32();
	
	int Error = 0;
	int i;
	
	StrOp(CGIStream, "checking name", NA_Name); 
	if (!NA_Name.StrLength()) {StrError(CGIStream, "no name specified");Error = 1;} else StrOk(CGIStream);
	
	StrOp(CGIStream, "checking e-mail", NA_Email);
	CMail Mail(NA_Email);
	if ((!NA_Email.StrLength())||(!Mail.isGoodInternet())) {
		StrError(CGIStream, "no email or invalid email specified");
		Error = 1;
	} else StrOk(CGIStream);
	
	StrOp(CGIStream, "checking alias for valid length", NA_Alias);    
	if ((NA_Alias.StrLength() > 10)||(NA_Alias.StrLength() < 3)) {
		StrError(CGIStream, "alias is empty, is too long or is too short, must be [3-10] characters");
		Error = 1;
	} else StrOk(CGIStream);        
	 
	StrOp(CGIStream, "checking if alias is valid", NA_Alias);
        bool AliasValid = true;
        for (i=0;i<NA_Alias.StrLength();i++) {
          if (!isalnum(NA_Alias[i])) {
            StrError(CGIStream, "invalid alias specified, must be single word and with no special characters");
            Error = 1;
            AliasValid = false;
            break;
          } 
        } 
        
        CString t = NA_Alias; 
        t+="Agnes";
        
        if (AliasValid) {
          StrOk(CGIStream);	
          StrOp(CGIStream, "checking if alias already exists", NA_Alias);
          if (Equivs.get_value(CGIStream, t, 0).StrLength()) {
            StrError(CGIStream, "sorry, this alias is already used, pick a different one");
            Error = 1;
          } else StrOk(CGIStream, "it does not, good!");
        }
	
	StrOp(CGIStream, "checking if file names are empty");
	if ((!NA_HeaderHTML.StrLength())||(!NA_FollowHTML.StrLength())||(!NA_PostNewHTML.StrLength())||(!NA_SearchHTML.StrLength())||(!NA_PreviewHTML.StrLength())) {
		StrError(CGIStream, "one of the header.html/article.html/... files is empty");
		Error = 1;
	} else StrOk(CGIStream);
	
	StrOp(CGIStream, "checking if URL is correct", NA_Url);
	if (!(NA_Url.StartsWith("http://"))) {
		StrError(CGIStream, "invalid url, must be http://...");
		Error = 1;
	} else StrOk(CGIStream);
	
	StrOp(CGIStream, "checking password", "*****");
	if ((NA_Pass.StrLength() < 4)||(NA_Pass.isSubStr(' ') != -1)||(NA_Pass!=NA_Pass2)) {
		StrError(CGIStream, "password too short (must be at least 4 chars), contains spaces or confirmed password does not match");
		Error = 1;
	} else StrOk(CGIStream);
	
	if (Error) 
          return;    
	
#ifdef proxy
	CGIStream << "removing proxy setting ...";
	Proxy.Free();
#endif
	
	NA_Url.append_if_missing('/');
	t = NA_Url; t += NA_HeaderHTML;
	StrOp(CGIStream, "checking header.html existence, this may take a while", t);
	nextgen_v2 gen(t); 
	if (gen.void_read) {
		CString tmp = "invalid url or invalid file name<br>error returned:"; tmp+=gen.contained;
		StrError(CGIStream, tmp);      
		return;
	} else {
		CString tmp = "read "; tmp+= gen.contained.StrLength(); tmp+=" bytes, good";
		StrOk(CGIStream, tmp);
	}
    
	StrOp(CGIStream, "checking header.html content", t);

	if (gen.contained.isSubStr("<!--AGNES-HEADER-->") == -1) {
		StrError(CGIStream, "invalid header.html file, must contain at least a valid aGNeS tag -  - this means that your header.html url is false or page is not accessible");
		CGIStream << "returned document<pre>" << gen.contained << "</pre>\n";
		return;
	} else StrOk(CGIStream, "ok, found a valid aGNeS tag, assuming other files are okay");
	
	CGIStream << "<b>HAL will be happy to add your new aGNeS forum, please be patient ...</b>" << elf;    
	CGIStream << "adding contact definition:<br><b># " << NA_Name << " &lt;" << NA_Email << "&gt;</b>" << elf;
	CGIStream << "adding equiv structure for contents:" << elf;
	CVector<CString> FirstSetNames; 
	CVector<CString> FirstSetValues;
	
	NA_Alias.LowerCase();
	NA_Alias[0] = CString :: ToUpper(NA_Alias[0]);
	
	t = NA_Alias; t+="Agnes";
	FirstSetNames+=t;
	FirstSetNames+="Parent";
	FirstSetNames+="";
	t = nextgen_v2::get_rootdir(); t+="data";t.append_backslash(); t+="agnes"; t.append_backslash();
	CString root(t); t+=NA_Alias.LowerCase();
	
	CGIStream << "current directory is: " << root.asString() << elf;
	
	if (!chdir(t.asString())) {
		StrError(CGIStream, "sorry, alias used from a different structure, please pick a different one");
		return;
	} 
	StrOp(CGIStream, "creating structure directory", t);
	nextgen_v2::check_dir(t);
	if (chdir(t.asString())) {
		StrError(CGIStream, "error creating directory");
		return;
	}
	t.append_backslash();
#ifdef ANSI
	CString t2 = "cp -R "; t2+=root; t2+="template/* "; t2+=t;
	StrOp(CGIStream, "executing", t2);    
	system(t2.asString());
#endif
    
	FirstSetValues+=t;
	FirstSetValues+="";
	FirstSetValues+="header.list";
	
	CGIStream << b_;
	for (i=0;i<FirstSetNames.Count();i++) { if (i) CGIStream << "+"; CGIStream << FirstSetNames[i];}
	CGIStream << ",";
	for (i=0;i<FirstSetValues.Count();i++) { if (i) CGIStream << "+"; CGIStream << FirstSetValues[i];}
	CGIStream << !b_ << elf;
	
	StrOp(CGIStream, "creating equiv structure for templates");
	CVector<CString> SecondSetNames;
	CVector<CString> SecondSetValues;
	NA_Alias[0] = CString :: ToUpper(NA_Alias[0]);
	t = NA_Alias; t+="Agnes"; t+="HTML";
	SecondSetNames+=t;
	SecondSetNames+="";
	SecondSetNames+="Search";
	SecondSetNames+="Followup";
	SecondSetNames+="PostNew";
	SecondSetNames+="Article";
	SecondSetNames+="Preview";
	SecondSetValues+=NA_Url;
	SecondSetValues+=NA_HeaderHTML;
	SecondSetValues+=NA_SearchHTML;
	SecondSetValues+=NA_FollowHTML;
	SecondSetValues+=NA_PostNewHTML;
	SecondSetValues+=NA_ArticleHTML;
	SecondSetValues+=NA_PreviewHTML;
	
	CGIStream << b_;
	for (i=0;i<SecondSetNames.Count();i++) { if (i) CGIStream << "+"; CGIStream << SecondSetNames[i];}
	CGIStream << ",";
	for (i=0;i<SecondSetValues.Count();i++) { if (i) CGIStream << "+"; CGIStream << SecondSetValues[i];}
	CGIStream << !b_ << elf;
	CGIStream << hr_ << elf;
	CString tCmd = getenv("SCRIPT_NAME"); tCmd+="?manage";
	CGIStream << b_ << a_.o("HREF", tCmd.asString(), 0) << "BOOKMARK THIS ADMINISTRATION URL" << !a_ << !b_ << hr_ << elf;  
	tCmd = getenv("SCRIPT_NAME"); 
	tCmd+="?"; tCmd+=NA_Alias; tCmd+="Agnes+"; tCmd+=NA_Alias; tCmd+="AgnesHTML";
	CGIStream << font_.o("SIZE","+1",0) << b_ << a_.o("HREF", tCmd.asString(), 0) << "CLICK HERE FOR YOUR NEW FORUM!" << !b_ << !font_
		<< !a_ << br_ << "the url is: " << b_ << "http://" << getenv("SERVER_NAME") << tCmd << !b_ << hr_ << elf;
	
	CString StatFile = Equivs.get_value(CGIStream, "Agnes-Stat", 0);
    CString StatContents;
    
	if (StatFile.StrLength()) {        
        nextgen_v2 stat(StatFile);
        StatContents = stat.contained;
    }
    
    CGIStream << font_.o("SIZE","-1", 0) << StatContents << !font_ << elf;
    CGIStream << hr_ << elf;
    CGIStream.flush();
    cout.flush();    
	
	NA_Alias+="Agnes";
	Access.write_full_access(NA_Alias, NA_Pass);    
	Admin.write_full_admin("agnes",NA_Alias);
	Equivs.write_full_equiv(NA_Name, NA_Email, FirstSetNames, FirstSetValues, SecondSetNames, SecondSetValues); 
	cout.flush();
	CGIStream << hr_ << elf;

	CEmail SEmail;
	CString RootEmail(Equivs.get_value(CGIStream, "ROOT-EMAIL", 0)); if (!RootEmail.StrLength()) RootEmail = "root";
	SEmail.from = RootEmail;
	SEmail.subject = "Welcome to the interactive world!";
	SEmail.name = "aGNeS Admin";
	SEmail.contents = "http://";
	SEmail.contents+=getenv("SERVER_NAME");
	CString Port(getenv("SERVER_PORT")); if (Port.Val() && Port.Val() != 80) {
		SEmail.contents += ":";
		SEmail.contents += Port.Val();
	}
	SEmail.contents+= tCmd;
	SEmail.contents+="\n\n";
	SEmail.contents+=StatContents.remove_br();    
	SEmail.to = NA_Email;
	CString MServer(Equivs.get_value(CGIStream, "MailServer", 0)); if (!MServer.StrLength()) MServer = "mail";
	SEmail.Send(MServer);	
}

void MHost::PasswordInput(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs){
	if (Access.get_value(CGIStream, "manage-add",0).StrLength()) 
		CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "Admin Password: " << !td_ << td_ << input_.o("NAME","PASSWORD","TYPE","PASSWORD","SIZE",M_SIZE,0) << !td_ << !tr_ << "\n";
}

void MHost::AliasSelect(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs){
	CGIStream << select_.o("NAME","VICTIM",0) << "\n";
	CVector<CString> SVector;
	for (int i=0;i<Equivs.EAlias.entries_count();i++) SVector+=Equivs.EAlias.get_name(i);
	SVector.QSort();
	for (int j=0;j<SVector.Count();j++)
		CGIStream << option_.o("VALUE",SVector[j].asString(), "", "SELECTED", 0) << SVector[j] << !option_ << "\n";
	//CGIStream << input_.o("TYPE","RADIO","NAME","VICTIM","VALUE",Equivs.EAlias.get_name(i).asString(), 0) << " " << Equivs.EAlias.get_name(i) << !input_ << elf;
	CGIStream << !select_;
}

void MHost::PassChangeForm(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs){
	Equivs.make_oce();
	
	CGIStream << form_.o("ACTION",Cmd.asString(),"METHOD","POST",0) << "\n";
	CGIStream << table_.o("WIDTH",M_TWIDTH, 0); 
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "Choose your data alias:" << !td_ << td_ << "\n";
	AliasSelect(CGIStream, Params, Equivs);
	CGIStream << !td_ << !tr_ << "\n";    
	CGIStream << input_.o("NAME","ACTION","TYPE","HIDDEN","VALUE","PASSWD-CHANGE",0) << "\n";
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "Current Password: " << !td_ << td_ << input_.o("NAME","PASS-OLD","TYPE","PASSWORD","SIZE",M_SIZE,0) << !td_ << !tr_ << "\n";
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "New Password:" << !td_ << td_ << input_.o("NAME","PASS-NEW","TYPE","PASSWORD","SIZE",M_SIZE, 0) << !td_ << !tr_ << "\n";
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "Confirm New Password:" << !td_ << td_ << input_.o("NAME","PASS-NEW2","TYPE","PASSWORD","SIZE",M_SIZE, 0) << !td_ << !tr_ << "\n";
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << !td_ << td_ << input_.o("NAME","SUBMIT","TYPE","SUBMIT","VALUE","Modify!", 0) << !td_ << !tr_ << "\n";
	CGIStream << !table_ << !form_ << "\n";
}

void MHost::AgnesAddForm(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs){
	CGIStream << form_.o("ACTION",Cmd.asString(),"METHOD","POST",0) << "\n";
	CGIStream << table_.o("WIDTH",M_TWIDTH, 0); 
	PasswordInput(CGIStream, Params, Equivs);
	CGIStream << input_.o("NAME","ACTION","TYPE","HIDDEN","VALUE","AGNES-ADD",0) << "\n";
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "Contact Name: " << !td_ << td_ << input_.o("NAME","NAME","TYPE","TEXT","SIZE",M_SIZE,0) << !td_ << !tr_ << "\n";
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "Contact E-Mail: " << !td_ << td_ << input_.o("NAME","EMAIL","TYPE","TEXT","SIZE",M_SIZE,0) << !td_ << !tr_ << "\n";
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "aGNeS Alias:<br><font size=\"-1\">(any simple single short word<br>that describes your site best)</font>" << !td_ << td_ << input_.o("NAME","ALIAS","TYPE","TEXT","SIZE",M_SIZE,0) << !td_ << !tr_ << "\n";
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "URL of the demo pages:<br><font size=\"-1\">(without any file names)</font>" << !td_ << td_ << input_.o("NAME","URL","TYPE","TEXT","VALUE","http://yoursite.url/agnes/","SIZE",M_SIZE, 0) << !td_ << !tr_ << "\n";
	
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "File Names:<br><font size=\"-1\">(without the urls,<br>adjust in case you have<br>an .htm extension<br>or different names)</font>" << !td_ << td_	 
		<< input_.o("NAME","header.html","TYPE","TEXT","VALUE","header.html","SIZE",M_SIZE, 0) << br_ << "\n"
		<< input_.o("NAME","article.html","TYPE","TEXT","VALUE","article.html","SIZE",M_SIZE, 0) << br_ << "\n"
		<< input_.o("NAME","postnew.html","TYPE","TEXT","VALUE","postnew.html","SIZE",M_SIZE, 0) << br_ << "\n"
		<< input_.o("NAME","follow.html","TYPE","TEXT","VALUE","follow.html","SIZE",M_SIZE, 0) << br_ << "\n"
		<< input_.o("NAME","preview.html","TYPE","TEXT","VALUE","preview.html","SIZE",M_SIZE, 0) << br_ << "\n"
		<< input_.o("NAME","search.html","TYPE","TEXT","VALUE","search.html","SIZE",M_SIZE, 0)	<< "\n"
		<< !td_ << !tr_ << "\n";
	
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "aGNeS Forum Password:<br><font size=\"-1\">(used for forum maintaince,<br>please remember it)</font>" << !td_ << td_ << input_.o("NAME","PASS","TYPE","PASSWORD","SIZE",M_SIZE, 0) << !td_ << !tr_ << "\n";
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "Confirm Password:" << !td_ << td_ << input_.o("NAME","PASS2","TYPE","PASSWORD","SIZE",M_SIZE, 0) << !td_ << !tr_ << "\n";
	
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << !td_ << td_ << input_.o("NAME","SUBMIT","TYPE","SUBMIT","VALUE","Confirm!", 0) << !td_ << !tr_ << "\n";
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << !td_ << td_ << "submission might take up to a minute,<br>please be patient" << !td_ << !tr_ << "\n";
	
	
	CGIStream << !table_ << !form_ << "\n";
}

void MHost::ProcessGet(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs){    
	if (Access.get_value(CGIStream, "root", 0).StrLength()) {	    
		CString tCmd;		
		
		if ((Params.argv.Count() >= 3)&&(Params.argv[2] == "add")) AgnesAddForm(CGIStream, Params, Equivs);    
		tCmd = Cmd; tCmd+="+add";
		CGIStream << "[ " << a_.o("HREF", tCmd.asString(), 0) << "create a new aGNeS forum" << !a_ << " ]" << elf;    
		
		tCmd = Cmd; tCmd+="+general";
		CGIStream << "[ " << a_.o("HREF", tCmd.asString(), 0) << "this group administration" << !a_ << " ]" << elf;
		tCmd = Cmd; tCmd+="+list";
		CGIStream << "[ " << a_.o("HREF", tCmd.asString(), 0) << "list all equivalence structures" << !a_ << " ]" << elf;
		tCmd = Cmd; tCmd+="+obvious";
		CGIStream << "[ " << a_.o("HREF", tCmd.asString(), 0) << "list all obvious associations" << !a_ << " ]" << elf;
		
		tCmd = Cmd; tCmd+="+admin";  
		CGIStream << "[ " << a_.o("HREF", tCmd.asString(), 0) << "aGNeS administration" << !a_ << " ]" << elf;    
		CGIStream << "[ " << a_.o("HREF", "http://www.vestris.com/agnes/split.html", 0) << "Vestris Inc. forum for questions" << !a_ << " ]" << elf;    
		if (Params.argv.Count() >= 3) {
			if (Params.argv[2] == "list") Equivs.output_equivs(CGIStream);
			else if (Params.argv[2] == "general") GAGeneral(CGIStream, Params, Equivs);
			else if (Params.argv[2] == "delete") GADeleteAll(CGIStream, Params, Equivs);
			else if (Params.argv[2] == "obvious") {
				CGIStream << hr_ << "constructing obvious associations:" << hr_;
				Equivs.make_obvious();
				for(int k=0;k<Equivs.EObvious.Count();k++) CGIStream << Equivs.EObvious[k] << elf;
				CGIStream << hr_ <<  Equivs.l_agnes << " aGNeS News Forums" << elf;  
			} else if (Params.argv[2] == "pass") PassChangeForm(CGIStream, Params, Equivs);
			else if (Params.argv[2] == "admin") GAEntry(CGIStream, Params, Equivs);
            else if (Params.argv[2] == "purge") {

                int nArticleAge = -1;
                if (Params.argv.Count() >= 3)
                    nArticleAge = Params.argv[3].Val();
                
                CGIStream << hr_ << "constructing obvious associations (" << nArticleAge << "):" << hr_;
                
                CGIStream << form_.o("ACTION",Cmd.asString(),"METHOD","POST",0) << "\n";
                CGIStream << table_.o("WIDTH",M_TWIDTH, 0); 
                CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "Choose the data aliases to purge:" << br_;                    
                CGIStream << !td_ << !tr_ << "\n";    
                CGIStream << input_.o("NAME","ACTION","TYPE","HIDDEN","VALUE","GAPURGE",0) << "\n";
                CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "Password: " << !td_ << td_ << input_.o("NAME","PASSWORD","TYPE","PASSWORD",0) << !td_ << !tr_ << "\n";
                CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << !td_ << td_;                
    
				Equivs.make_obvious(nArticleAge, 1);
				for(int k=0;k<Equivs.EObvious.Count();k++) 
                {
                    CGIStream << Equivs.EObvious[k] << elf;
                }
                CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << !td_ << td_ << input_.o("NAME","SUBMIT","TYPE","SUBMIT","VALUE","Purge", 0) << !td_ << !tr_ << "\n";
                CGIStream << !table_ << !form_ << "\n";
				CGIStream << hr_ <<  Equivs.l_agnes << " aGNeS News Forums" << elf;
            }
		} else {
			GAEntry(CGIStream, Params, Equivs);     
			Equivs.make_obvious();      
			CGIStream << hr_ <<  Equivs.l_agnes << " aGNeS News Forums" << elf;  
		}    
	} else GAGeneral(CGIStream, Params, Equivs);
}

void MHost::Operate(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs){  
	Cmd = getenv("SCRIPT_NAME");
	if (Params.argv.Count() >= 2) {
		Cmd+="?"; Cmd+=Params.argv[1];
	} else Cmd += "?manage";
	CGIStream << blockquote_ << "scripting host type: " << b_ << HostId() << !b_  << " - (" << __DATE__ << ")" << elf;  
#ifdef ANSI
	struct utsname UTS;
	if (uname(&UTS) >= 0) {
		CGIStream << "Running <b>" << UTS.sysname << " " << UTS.release << " (" << UTS.version << "/" << UTS.machine << ")</b>" << elf;
		CGIStream << "UTS node: <b>" << UTS.nodename << "</b>" << elf;
	}
#else
	CGIStream << "Runinng <b>Windows 95/NT</b>" << elf;
#endif
	
	CString MV4Group(getenv("SCRIPT_NAME"));  
	int sPos = MV4Group.InvPos('/');
	if (sPos >= 0) {
	  MV4Group.StrDelete(sPos, MV4Group.StrLength());
		sPos = MV4Group.InvPos('/');
		if (sPos >= 0) {
		        MV4Group.StrInsert(sPos+1,b_.o("","",0));
			MV4Group+=!b_;
		}
		if (MV4Group.StrLength()) CGIStream << "group: " << MV4Group << elf;
	}
	
	CString Banner = Equivs.get_value(CGIStream, "Banner", 0); 
	if (Banner.StrLength()) 
		CGIStream << br_ << a_.o("HREF", "http://www.vestris.com", 0) << img_.o("SRC",Banner.asString(),0) << !a_ << br_ << elf;
	
	if (Params.isPost() == 1) ProcessPost(CGIStream, Params, Equivs);
	ProcessGet(CGIStream, Params, Equivs);
	
	CString RootEmail(Equivs.get_value(CGIStream, "Root-Email", 0)); 
	if (!RootEmail.StrLength()) RootEmail = "root";
	CString RootEmailLink("mailto:"); RootEmailLink+=RootEmail; RootEmailLink+="?Subject=Question from MV4 Scripting Host";
	CGIStream << hr_ << "(this is an automatic script process, for all questions or problems, please e-mail " << a_.o("HREF",RootEmailLink.asString(),0) << RootEmail << !a_ << ")" << elf; 
	CGIStream << !blockquote_ << "</body>\n</html>";
}

void MHost::GAEntry(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs){
	if (Access.get_value(CGIStream, "root", 0).StrLength()) {
		Equivs.make_oce();  
		CGIStream << form_.o("ACTION",Cmd.asString(),"METHOD","POST",0) << "\n";
		CGIStream << table_.o("WIDTH",M_TWIDTH, 0); 
		CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "Choose your data alias:" 
			  << br_ << font_.o("SIZE","-2",0) << "(after login you can" << br_ << "change admin password, delete an alias" << br_ << "update cache, change URLs and" << br_ << "modify admin parameters)" << !font_ << !td_ << td_ << "\n";
		AliasSelect(CGIStream, Params, Equivs);
		CGIStream << !td_ << !tr_ << "\n";    
		CGIStream << input_.o("NAME","ACTION","TYPE","HIDDEN","VALUE","GAENTRY",0) << "\n";
		CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "Password: " << !td_ << td_ << input_.o("NAME","PASSWORD","TYPE","PASSWORD",0) << !td_ << !tr_ << "\n";
		CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << !td_ << td_ << input_.o("NAME","SUBMIT","TYPE","SUBMIT","VALUE","MV4 Administration", 0) << !td_ << !tr_ << "\n";
		CGIStream << !table_ << !form_ << "\n";
	} else GAGeneral(CGIStream, Params, Equivs);
}

void MHost::GAPostDelete(cgiOutStream& CGIStream, const CString& Victim, pParam_v2& Params, cgi_equiv_manager& Equivs) {
    StrOp(CGIStream, "deleting admin alias");
    Admin.entry_manager::del_value(Victim);
    //Admin.entry_manager::set_value(Victim, "deleted", Victim);
    StrOk(CGIStream);
    StrOp(CGIStream, "flushing data to admin.struct");
    if (Admin.cgi_admin_write()) StrOk(CGIStream); else StrError(CGIStream);
    
    StrOp(CGIStream, "deleting access alias");
    Access.entry_manager::Singles.Del(Victim);
    StrOk(CGIStream);
    StrOp(CGIStream, "flushing data to access.struct");
    if (Access.cgi_access_write()) StrOk(CGIStream); else StrError(CGIStream);
    
    if (Victim.EndsWith("AGNES")) {
        CString Parent(Victim); Parent+="Parent";
        CString Directory(Equivs.entry_manager::get_value(Parent));
        CGIStream << "deleting directory " << Directory << elf;
#ifdef ANSI
        if (Directory.StrLength()) {
            CString Cmd = "rm -r "; Cmd += Directory;	
            system(Cmd.asString());
        }
#endif
    } else {
        unlink(Equivs.entry_manager::get_value(Victim).asString());
    }   
    StrOp(CGIStream, "deleting equiv alias, flushing equiv.struct");
    if (Equivs.delete_template(CGIStream, Victim)) StrOk(CGIStream); else StrError(CGIStream);       
}
    
void MHost::GAPostDelete(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs){
	CString Victim;
	if (GAPostCommon(CGIStream, Params, Equivs, Victim)) {        
        GAPostDelete(CGIStream, Victim, Params, Equivs);       		
	}
}

void MHost::GAPostGeneral(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs){
	CString NewPassword(Params.get_entry("NEWPASSWORD"));
	CString OldPassword(Params.get_entry("PASSWORD"));
	CString RootPassword(Access.get_value(CGIStream, "root", 0));
	StrOp(CGIStream, "checking for root password existance");
	if (RootPassword.StrLength()) {	
		StrOk(CGIStream);
		StrOp(CGIStream, "checking root password (entire group administrator)");    
		if (RootPassword != OldPassword) {
			StrError(CGIStream);
			return;
		} else StrOk(CGIStream);
	} else StrError(CGIStream, "none");
	
	StrOp(CGIStream, "checking for a new password");
	if (NewPassword.StrLength()){
		StrOk(CGIStream);
		StrOp(CGIStream, "checking new password", "*****");
		if ((NewPassword.StrLength() < 4)||(NewPassword.isSubStr(' ') != -1)) {
			StrError(CGIStream, "password too short (must be at least 4 chars) or contains spaces");
			return;
		} else StrOk(CGIStream);
		
		StrOp(CGIStream, "checking if confirmation password matches");  
		if (NewPassword != Params.get_entry("NEWPASSWORD2")) {
			StrError(CGIStream);
			return;
		} else StrOk(CGIStream);
		
		Access.set_value("root", NewPassword);
		StrOp(CGIStream, "flushing data to access.struct");
		if (Access.cgi_access_write()) StrOk(CGIStream); else StrError(CGIStream);	
	} else StrError(CGIStream, "ignoring");
	
	CString NewManageAdd(Params.get_entry("MANAGEADD"));
	CString OldManageAdd(Access.get_value(CGIStream, "manage-add", 0));
	StrOp(CGIStream, "checking for new management password (forum creation)");
	if (NewManageAdd.StrLength()) {
		if (NewManageAdd != OldManageAdd){
			StrOk(CGIStream);
			StrOp(CGIStream, "flushing data to access.struct");
			Access.set_value("manage-add", NewManageAdd);
			StrOp(CGIStream, "flushing data to access.struct");
			if (Access.cgi_access_write()) StrOk(CGIStream); else StrError(CGIStream);
		} else StrError(CGIStream, "ignoring");
	} else StrOk(CGIStream, "ignoring");
	
	CString NewEmail(Params.get_entry("EMAIL"));	
	CString RootEmail(Equivs.get_value(CGIStream, "Root-Email", 0)); 
	CString NewEmailTmp("checking for new admin email "); NewEmailTmp+=NewEmail; 
	if (RootEmail.StrLength()) {
		NewEmailTmp += "/"; NewEmailTmp+=RootEmail;
	}
	StrOp(CGIStream, NewEmailTmp);
	if (NewEmail.StrLength()) {
		if (NewEmail != RootEmail){
			if ((!NewEmail.StrLength())||(NewEmail.isSubStr('@') == -1)||(NewEmail.isSubStr('.') == -1)||(NewEmail.isSubStr(' ')!=-1)) {
				StrError(CGIStream, "invalid email specified");
				return;
			} else {
				StrOk(CGIStream);
				StrOp(CGIStream, "flushing equiv.struct");
				if (Equivs.replace_equiv("Root-Email",NewEmail,1)) StrOk(CGIStream); else StrError(CGIStream);
			}
		} else StrError(CGIStream, "ignoring");
	} else StrOk(CGIStream, "ignoring");
	
	CString NewBlank(Params.get_entry("BLANK"));	
	CString OldBlank(Equivs.get_value(CGIStream, "BLANK", 0)); 
	StrOp(CGIStream, "checking for new blank url");
	if (NewBlank.StrLength()) {
		if (NewBlank != OldBlank){
			StrOk(CGIStream);
			StrOp(CGIStream, "flushing equiv.struct");
			if (Equivs.replace_equiv("Blank",NewBlank,1)) StrOk(CGIStream); else StrError(CGIStream);
		} else StrError(CGIStream, "ignoring");
	} else StrOk(CGIStream, "ignoring");
	
	CString NewMailServer(Params.get_entry("MAILSERVER"));	
	CString OldMailServer(Equivs.get_value(CGIStream, "MailServer", 0)); 
	StrOp(CGIStream, "checking for new mail server");
	if (NewMailServer.StrLength()) {
		if (NewMailServer != OldMailServer){
			StrOk(CGIStream);
			StrOp(CGIStream, "flushing equiv.struct");
			if (Equivs.replace_equiv("MailServer",NewMailServer,1)) StrOk(CGIStream); else StrError(CGIStream);
		} else StrError(CGIStream, "ignoring");
	} else StrOk(CGIStream, "ignoring");
	
	CString NewProxy(Params.get_entry("PROXY"));	
	CString OldProxy(Equivs.get_value(CGIStream, "Proxy", 0)); 
	StrOp(CGIStream, "checking for new proxy server");
	if (NewProxy.StrLength() || OldProxy.StrLength()) {
		if (NewProxy != OldProxy){
			StrOk(CGIStream);
			StrOp(CGIStream, "flushing equiv.struct");      
			if (Equivs.replace_equiv("Proxy",NewProxy,1)) StrOk(CGIStream); else StrError(CGIStream);      
		} else StrError(CGIStream, "ignoring");
	} else StrOk(CGIStream, "ignoring");	
}


int MHost::Accept(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs, CString& Victim){
	Victim = Params.get_entry("VICTIM");
	if (!(Victim.StrLength())) {
		return 0;
	}
	CString Password(Params.get_entry("PASSWORD"));  
	if ((Access.get_value(CGIStream, Victim, 0) != Password)||(!Password.StrLength())) {
		if ((Access.get_value(CGIStream, "root", 0) != Password)||(!Password.StrLength())) {
			return 0;
		}
	}
	return 1;
}

int MHost::GAPostCommon(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs, CString& Victim){
	StrOp(CGIStream, "checking for a selected data alias");
	Victim = Params.get_entry("VICTIM");
	if (!(Victim.StrLength())) {
		StrError(CGIStream, "none chosen - please select your script alias");
		return 0;
	} else StrOk(CGIStream);
	CString Password(Params.get_entry("PASSWORD"));  
	StrOp(CGIStream, "checking administrative password (current alias administrator)");
	if ((Access.get_value(CGIStream, Victim, 0) != Password)||(!Password.StrLength())) {
		StrError(CGIStream);
		StrOp(CGIStream, "checking root password (entire group administrator)");    
		if ((Access.get_value(CGIStream, "root", 0) != Password)||(!Password.StrLength())) {
			StrError(CGIStream);
			CString RootEmail(Equivs.get_value(CGIStream, "Root-Email", 0)); if (!RootEmail.StrLength()) RootEmail = "root";
			CString EmailAdmin("mailto:"); EmailAdmin+=RootEmail; EmailAdmin+="?Subject=Password for "; EmailAdmin+=Victim; 
			EmailAdmin+=" - "; EmailAdmin+=getenv("SCRIPT_NAME");
			CGIStream << b_ << "forgot your password?" << br_ << a_.o("HREF",EmailAdmin.asString(),0) << "e-mail the administrator" << !a_ << elf;
			GAMakePasswordReminderForm(CGIStream, Victim, Equivs);
			return 0;
		} else StrOk(CGIStream);
	} else StrOk(CGIStream);
	StrOp(CGIStream, "primary access granted, calculating magic number"); StrOk(CGIStream);
	return 1;
}

void MHost::GAMakePasswordReminderForm(cgiOutStream& CGIStream, const CString& Victim, cgi_equiv_manager& Equivs) {
	Equivs.make_asocs();    
	Equivs.make_obvious();
	CString CurEmail(Equivs.EEmail.get_value(Victim)); CurEmail.StrTrim();  
	CString CurName;
	int iPos = CurEmail.Pos('<');
	if (iPos > 0) CurName = CurEmail.Copy(0, iPos).StrTrim();
	CurEmail.StrDelete(0, iPos+1);
	iPos = CurEmail.Pos('>');
	if (iPos > 0) CurEmail.StrDelete(iPos, CurEmail.StrLength());   
	
	CGIStream << form_.o("ACTION",Cmd.asString(),"METHOD","POST",0) << "\n";
	CGIStream << table_.o("WIDTH",M_TWIDTH, 0); 
	CGIStream << input_.o("NAME","VICTIM","TYPE","HIDDEN","VALUE",Victim.asString(),0) << "\n";  
	CGIStream << input_.o("NAME","ACTION","TYPE","HIDDEN","VALUE","PASSWD-REMINDER",0) << "\n";
	CGIStream << tr_ << td_ << "e-mail the password to: " << b_ << CurName << " &lt;<font color=\"red\">" << CurEmail << "</font>&gt; " << !b_ << input_.o("NAME","SUBMIT","TYPE","SUBMIT","VALUE","Email Password", 0) << !td_ << !tr_ << "\n";
	CGIStream << !table_ << !form_ << "\n";
}

CString MHost::GAMakeRoot(cgiOutStream& CGIStream, const CString& Victim, cgi_equiv_manager& Equivs){
	CString EVictimHTML(Victim); EVictimHTML+="HTML";    
	CString ERoot(Equivs.get_value(CGIStream, EVictimHTML, 0));
	if (ERoot.StrLength()) {
		int sPos = ERoot.InvPos('/'); if (sPos >= 0) ERoot.StrDelete(sPos+1, ERoot.StrLength());
		return ERoot;
	} else return EUndefinedRoot;
}

void MHost::GAPostEntry(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs){
	CString Victim;
	if (GAPostCommon(CGIStream, Params, Equivs, Victim)) {
		CGIStream << h3_ << "alias: " << b_ << Victim << !b_ << !h3_ << "\n";
		CString ERoot(GAMakeRoot(CGIStream, Victim, Equivs));
		if (ERoot != EUndefinedRoot) CGIStream << "equiv root: " << a_.o("HREF",ERoot.asString(),0) << ERoot << !a_ << elf;
		Equivs.make_asocs();    
		Equivs.make_obvious();    
		
		CString EAlias =  Equivs.EAssocs.get_value("aliased", Victim);
		if (EAlias.StrLength()) {
			CGIStream << a_.o("HREF", EAlias.asString(), 0) << EAlias << !a_ << elf;
			
			CString EAliasTmp;      
			CGIStream << "<ul>";
			CGIStream << "<li>[ "; EAliasTmp = EAlias; EAliasTmp+="+delete"; CGIStream << a_.o("HREF", EAliasTmp.asString(), 0) << "delete articles and archives" << !a_ << "]\n";
			CGIStream << "<li>[ "; EAliasTmp = EAlias; EAliasTmp+="+archive"; CGIStream << a_.o("HREF", EAliasTmp.asString(), 0) << "archive articles" << !a_ << "]\n";
			CGIStream << "<li>[ "; EAliasTmp = EAlias; EAliasTmp+="+merge"; CGIStream << a_.o("HREF", EAliasTmp.asString(), 0) << "merge/rename archives" << !a_ << "]\n";
			CGIStream << "<li>[ "; EAliasTmp = EAlias; EAliasTmp+="+edit"; CGIStream << a_.o("HREF", EAliasTmp.asString(), 0) << "edit/moderate articles" << !a_ << "]\n";	
			CGIStream << "</ul>";			
		}
		
		GACache(CGIStream, Params, Equivs);   
		GADelete(CGIStream, Params, Equivs);   
		GAPasswd(CGIStream, Params, Equivs);
		GAEmail(CGIStream, Params, Equivs, Victim);
		GATemplate(CGIStream, Params, Equivs, Victim);
	}
}

void MHost::GAPostEmail(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs){
	CString Victim;
	if (GAPostCommon(CGIStream, Params, Equivs, Victim)) {
		CString NA_Email = Params.get_entry("ADMIN-EMAIL");
		CString NA_Name = Params.get_entry("ADMIN-NAME");
		StrOp(CGIStream, "checking e-mail", NA_Email);
		CMail Mail(NA_Email);
		if ((!NA_Email.StrLength())||(!Mail.isGoodInternet())) {
			StrError(CGIStream, "no email or invalid email specified");
			return;
		} else StrOk(CGIStream);
		StrOp(CGIStream, "checking name", NA_Name); 
		if (!NA_Name.StrLength()) {
			StrError(CGIStream, "no name specified");
			return;
		} else StrOk(CGIStream);
		StrOp(CGIStream, "writing name and email to equiv.struct");
		StrOp(CGIStream, Victim);
		if (!Equivs.write_email(NA_Name, NA_Email, Victim)) {
			StrError(CGIStream, "error, please contact the administrator");
		} else StrOk(CGIStream);    
	}  
}

void MHost::GAPostTemplate(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs){
	CString Victim;
	if (GAPostCommon(CGIStream, Params, Equivs, Victim)) {
		CString Template = Params.get_entry("TEMPLATE");    
		StrOp(CGIStream, "checking if URL is correct", Template);
		if (!(Template.StartsWith("http://"))) {
			StrError(CGIStream, "invalid url, must be http://...");
			return;
		} else StrOk(CGIStream);    
		StrOp(CGIStream, "writing name and email to equiv.struct");
		if (!Equivs.write_template(Template, Victim, GAMakeRoot(CGIStream, Victim, Equivs))) {
			StrError(CGIStream, "error, please contact the administrator");
		} else StrOk(CGIStream);    
	}  
}

void MHost::GAPostCache(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs){
	StrOp(CGIStream, "checking for a selected data alias");
	CString Victim = Params.get_entry("VICTIM");
	if (!(Victim.StrLength())) {
		StrError(CGIStream, "none chosen - please select your script alias");
		return;
	} else StrOk(CGIStream);
	
	CString cache_equiv = Equivs.get_value(CGIStream, "Cache", 0);
	if (cache_equiv.StrLength() == 0) cgi_error(CGIStream, "this server does not cache html retrievals");
	for (int i=0; i< Params.entries_count();i++)
		if (Params.get_name(i) == "ALIAS") {
			CString ce = Equivs.get_value(CGIStream, Params.get_entry(i), 0);
			if (ce.StrLength()) {	
				CGIStream << "updating cache for " << b_ << Params.get_entry(i) << !b_ << elf;
#ifdef _DEBUG
				debug++;
#endif
				nextgen_v2 dummy(ce, cache_equiv, 0);
#ifdef _DEBUG
				debug--;
#endif
			} else CGIStream << "no equivalence for " << b_ << ce << !b_ << elf;
			CGIStream.flush();
		}
		StrOp(CGIStream, "synching cache progress mechanism"); StrOk(CGIStream);
}

void MHost::GAPasswd(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs){
	CGIStream << hr_ << b_ << "3. modify administrator password:" << !b_ << "\n";
	CGIStream << form_.o("ACTION",Cmd.asString(),"METHOD","POST",0) << "\n";
	CGIStream << table_.o("WIDTH",M_TWIDTH, 0); 
	CGIStream << input_.o("NAME","VICTIM","TYPE","HIDDEN","VALUE",Params.get_entry("VICTIM").asString(),0) << "\n";
	CGIStream << input_.o("NAME","ACTION","TYPE","HIDDEN","VALUE","PASSWD-CHANGE",0) << "\n";
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "Current Password: " << !td_ << td_ << input_.o("NAME","PASS-OLD","TYPE","PASSWORD","SIZE",M_SIZE,0) << !td_ << !tr_ << "\n";
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "New Password:" << !td_ << td_ << input_.o("NAME","PASS-NEW","TYPE","PASSWORD","SIZE",M_SIZE, 0) << !td_ << !tr_ << "\n";
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "Confirm New Password:" << !td_ << td_ << input_.o("NAME","PASS-NEW2","TYPE","PASSWORD","SIZE",M_SIZE, 0) << !td_ << !tr_ << "\n";
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << !td_ << td_ << input_.o("NAME","SUBMIT","TYPE","SUBMIT","VALUE","Modify Password", 0) << !td_ << !tr_ << "\n";
	CGIStream << !table_ << !form_ << "\n";  
}

void MHost::GADelete(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs){
	CGIStream << hr_ << b_ << "2. delete/destroy your aGNeS News Forum (this cannot be undone):" << !b_ << "\n";
	CGIStream << form_.o("ACTION",Cmd.asString(),"METHOD","POST",0) << "\n";
	CGIStream << table_.o("WIDTH",M_TWIDTH, 0); 
	CGIStream << input_.o("NAME","VICTIM","TYPE","HIDDEN","VALUE",Params.get_entry("VICTIM").asString(),0) << "\n";
	CGIStream << input_.o("NAME","ACTION","TYPE","HIDDEN","VALUE","GADELETE",0) << "\n";
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "Password: " << !td_ << td_ << input_.o("NAME","PASSWORD","TYPE","PASSWORD","SIZE",M_SIZE,0) << !td_ << !tr_ << "\n";
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << !td_ << td_ << input_.o("NAME","SUBMIT","TYPE","SUBMIT","VALUE","Delete Alias", 0) << !td_ << !tr_ << "\n";
	CGIStream << !table_ << !form_ << "\n";
}

void MHost::GADeleteAll(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs) {
	Equivs.make_oce();
	
	CGIStream << hr_ << b_ << "Delete/destroy aliases (this cannot be undone):" << !b_ << "\n";
	CGIStream << form_.o("ACTION",Cmd.asString(),"METHOD","POST",0) << "\n";
	CGIStream << table_.o("WIDTH",M_TWIDTH, 0); 
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "Alias: " << !td_ << td_;  
	AliasSelect(CGIStream, Params, Equivs);
	CGIStream << !td_ << !tr_ << "\n";  
	CGIStream << input_.o("NAME","ACTION","TYPE","HIDDEN","VALUE","GADELETE",0) << "\n";
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "Password: " << !td_ << td_ << input_.o("NAME","PASSWORD","TYPE","PASSWORD","SIZE",M_SIZE,0) << !td_ << !tr_ << "\n";
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << !td_ << td_ << input_.o("NAME","SUBMIT","TYPE","SUBMIT","VALUE","Delete Alias", 0) << !td_ << !tr_ << "\n";
	CGIStream << !table_ << !form_ << "\n";
}

void MHost::GATemplate(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs, const CString& Victim){
	CGIStream << hr_ << b_ << "5. modify the URL of the template pages:" << !b_ << "\n";
	CGIStream << form_.o("ACTION",Cmd.asString(),"METHOD","POST",0) << "\n";
	CGIStream << table_.o("WIDTH",M_TWIDTH, 0); 
	CGIStream << input_.o("NAME","VICTIM","TYPE","HIDDEN","VALUE",Params.get_entry("VICTIM").asString(),0) << "\n";
	CGIStream << input_.o("NAME","ACTION","TYPE","HIDDEN","VALUE","GATEMPLATE",0) << "\n";
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "Password: " << !td_ << td_ << input_.o("NAME","PASSWORD","TYPE","PASSWORD","SIZE",M_SIZE,0) << !td_ << !tr_ << "\n";
	
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "Global Location: " << !td_ << td_ << input_.o("NAME","TEMPLATE","TYPE","TEXT","SIZE",M_SIZE,"VALUE", GAMakeRoot(CGIStream, Victim, Equivs).asString(), 0) << !td_ << !tr_ << "\n";
	
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "resolved:" << !td_ << td_;
	CString Tmp; CString HTMLVictim(Victim); HTMLVictim+="HTML";
	for (int i=0;i<Equivs.entries_count();i++) {
		Tmp = Equivs.get_name(i);
		if (Tmp.StartsWith(HTMLVictim)) CGIStream << Tmp << " = " << b_ << Equivs.get_value(CGIStream, Tmp) << !b_ << elf;
	}
	CGIStream << !td_ << !tr_ << "\n";
	
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << !td_ << td_ << input_.o("NAME","SUBMIT","TYPE","SUBMIT","VALUE","Modify Templates Location", 0) << !td_ << !tr_ << "\n";
	CGIStream << !table_ << !form_ << "\n";
}

void MHost::GAEmail(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs, const CString& Victim){
	Equivs.make_asocs();
	CString CurEmail(Equivs.EEmail.get_value(Victim)); CurEmail.StrTrim();
	CString CurName;
	int iPos = CurEmail.Pos('<');
	if (iPos > 0) CurName = CurEmail.Copy(0, iPos).StrTrim();
	CurEmail.StrDelete(0, iPos+1);
	iPos = CurEmail.Pos('>');
	if (iPos > 0) CurEmail.StrDelete(iPos, CurEmail.StrLength());
	
	CGIStream << hr_ << b_ << "4. change the administrative name/e-mail:" << !b_ << "\n";
	CGIStream << form_.o("ACTION",Cmd.asString(),"METHOD","POST",0) << "\n";
	CGIStream << table_.o("WIDTH",M_TWIDTH, 0); 
	CGIStream << input_.o("NAME","VICTIM","TYPE","HIDDEN","VALUE",Params.get_entry("VICTIM").asString(),0) << "\n";
	CGIStream << input_.o("NAME","ACTION","TYPE","HIDDEN","VALUE","GAEMAIL",0) << "\n";
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "Password: " << !td_ << td_ << input_.o("NAME","PASSWORD","TYPE","PASSWORD","SIZE",M_SIZE,0) << !td_ << !tr_ << "\n";
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "Name: " << !td_ << td_ << input_.o("NAME","ADMIN-NAME","TYPE","TEXT","SIZE",M_SIZE,"VALUE", CurName.asString(), 0) << !td_ << !tr_ << "\n";  
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "Email: " << !td_ << td_ << input_.o("NAME","ADMIN-EMAIL","TYPE","TEXT","SIZE",M_SIZE,"VALUE", CurEmail.asString(), 0) << !td_ << !tr_ << "\n";  
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << !td_ << td_ << input_.o("NAME","SUBMIT","TYPE","SUBMIT","VALUE","Update Name/Email", 0) << !td_ << !tr_ << "\n";
	CGIStream << !table_ << !form_ << "\n";
}

void MHost::GACache(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs){
	CString Victim(Params.get_entry("VICTIM"));
	CGIStream << hr_ << b_ << "1. cache updater:" << !b_ << "\n";
	CGIStream << form_.o("ACTION",Cmd.asString(),"METHOD","POST",0) << "\n";
	CGIStream << table_.o("WIDTH",M_TWIDTH, 0); 
	CGIStream << input_.o("NAME","VICTIM","TYPE","HIDDEN","VALUE",Victim.asString(),0) << "\n";
	CGIStream << input_.o("NAME","ACTION","TYPE","HIDDEN","VALUE","GACACHE",0) << "\n";
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "update for:" << !td_ << td_;
	
	CString cache_equiv = Equivs.get_value(CGIStream, "Cache", 0);
	if (cache_equiv.StrLength() == 0) CGIStream << "this server does not cache html retrievals";
	else {
		CVector<CString> c_equivs;
		int i;
		for (i=0;i<Equivs.entries_count();i++) {
			/*
			CString ce = Equivs.entry_manager::get_name(i);
			if (ce.EndsWith("Agnes") || ce.EndsWith("AgnesParent")) continue;
			if ((!ce.isSubStrI(Victim))&&((ce.isSubStrI("HTML") >= 0)||(ce.isSubStrI("IRMA")>=0)))  c_equivs+=ce;
			*/
			CString ce = Equivs.entry_manager::get_value(i);
			CString cn = Equivs.entry_manager::get_name(i);
			if (ce.StartsWith("http://") && !cn.isSubStrI(Victim)) c_equivs+=cn;
		}
		if (c_equivs.Count()) {
			c_equivs.QSort();
			for (i=0;i<c_equivs.Count();i++){
				CGIStream << input_.o("TYPE","checkbox","NAME", "ALIAS", "VALUE", c_equivs[i].asString(), "", "CHECKED",0);
				CGIStream << c_equivs[i] << font_.o("SIZE","-1",0) << " (" << Equivs.get_value(CGIStream, c_equivs[i], 0) << ")" << !font_;
				CGIStream << !input_ << elf;
			}    
			CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << !td_ << td_ << input_.o("NAME","SUBMIT","TYPE","SUBMIT","VALUE","Update Cache", 0) << !td_ << !tr_ << "\n";
		} else {
			CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGH","RIGHT", 0) << !td_ << td_ << "<b>no cached aliases found</b>" << !td_ << !tr_ << "\n";
		}
	}
	CGIStream << !table_ << !form_ << "\n";
}

void MHost::MakeDirectory(cgiOutStream& CGIStream, const CString& Tmp){
	CString Preserve = nextgen_v2::get_curdir();
	if (chdir(Tmp.asString()) < 0) {
		StrOp(CGIStream, "current directory is ");
		StrOk(CGIStream, Preserve);
		StrOp(CGIStream, "making ");		
		StrOk(CGIStream, Tmp);
		StrOp(CGIStream, "confirmed ");
		StrOk(CGIStream, nextgen_v2::check_dir(Tmp));
	}
	chdir(Preserve.asString());
}

void MHost::MakePaths(cgiOutStream& CGIStream) {
	CString CurDir(nextgen_v2::get_rootdir());	
	CString Tmp(CurDir); Tmp+="equiv";
	MakeDirectory(CGIStream, Tmp);		
	Tmp = CurDir; Tmp+="data";
	MakeDirectory(CGIStream, Tmp);
	Tmp = CurDir; Tmp+="data"; Tmp.append_backslash();Tmp+="agnes";
	MakeDirectory(CGIStream, Tmp);	
}

void MHost::GAGeneral(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs){		
	CGIStream << hr_ << b_ << "0. first time general setup:" << !b_ << "\n";
	CGIStream << form_.o("ACTION",Cmd.asString(),"METHOD","POST",0) << "\n";
	CGIStream << table_.o("WIDTH",M_TWIDTH, 0); 
	CGIStream << input_.o("NAME","ACTION","TYPE","HIDDEN","VALUE","GAGENERAL",0) << "\n";
	if (Access.get_value(CGIStream, "root", 0).StrLength()) CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "old admin password: " << !td_ << td_ << input_.o("NAME","PASSWORD","TYPE","PASSWORD","SIZE",M_SIZE,0) << !td_ << !tr_ << "\n";	
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "new admin password: " << !td_ << td_ << input_.o("NAME","NEWPASSWORD","TYPE","PASSWORD","SIZE",M_SIZE,0) << !td_ << !tr_ << "\n";
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "confirm new password: " << !td_ << td_ << input_.o("NAME","NEWPASSWORD2","TYPE","PASSWORD","SIZE",M_SIZE,0) << !td_ << !tr_ << "\n";
	CString RootEmail(Equivs.get_value(CGIStream, "ROOT-EMAIL", 0)); if (!RootEmail.StrLength()) RootEmail = "root";
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "new admin email: " << !td_ << td_ << input_.o("NAME","EMAIL","TYPE","TEXT","SIZE",M_SIZE,"VALUE",RootEmail.asString(),0) << !td_ << !tr_ << "\n";
	CString Blank(Equivs.get_value(CGIStream, "BLANK", 0)); if (!Blank.StrLength()) Blank = "/b.gif";
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "blank.gif URL:<br><font size=\"-1\">(ex: <b>http://www.vestris.com/b.gif</b>)</font> " << !td_ << td_ << input_.o("NAME","BLANK","TYPE","TEXT","SIZE",M_SIZE,"VALUE", Blank.asString(), 0) << !td_ << !tr_ << "\n";  	
	CString MailServer(Equivs.get_value(CGIStream, "MailServer", 0)); if (!MailServer.StrLength()) MailServer = "mail";
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "mail exchanger:<br><font size=\"-1\">(ex: <b>mail.myserver.com</b>)</font> " << !td_ << td_ << input_.o("NAME","MAILSERVER","TYPE","TEXT","SIZE",M_SIZE,"VALUE", MailServer.asString(), 0) << !td_ << !tr_ << "\n";
	CString ManageAdd(Equivs.get_value(CGIStream, "Manage-add", 0));
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "forum creation password:<br><font size=\"-1\">(used to create new forums, not required)</font>" << !td_ << td_ << input_.o("NAME","MANAGEADD","TYPE","TEXT","SIZE",M_SIZE,"VALUE", ManageAdd.asString(), 0) << !td_ << !tr_ << "\n";
	CString ProxyServer(Equivs.get_value(CGIStream, "PROXY", 0));
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "proxy setting:<br><font size=\"-1\">(ex: <b>cuiproxy.unige.ch:3128</b>, not required)</font> " << !td_ << td_ << input_.o("NAME","PROXY","TYPE","TEXT","SIZE",M_SIZE,"VALUE", ProxyServer.asString(), 0) << !td_ << !tr_ << "\n";
	// CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "local cache directory (ex: /home/cgi-bin/cache): " << !td_ << td_ << input_.o("NAME","CACHE","TYPE","TEXT","SIZE",M_SIZE,"VALUE", Cache.asString(), 0) << !td_ << !tr_ << "\n";
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << !td_ << td_ << input_.o("NAME","SUBMIT","TYPE","SUBMIT","VALUE","Generate Base", 0) << !td_ << !tr_ << "\n";
	CGIStream << !table_ << !form_ << "\n";

	CGIStream << hr_ << b_ << "1. test mail server:" << !b_ << "\n";
	CGIStream << form_.o("ACTION",Cmd.asString(),"METHOD","POST",0) << "\n";
	CGIStream << input_.o("NAME","ACTION","TYPE","HIDDEN","VALUE","TESTMAILEXCHANGER",0) << "\n";
	CGIStream << input_.o("NAME","SUBMIT","TYPE","SUBMIT","VALUE",MailServer.asString(), 0) << "\n";
	CGIStream << !form_ << "\n";
}
