#include <agnespch.hpp>

cgi_access_manager * current_cgi_access_manager = 0;

int check_root_access(const CString& password){
	if (!current_cgi_access_manager) current_cgi_access_manager = new cgi_access_manager();  
	if (current_cgi_access_manager->entry_manager::get_value("root") == password) return 0; else return 1;
}

void output_current_access(cgiOutStream& CGIStream){
	if (current_cgi_access_manager) current_cgi_access_manager->output_access(CGIStream);
	else {
		current_cgi_access_manager = new cgi_access_manager();
		current_cgi_access_manager->output_access(CGIStream);
	}
}

void cgi_access_manager::output_access(cgiOutStream& CGIStream){
	CGIStream << hr_ << "retreiving access password structures: " << hr_;
	for (int i=0;i<entries_count();i++)
		CGIStream << entry_manager::get_name(i) << " == " << entry_manager::get_value(i) << elf;
}


cgi_access_manager::cgi_access_manager(void){
	current_cgi_access_manager = this;
	fill_access();
}

cgi_access_manager::cgi_access_manager(const CString& container){
	current_cgi_access_manager = this;
	fill_access(container);
}


cgi_access_manager::~cgi_access_manager(void){
	if (current_cgi_access_manager==this)  current_cgi_access_manager = 0;
}

CString cgi_access_manager::get_value(cgiOutStream& CGIStream,const CString& name){
	return get_value(CGIStream, name, 1);
}

CString cgi_access_manager::get_value(cgiOutStream& CGIStream, const CString& name, int fatal){
	CString result = entry_manager::get_value(name);
	if ((!result.StrLength())&&fatal) cgi_error(CGIStream, "equivalence access denied");
	return result;
}


void cgi_access_manager::add_access(const CString& source, const CString& target){
	if ((!source.StrLength())||(source[0] == '#')) return;
	entry_manager::set_value(source, target);
}


void cgi_access_manager::fill_access(const CString& container){
	nextgen_v2 hfile(container);
	
	int recompile = 0;  
	CString s;
	CVector<CString> tokens;
	
	while (!hfile.feofc()) {
		s = hfile.read_line();
		s.Tokenizer(',', tokens);
		if (tokens.Count() == 2){
			if (tokens[1][0] == ' ') {
				recompile=1;
				tokens[1].StrTrim32();
			} else tokens[1].Decrypt();
			add_access(tokens[0], tokens[1]);
		} else if (tokens.Count() > 2) cerr << "malformed token at cgi_access::fill_access() " << tokens[0] << elf;
	}
	
	if (recompile) cgi_access_write(container);  
}

int cgi_access_manager::cgi_access_write(void){
	return cgi_access_write(entry_manager::make_equiv_path("access.struct"));
}

int cgi_access_manager::cgi_access_write(const CString& container){

#ifndef ANSI

	chmod(container.asString(), _S_IREAD | _S_IWRITE);

#endif
	int hfile2 = v_open(container.asString(), O_WRONLY | O_TRUNC | O_CREAT, S_IRWXU);    

	if (hfile2 == -1) {

		perror("<br>Error writing access.struct:");

		return 0;

	}

    for (int m = 0; m<entry_manager::entries_count();m++) {
		write(hfile2, entry_manager::get_name(m).asString(), entry_manager::get_name(m).StrLength());
		write(hfile2, ",", strlen(","));
		CString PasswdValue(entry_manager::get_value(m).Encrypt());
		write(hfile2, PasswdValue.asString(), PasswdValue.StrLength());
		write(hfile2, "\n", strlen("\n"));
    }
    close(hfile2);
    return 1;
}

void cgi_access_manager::fill_access(void){
	equiv_file = make_equiv_path("access.struct");	
	fill_access(equiv_file);
}



void cgi_access_manager::check_access(cgiOutStream& CGIStream, const CString& pass, const CString& id){
	if (!pass.StrLength()) cgi_error(CGIStream, "access denied");
	if ((entry_manager::get_value(id)!=pass)&&(check_root_access(pass))) cgi_error(CGIStream, "access denied");
}

int cgi_access_manager::write_full_access(const CString& Equiv, const CString& Password){
	if (!equiv_file.StrLength()) {
		current_cgi_access_manager = this;
		equiv_file = entry_manager::make_equiv_path("access.struct");
	}

#ifndef ANSI

	chmod(equiv_file.asString(), _S_IREAD | _S_IWRITE);

#endif

	int hfile2 = v_open(equiv_file.asString(), O_WRONLY | O_APPEND | O_CREAT, S_IRWXU);

	if (hfile2 == -1) {

		perror("<br>Error writing access.struct:");

		return 0;

	}
    CString EPassword(Password);    
    write(hfile2, Equiv.asString(), Equiv.StrLength());    
    write(hfile2, ",", strlen(","));

	CString EncryptedPassword(EPassword); EncryptedPassword.Encrypt();
    write(hfile2, EncryptedPassword.asString(), EncryptedPassword.StrLength());
    write(hfile2, "\n", strlen("\n"));  
    close(hfile2);
    return 1;      
}


