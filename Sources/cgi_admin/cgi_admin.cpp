#include <agnespch.hpp>

cgi_admin_manager * current_cgi_admin_manager = 0;

void output_current_admins(cgiOutStream& CGIStream){
	if (current_cgi_admin_manager) current_cgi_admin_manager->output_admins(CGIStream);
	else {
		current_cgi_admin_manager = new cgi_admin_manager();
		current_cgi_admin_manager->output_admins(CGIStream);
	}
}

cgi_admin_manager::cgi_admin_manager(void){
	current_cgi_admin_manager = this;
	fill_admin();
}

cgi_admin_manager::~cgi_admin_manager(void){
	if (current_cgi_admin_manager == this) current_cgi_admin_manager = 0;
}

int cgi_admin_manager::admin(const CString& class_name, const CString& category){  
	if (!(entry_manager::get_value(category, class_name).StrLength())) return 0; else return 1;
}

void cgi_admin_manager::output_admins(cgiOutStream& CGIStream){
	CGIStream << hr_ << "retreiving administration equivalence structures:" << hr_;
	for (int i=0;i<entry_manager::class_count();i++){
		CGIStream << entry_manager::get_class(i);
		for (int j=0;j<entry_manager::class_count(i);j++) {
			CGIStream << " : " << entry_manager::get_name(i, j);
		}
		CGIStream << elf;
	} 
}

void cgi_admin_manager::add_admin(const CString& class_, const CString& category){ 
	if (class_.StrLength()&&(class_[0]!='#')) {
		CVector<CString> Tokens; category.Tokenizer('+',Tokens);
		for (int i=0;i<Tokens.Count();i++){		
			entry_manager::add_value(Tokens[i], class_, Tokens[i]);
		}
	}
}

void cgi_admin_manager::fill_admin(void){
	equiv_file = entry_manager::make_equiv_path("admin.struct");
	nextgen_v2 hfile(equiv_file);
	CVector<CString> Tokens;
	CString s;
	while (!hfile.feofc()) {
		s = hfile.read_line();
		s.StrTrim32();	
		s.Tokenizer(',', Tokens);	
		if (Tokens.Count() == 2)
			add_admin(Tokens[0], Tokens[1]);
	}
}

int cgi_admin_manager::write_full_admin(const CString& AdminClass, const CString& Equiv){
	if ((!AdminClass.StrLength())||(!Equiv.StrLength())) return 0;
	if (!equiv_file.StrLength()) {
		current_cgi_admin_manager = this;
		equiv_file = entry_manager::make_equiv_path("admin.struct");
	}
#ifndef ANSI

	chmod(equiv_file.asString(), _S_IREAD | _S_IWRITE);

#endif
	int hfile2 = v_open(equiv_file.asString(), O_WRONLY | O_APPEND | O_CREAT, S_IRWXU);    
	if (hfile2 == -1) {

		perror("<br>Error writing admin.struct:");

		return 0;  

	}
	write(hfile2, AdminClass.asString(), AdminClass.StrLength());    
	write(hfile2, ",", strlen(","));
	write(hfile2, Equiv.asString(), Equiv.StrLength());
	write(hfile2, "\n", strlen("\n"));  
	close(hfile2);
	return 1; 
}

int cgi_admin_manager::cgi_admin_write(void){
	return cgi_admin_write(entry_manager::make_equiv_path("admin.struct"));
}


int cgi_admin_manager::cgi_admin_write(const CString& container){
#ifndef ANSI

	chmod(equiv_file.asString(), _S_IREAD | _S_IWRITE);

#endif

	int hfile2 = v_open(container.asString(), O_WRONLY | O_TRUNC | O_CREAT, S_IRWXU);  
	if (hfile2 == -1) {

		perror("<br>Error writing admin.struct:");

		return 0;

	}
	for (int i=0;i<entry_manager::class_count();i++){
		for (int j=0;j<entry_manager::class_count(i);j++) {
			write(hfile2, entry_manager::get_name(i, j).asString(), entry_manager::get_name(i, j).StrLength());    
			write(hfile2, ",", strlen(","));
			write(hfile2, entry_manager::get_class(i).asString(), entry_manager::get_class(i).StrLength());
			write(hfile2, "\n", strlen("\n"));  
		}
	}
	close(hfile2);
	return 1;  
}
