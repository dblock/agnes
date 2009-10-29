/*
  aGNeS User Manager
  */

#include <agnespch.hpp>

CVector<CString> EmptyStringVector;

#define M_SIZE "40"
#define M_TWIDTH "100%"
#define TD1WIDTH "25%"

CUserManager::CUserManager(const CString& Directory)
  : ext_param(
	      "CLASS","CUserManager",
	      0) {
    m_Directory = Directory;
    m_Directory.append_backslash();
    m_OptionsFile = "agnes.opt";    
    m_UserFile = "agnes.usr";
    m_UsersModified = 0; 
    m_OptionsModified = 0;
    m_AccessManager = NULL; 
    UpdateFilenames();    
}

void CUserManager::UpdateFilenames(void) {
  m_UserFilePath = m_Directory; m_UserFilePath += m_UserFile;
  m_OptionsFilePath = m_Directory; m_OptionsFilePath += m_OptionsFile;  
  LoadOptions();
  LoadUsers();    
}

CString CUserManager::GetDirectory(void) const {
  return m_Directory;
}

int CUserManager::SetDirectory(const CString& Directory) {
  if (Directory != m_Directory) {
    m_Directory = Directory;
    m_Directory.append_backslash();
    UpdateFilenames();
    return 1;
  } else return 0;
}

void CUserManager::LoadOptions(void) {  
  nextgen_v2 hfile(m_OptionsFilePath);  
  CString s;
  CVector<CString> tokens;  
  while (!hfile.feofc()) {
    s = hfile.read_line();
    s.Tokenizer('=', tokens);
    if (tokens.Count() == 2){     
      set_value(tokens[0], tokens[1]);
    }
  } 
}

void CUserManager::LoadUsers(void) {
  if (m_AccessManager) delete m_AccessManager;
  m_AccessManager = new cgi_access_manager(m_UserFilePath);
}

void CUserManager::SaveOptions(void) {
  if (m_OptionsModified) {
    m_OptionsModified = 0;
    ofstream hfile2(m_OptionsFilePath.asString(), ios::out | ios::trunc, filebuf::openprot);
    if (!hfile2.bad()) {
      for (int m = 0; m<entry_manager::entries_count();m++)
	hfile2 << entry_manager::get_name(m) << "=" << entry_manager::get_value(m) << "\n";    
    }
  }
}

void CUserManager::SaveUsers(void) {
  if (m_AccessManager && m_UsersModified) {
    m_AccessManager->cgi_access_write(m_UserFilePath);
    m_UsersModified = 0;
  }
}

CUserManager::~CUserManager(void) {
  SaveUsers();
  SaveOptions();
  if (m_AccessManager) delete m_AccessManager;
}

void CUserManager::Add(const CString& Name, const CString& Password) {
  if (m_AccessManager) {
    m_AccessManager->entry_manager::set_value(Name, Password);
    m_UsersModified = 1;
  }  
}

void CUserManager::Set(const CString& Name, const CString& Value) {
  set_value(Name, Value);
  m_OptionsModified = 1;
}

int CUserManager::Validate(const CString& Name, const CString& Password) {
  if (!Name.StrLength()) return 0;  
  if (!Password.StrLength()) return 0;
  return (m_AccessManager->entry_manager::get_value(Name).isSame(Password));
  /*
    return (!((m_AccessManager->entry_manager::get_value(Name) != Password) && (check_root_access(Password))));  
  */
}

int CUserManager::GetUsermode(void) {
  return (get_value("UserMode").Val() == 1);
}

CString CUserManager::GetUsername(void) {
  return (get_value("USERNAME"));
}

CString CUserManager::GetUserpass(void) {
  return (get_value("USERPASS"));
}

void CUserManager::CreateUser(cgiOutStream& CGIStream, CString Name, CString Password, int Overwrite) {
  if (Name == "root") {
    CGIStream << "<font color=\"red\"><b>cannot modify the administrative password here, use ?manage</b></font>" << elf;
    return;
  }
  if (Password.StrLength() == 0) {
    CGIStream << "<font color=\"red\"><b>please specify a password!</b></font>" << elf;
    return;
  }
  if (!Overwrite && (m_AccessManager->entry_manager::get_value(Name).StrLength())) {
    CGIStream << "<font color=\"red\"><b>user " << Name << " already exists!</b></font>" << elf;
    return;
  }
  int u;
  for (u=0;u<Name.StrLength();u++) if (!isalnum(Name[u])) {
    CGIStream << "<font color=\"red\"><b>username " << Name << " contains invalid characters!</b></font>" << elf;
    return;
  }
  for (u=0;u<Password.StrLength();u++) if (!isalnum(Password[u])) {
    CGIStream << "<font color=\"red\"><b>password contains invalid characters!</b></font>" << elf;
    return;
  }  
  //if ((Name == "GUEST")&&(Password.StrLength() == 0)) Password = "VOID";
  Add(Name, Password);
  CGIStream << "<font color=\"green\"><b>user " << Name << " successfully created/modified!</b></font>" << elf;
}

void CUserManager::DeleteUser(cgiOutStream& CGIStream, CString Name) {
  if (Name == "root") {
    CGIStream << "<font color=\"red\"><b>cannot delete the root user</b></font>" << elf;
    return;
  }
  if (m_AccessManager->entry_manager::get_value(Name).StrLength() == 0) {
    CGIStream << "<font color=\"red\"><b>user " << Name << " does not exist!</b></font>" << elf;
    return;
  }
  m_AccessManager->entry_manager::Singles.Del(Name);
  m_UsersModified = 1;
  CGIStream << "<font color=\"green\"><b>user " << Name << " successfully deleted!</b></font>" << elf;
}

void CUserManager::Show(cgiOutStream& CGIStream, pParam_v2& Params, cgi_equiv_manager& Equivs, const CString& Victim) {
  CGIStream << "<hr><blockquote><h3><font color=\"red\">User Administration</font></h3>\n";
  CString Cmd = getenv("SCRIPT_NAME");
  if (Params.argv.Count() >= 2) {
    Cmd+="?"; Cmd+=Params.argv[1];
  }

  if (Params.isPost() == 1) {
    CString Action = Params.get_entry("ACTION");
    if (Action == "USER-ENABLEACCESS") Set("UserMode","1");
    else if (Action == "USER-DISABLEACCESS") Set("UserMode","0");
    else if (Action == "USER-CREATE") CreateUser(CGIStream, Params.get_entry("USERNAME"), Params.get_entry("USERPASS"));
    else if (Action == "USER-DELETE") DeleteUser(CGIStream, Params.get_entry("USERNAME"));
    else if (Action == "USER-MODIFY-PASSWORD") CreateUser(CGIStream, Params.get_entry("USERNAME"), Params.get_entry("USERPASS"), 1);
  }

  if (get_value("UserMode").Val() == 1) {   
    int u;
    CGIStream << hr_
	      << "1. current board users" << elf;    
    
    CVector<CString> Users; for (u=0;u<m_AccessManager->entries_count();u++) Users+=m_AccessManager->get_name(u); Users.QSort();
    CGIStream << "<ul>\n"; for (u=0;u<Users.Count();u++) CGIStream << "<li><a href=\"#USER" << Users[u] << "\">" << Users[u] << "</a><br>\n"; CGIStream << "</ul>\n";

    CGIStream << "<table border=\"1\" cellpadding=\"5\"><tr><td bgcolor=\"yellow\"><b>User Name</b></td><td bgcolor=\"yellow\"><b>Commands</b></td></tr>\n";
    for (u=0;u<m_AccessManager->entries_count();u++) {      
      CGIStream << "<tr><td align=\"center\" valign=\"center\"><b>" << "<a name=\"USER" << m_AccessManager->get_name(u) << "\">" << m_AccessManager->get_name(u) << "</a></b></td><td valign=\"center\">";

      CGIStream << form_.o("ACTION",Cmd.asString(),"METHOD","POST",0) << "\n"	
		<< "<font size=\"-2\">admin pass: " << input_.o("NAME","PASSWORD","TYPE","PASSWORD","SIZE","15",0) << "\n"    
		<< input_.o("NAME","VICTIM","TYPE","HIDDEN","VALUE",Victim.asString(),0) << "\n"
		<< input_.o("NAME","USERNAME","TYPE","HIDDEN","VALUE",m_AccessManager->get_name(u).asString(),0) << "\n"		
		<< input_.o("NAME","ACTION","TYPE","HIDDEN","VALUE","USER-DELETE",0) << "\n"
		<< input_.o("NAME","SUBMIT","TYPE","SUBMIT","VALUE","Delete User",0) << "\n"		
		<< "</font>" << !form_;      

      CGIStream << "<br>" << form_.o("ACTION",Cmd.asString(),"METHOD","POST",0) << "\n"	
		<< "<font size=\"-2\">admin pass: " << input_.o("NAME","PASSWORD","TYPE","PASSWORD","SIZE","15",0) << "\n"    
		<< input_.o("NAME","VICTIM","TYPE","HIDDEN","VALUE",Victim.asString(),0) << "\n"
		<< input_.o("NAME","USERNAME","TYPE","HIDDEN","VALUE",m_AccessManager->get_name(u).asString(),0) << "\n"		
		<< input_.o("NAME","ACTION","TYPE","HIDDEN","VALUE","USER-MODIFY-PASSWORD",0) << "\n"
		<< "New Password: " << input_.o("NAME","USERPASS","TYPE","PASSWORD","SIZE","15",0) << "\n"
		<< input_.o("NAME","SUBMIT","TYPE","SUBMIT","VALUE","Modify Password",0) << "\n"		
		<< "</font>" << !form_;  

      CGIStream << "</td></tr>" << elf;      
    }

    CGIStream << "<tr><td align=\"center\"><b>create a new user:</b></td><td>"
	      << form_.o("ACTION",Cmd.asString(),"METHOD","POST",0) << "\n"
	      << "<font size=\"-2\">admin pass: " << input_.o("NAME","PASSWORD","TYPE","PASSWORD","SIZE","15",0) << "\n"    		
	      << input_.o("NAME","SUBMIT","TYPE","SUBMIT","VALUE","Create a New User!", 0)
	      << input_.o("NAME","VICTIM","TYPE","HIDDEN","VALUE",Victim.asString(),0) << "<br>\n"
	      << input_.o("NAME","ACTION","TYPE","HIDDEN","VALUE","USER-CREATE",0) << "\n"
	      << "<br>Username: " << input_.o("NAME","USERNAME","TYPE","TEXT","SIZE",M_SIZE,0) << "\n"
	      << "<br>Password: " << input_.o("NAME","USERPASS","TYPE","PASSWORD","SIZE",M_SIZE,0) << "\n<br>"
	      << "</font>" << !form_;    
    CGIStream << "</tr></td></table>\n";
    
    CGIStream << hr_ 
	      << "2. disable the user access mode" << elf;	          
    CGIStream << form_.o("ACTION",Cmd.asString(),"METHOD","POST",0) << "\n";
    CGIStream << table_.o("WIDTH",M_TWIDTH, 0); 
    CGIStream << input_.o("NAME","VICTIM","TYPE","HIDDEN","VALUE",Victim.asString(),0) << "\n";  
    CGIStream << input_.o("NAME","ACTION","TYPE","HIDDEN","VALUE","USER-DISABLEACCESS",0) << "\n";
    CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "Current Password: " << !td_ << td_ << input_.o("NAME","PASSWORD","TYPE","PASSWORD","SIZE",M_SIZE,0) << !td_ << !tr_ << "\n";  
    CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "&nbsp;" << !td_ << td_ << input_.o("NAME","SUBMIT","TYPE","SUBMIT","VALUE","Disable User Access Mode!", 0) << !td_ << !tr_ << "\n";
    CGIStream << !table_ << !form_ << "\n"; 
    

  } else {
    CGIStream << hr_ 
	      << "1. create a user <b>root</b> with the current administrative password and enable the user access mode" << elf;	          
    CGIStream << form_.o("ACTION",Cmd.asString(),"METHOD","POST",0) << "\n";
    CGIStream << table_.o("WIDTH",M_TWIDTH, 0); 
    CGIStream << input_.o("NAME","VICTIM","TYPE","HIDDEN","VALUE",Victim.asString(),0) << "\n";  
    CGIStream << input_.o("NAME","ACTION","TYPE","HIDDEN","VALUE","USER-ENABLEACCESS",0) << "\n";
    CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "Current Password: " << !td_ << td_ << input_.o("NAME","PASSWORD","TYPE","PASSWORD","SIZE",M_SIZE,0) << !td_ << !tr_ << "\n";  
    CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "&nbsp;" << !td_ << td_ << input_.o("NAME","SUBMIT","TYPE","SUBMIT","VALUE","Enable User Access Mode!", 0) << !td_ << !tr_ << "\n";
    CGIStream << !table_ << !form_ << "\n"; 
  }
  CGIStream << "</blockquote>";
}

void CUserManager::Login(cgiOutStream& CGIStream, const CString& Victim, CCookieManager& CookieManager, pParam_v2& Params) {
  CString Cmd = getenv("REQUEST_URI");
  if (get_value("UserMode").Val() != 1) return;
  CString VictimName(Victim); VictimName+="-NAME";
  CString VictimPassword(Victim); VictimPassword+="-PASS";
    
  CString VictimPasswordNew(Victim); VictimPasswordNew+="-PASS-NEW";
  CString VictimPasswordNewConfirm(Victim); VictimPasswordNewConfirm+="-PASS-NEW-CONFIRM";

  CString VictimEmail(Victim); VictimEmail += "-EMAIL";
  CString VictimRealName(Victim); VictimRealName += "-REAL-NAME";

  CString Password = CookieManager.Get(VictimPassword); Password.Decrypt();
  
  /*
    CGIStream << "Database Password is: " << m_AccessManager->entry_manager::get_value(VictimName) << elf;
    CGIStream << "Checking with password: " << Password << elf;
    CGIStream << "Current password is: " << m_AccessManager->entry_manager::get_value(CookieManager.Get(VictimName)) << elf;   
  */
  
  if (!Validate(CookieManager.Get(VictimName), Password)) {
    cgi_access_manager AccessManager;
    if (
	(CookieManager.Get(VictimName) != "root") || 
	((Password != AccessManager.get_value(CGIStream, Victim)) &&
	 (Password != AccessManager.get_value(CGIStream, "root"))
	 )) {
      CGIStream << "<html><title>" << Victim << " BBS</title><body bgcolor=white>";
      CGIStream << form_.o("ACTION",Cmd.asString(),"METHOD","POST",0) << "\n";
      CGIStream << table_.o("WIDTH",M_TWIDTH,0);     
      CGIStream << tr_ << td_.o("ALIGN","RIGHT", "BGCOLOR", "YELLOW", 0) << b_ << "Login to " << Victim << ":" << !b_ << !td_ << !td_ << td_ << !tr_ << "\n";  
	  CGIStream << input_.o("NAME","ACTION","TYPE","HIDDEN","VALUE","BOARD-LOGIN",0) << "\n";
      CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "Username: " << !td_ << td_ << input_.o("NAME",VictimName.asString(),"TYPE","TEXT","SIZE",M_SIZE,0) << !td_ << !tr_ << "\n";      
      CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "Password: " << !td_ << td_ << input_.o("NAME",VictimPassword.asString(),"TYPE","PASSWORD","SIZE",M_SIZE,0) << !td_ << !tr_ << "\n";  
	  
      CGIStream << tr_ << td_.o("ALIGN","RIGHT", "BGCOLOR", "YELLOW", 0) << b_ << "Change Password:" << !b_ << !td_ << !td_ << td_ << !tr_ << "\n";
      CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "New Password: " << !td_ << td_  << input_.o("NAME",VictimPasswordNew.asString(),"TYPE","PASSWORD","SIZE",M_SIZE,0) << !td_ << !tr_ << "\n";  
      CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "Confirm New Password: " << !td_ << td_ << input_.o("NAME",VictimPasswordNewConfirm.asString(),"TYPE","PASSWORD","SIZE",M_SIZE,0) << !td_ << !tr_ << "\n";  
      
      /*
	CGIStream << tr_ << td_.o("ALIGN","RIGHT", "BGCOLOR", "YELLOW", 0) << b_ << "Change Properties:" << !b_ << !td_ << !td_ << td_ << !tr_ << "\n";
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "Name: " << !td_ << td_ << input_.o("NAME",VictimRealName.asString(),"TYPE","TEXT","SIZE",M_SIZE,0) << !td_ << !tr_ << "\n";  
	CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "Email: " << !td_ << td_ << input_.o("NAME",VictimEmail.asString(),"TYPE","TEXT","SIZE",M_SIZE,0) << !td_ << !tr_ << "\n";  
      */
      
      CGIStream << tr_ << td_.o("WIDTH",TD1WIDTH,"ALIGN","RIGHT", 0) << "&nbsp;" << !td_ << td_ << input_.o("NAME","SUBMIT","TYPE","SUBMIT","VALUE","Login !", 0) << !td_ << !tr_ << "\n";
      CGIStream << !table_ << !form_ << "\n</body></html>";
      CGIStream.flush();
      exit(0);
    }
  }

  set_value("USERNAME", CookieManager.Get(VictimName));
  set_value("USERPASS", CookieManager.Get(VictimPassword));
	  
  if (Params.isPost() == 1){
    CString NewPassword = Params.get_entry(VictimPasswordNew);
	CString NewPasswordConfirmation = Params.get_entry(VictimPasswordNewConfirm);
	if ((NewPassword.StrLength() || NewPasswordConfirmation.StrLength()) && (NewPassword != NewPasswordConfirmation)) {
	  CGIStream << "Error! Cannot change password, confirmation password does not match." << hr_;
	  return;
	}
	if (NewPassword.StrLength()) {
	  CGIStream << "Password for <b>" << get_value("USERNAME") << "</b> successfully changed." << hr_;
	  Add(get_value("USERNAME"), NewPassword);
	  CookieManager.Add(VictimPassword, NewPassword.Encrypt());
	  SaveUsers();
	}
  }
}
