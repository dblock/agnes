#include <agnespch.hpp>

void ext_param::traverse_cline_internal(cgiOutStream& CGIStream, const CString& arg){
	if (arg == "equiv") output_current_equivs(CGIStream);
	else if (arg == "nocache") entry_manager::set_value("CACHED", "0");
	else if (arg == "admin") output_current_admins(CGIStream);
	else if (arg.StartsWith("access")) {
		CString passwd = arg.Copy(strlen("access"), arg.StrLength());
		if (check_root_access(passwd)) cgi_error(CGIStream, "access denied");
		output_current_access(CGIStream);
	} else if (arg == "version") {
		CGIStream << "(" << __DATE__ ") Daniel Doubrovkine - University of Geneva" << elf;
		CGIStream << "&copy; Vestris Inc. - 1994-1998 - All Rights Reserved" << elf;
	}
	else if (arg == "environ") {
#ifdef ANSI
		extern char **environ;
		char ** _environ = environ;
#endif
		int i=0; while (_environ[i]) CGIStream << _environ[i++]<<elf;
	}
}

CString ext_param::get_option(const CString& name, int index){
	return entry_manager::get_value(name, index);
}

CString ext_param::get_option(const CString& class_name, const CString& name){
	return entry_manager::get_value(class_name, name);
}

ext_param::ext_param(char * name, char * value, ...){
	entry_manager::set_value("CACHED","1");
	va_list options;
	va_start(options, value);
	do {
		assert(name);
		assert(value);      
		entry_manager::set_value(name, value);
		name = va_arg(options, char *);
		if (name) {
			value = va_arg(options, char *);
		} else value = 0;
	} while (name && value);
	va_end(options);
  
}

void ext_param::Reset(void){
	entry_manager::clear();
	entry_manager::set_value("CACHED","1");
}

ext_param::~ext_param(void){

}

CString ext_param::get_option(const CString& name){
	return entry_manager::get_value(name);
}

void ext_param::traverse_options(const CString& s){  
	CString Local(s);   
	CString Extract;
	CVector<CString> Tokens;
	Local.UpCase();
	int left = Local.isSubStr("<!--SET ", 0);
	while ((left>=0)&&(left < Local.StrLength())) {
		int right = Local.isSubStr((const char *) "-->", left);
        if (right>=0) {
			left+=strlen("<!--SET ");
			s.Copy(left, right - left, Extract); 
            
			int semiPos = Extract.Pos(':');
			int dashPos = Extract.isSubStr("--");
      
			if ((semiPos!=-1)&&(semiPos < dashPos)) {
				Extract.Tokenizer(':', Tokens);	
			} else if (dashPos != -1) {	
				Extract.Tokenizer("--", Tokens);      	
			}
      
			if (Tokens.Count() == 2) {	  	
				entry_manager::set_value(Tokens[0], Tokens[1]);
			} else if (Tokens.Count() == 3) 
                entry_manager::set_value(Tokens[0], Tokens[1], Tokens[2]);
            
			left = Local.isSubStr((const char *) "<!--SET ", right);
			right+=strlen("-->");      
		} else left+=strlen("<!--SET ");
	}
}


