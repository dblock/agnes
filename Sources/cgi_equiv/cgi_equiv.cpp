#include <agnespch.hpp>

cgi_equiv_manager * current_cgi_equiv_manager = 0;

void output_current_equivs(cgiOutStream& CGIStream){
	if (current_cgi_equiv_manager) current_cgi_equiv_manager->output_equivs(CGIStream);
	else {
		current_cgi_equiv_manager = new cgi_equiv_manager();
		current_cgi_equiv_manager->output_equivs(CGIStream);
	}
}

void cgi_equiv_manager::set_value_oce(cgi_entry_manager& EAlias, const CString& source, CString target){
	if ((source.EndsWith("Agnes")) || (source.EndsWith("Guest")) || (source.EndsWith("Links")) || (source.EndsWith("Forum"))) {    
		EEmail.set_value(source, target);
		int iPos = target.Pos('<');    
		target.StrDelete(0, iPos+1);
		iPos = target.Pos('>');
		target.StrDelete(iPos, target.StrLength());          
		EAlias.set_value(source, target);
	}
}

void cgi_equiv_manager::add_value_oce(cgi_entry_manager& EAlias, const CString& source, const CString& target, const CString EMail){
	if (source.StrLength()){
		if (source[0] != '#') {
			CVector<CString> TokensLeft; source.Tokenizer('+', TokensLeft);
			CVector<CString> TokensRight; target.Tokenizer('+', TokensRight);
			if (TokensLeft.Count() > 1) {
				if (TokensLeft.Count() > TokensRight.Count()) {
					
				} else {
					for (int i=1;i<TokensLeft.Count();i++) {
						CString TL(TokensLeft[0]); TL+=TokensLeft[i];
						CString TR(TokensRight[0]); TR+=TokensRight[i];	    
						set_value_oce(EAlias, TL, EMail);
					}
				}
			} else set_value_oce(EAlias, source, EMail);
		}
	}
}

void cgi_equiv_manager::make_oce(void){
	if (EAlias.entries_count()) return;
	CString current_email;  
	nextgen_v2 hfile(equiv_file);
	CString s;
	while (!hfile.feofc()) {
		s = hfile.read_line();
		s.StrTrim32();
		if (!(s.StrLength())) continue;
		if (s[0] == '#') {
			s.StrTrim('#','#');
			if ((s.isSubStr('<')>=0)&&(s.isSubStr('>')>=0)) current_email=s;
		} else {
			CVector<CString> Tokens; s.Tokenizer(',', Tokens);
			if (Tokens.Count()==2) {	
				add_value_oce(EAlias, Tokens[0], Tokens[1], current_email);
			}
		}
	}
}

CVector<CString> cgi_equiv_manager::make_asocs(void){
	CVector<CString> Vector;
	CString e_name;
	CString e_name_short;
	CString e_value;
	CString cgi_desig;

	CString cgi_agnes_desig = getenv("SCRIPT_NAME");
	CString cgi_guest_desig = "guest";
	CString cgi_forum_desig = "forum";
	CString cgi_link_desig = "links";
#ifndef ANSI
	cgi_guest_desig +=".exe";
	cgi_forum_desig +=".exe";
	cgi_link_desig += ".exe";
#endif

	for (int i=0;i<entries_count();i++){
		e_name = entry_manager::get_name(i);
		e_value = entry_manager::get_value(i);
		
		if  (e_name.EndsWith("HTML")){
		        if (e_name.isSubStrI("agnes") >= 0) cgi_desig = cgi_agnes_desig;
			else if (e_name.isSubStrI("guest") >= 0) cgi_desig = cgi_guest_desig;
			else if (e_name.isSubStrI("forum") >= 0) cgi_desig = cgi_forum_desig;
			else if (e_name.isSubStrI("link") >= 0) cgi_desig = cgi_link_desig;
			else cgi_desig.Free();
			
			if (cgi_desig.StrLength()) {			  
				e_name_short=e_name.Copy(0, e_name.StrLength() - strlen("HTML"));
				
				if (entry_manager::get_value(e_name_short).StrLength()) {
					cgi_desig+="?";
					cgi_desig+=e_name_short;
					cgi_desig+="+";
					cgi_desig+=e_name;	  	  
					EAssocs.set_value(cgi_desig, EAlias.get_value(e_name_short));	 
					if (admin_manager.admin("deleted", e_name_short)) EAssocs.set_value("deleted", cgi_desig, e_name_short);
					EAssocs.set_value("aliased", e_name_short, cgi_desig);	  
				}
			}
		} 
		CString Tmp;
		Tmp+=e_name;
		Tmp+="::";
		if (e_name.isSubStr("HTML") >= 0) {            
			Tmp+=a_.o("HREF", e_value.asString(), 0);
			Tmp+=e_value;
			Tmp+=!a_;      
		} else {    
			Tmp+=e_value;     
		}
		Tmp+=elf;
		Vector+=Tmp;
	}
	Vector.QSort();
	return Vector;
}

void cgi_equiv_manager::make_obvious(int nArticleAge, int GenSelection){ 
	if (EObvious.Count()) return;  
	make_oce();
	make_asocs();
	
	l_agnes = 0;
	l_guest = 0;
	l_links = 0;
	l_forum = 0;

	for (int k=0;k<EAssocs.entries_count();k++) {
        CString Current;
		CString EName(EAssocs.get_name(k));
		
		if ((EName.isSubStr("agnes?") >= 0) || (EName.isSubStr("agnes.") >= 0)) {
            recent_manager RecentManager(NULL);
			CString Alias = EName;
			int sPos = Alias.Pos('?');
			int fPos = Alias.Pos('+');
			if ((sPos != -1)&&(fPos != -1)&&(fPos > sPos)) {
				Alias = Alias.Copy(sPos+1, fPos-sPos-1);
    
                int nLastPostAge = -1;
                CVector<CString> LineVector;
				nextgen_v2 Header(entry_manager::get_value(Alias));	

				if (Header.contained.StrLength()) {
					int lPos = Header.contained.StrLength()-1;
					while ((Header.contained[lPos] < ' ')&&(Header.contained[lPos] > 0)) lPos--;
					while ((Header.contained[lPos] != 13)&&(Header.contained[lPos] != 10)&&(lPos > 0)) lPos--;	  
					CString Line = Header.contained.Copy(lPos, Header.contained.StrLength());	  
					LineVector = Line.Tokenizer('\t');	  
					if (LineVector.Count() > 2) {                                                
                        // determine the age of the article
                        nLastPostAge = RecentManager.how_old(LineVector[2]);                        
                    }
                }

                if (GenSelection) {
                    Current += "<input type=\"checkbox\" name=\"victim\" value=\"";
                    Current += Alias;
                    Current += "\"";
                    
                    if ((nArticleAge != -1) && (nLastPostAge >= nArticleAge)) {
                        Current += " checked";
                    } else if (!admin_manager.admin(admin_class, Alias)) {
                        Current += " checked";
                    }
                    
                    Current += ">";
                }
                
                Current += "<a href=\"";
                Current += EName;
                Current += "\">";
                Current += EName;
                Current += "</a>";		
                
                Current += " (";
                Current += Header.line_count();
                Current += ")";
    
                if (LineVector.Count() > 2) {
                    Current += " <font size=\"-1\">";	
                    Current += LineVector[2];
                    if (nLastPostAge != -1) {
                        Current += " (";
                        Current += nLastPostAge;
                        Current += " days old)";
                    }                        
                    Current += "</font>";
                }
                
			}
		}
		else if (EName.isSubStr("guest?") >= 0) l_guest++;
		else if (EName.isSubStr("forum?") >= 0) l_forum++;
		else if (EName.isSubStr("links?") >= 0) l_links++;
				
        if (Current.StrLength())
            EObvious+=Current;
	}
	EObvious.QSort();
}

void cgi_equiv_manager::output_equivs(cgiOutStream& CGIStream){      
	make_oce();
	CGIStream << hr_ << "retreiving equivalence structures:" << hr_;
	CVector<CString> Vector = make_asocs();  
	for (int b=0;b<Vector.Count();b++) CGIStream << Vector[b] << "\n";
	
	CGIStream << hr_ << "constructing obvious associations:" << hr_;
	
	make_obvious();
	for(int k=0;k<EObvious.Count();k++) CGIStream << EObvious[k] << elf;
	
	CGIStream << hr_ << "agnes: " << l_agnes << " guest: " << l_guest << " forum: " << l_forum << " links: " << l_links << elf;  
	
	/*
    CGIStream << hr_ << "e-mail headers:" << hr_ << elf;
    CString pEmail;
    Email.QSort();
    for (int l=0;l<Email.Count();l++) {
    if (pEmail == Email[l]) continue;
    pEmail = Email[l];
    for (int m=0;m<Email[l].StrLength();m++)
    if (Email[l][m] == '<') CGIStream << "&lt;";
    else if (Email[l][m] == '>') CGIStream << "&gt;";
    else CGIStream << Email[l][m];
    CGIStream << elf;
    }
    */
}

cgi_equiv_manager::cgi_equiv_manager(const CString& _class){
	admin_class = _class;
	fill_equiv();
}

cgi_equiv_manager::cgi_equiv_manager(void){
	fill_equiv();
}

cgi_equiv_manager::~cgi_equiv_manager(void){
	if (current_cgi_equiv_manager == this) current_cgi_equiv_manager = NULL;
}

CString cgi_equiv_manager::get_value(cgiOutStream& CGIStream, const CString& name){
	return get_value(CGIStream, name, 1);
}

CString cgi_equiv_manager::get_value_admin(cgiOutStream& CGIStream, const CString& name){
	CString ires = get_value(CGIStream, name, 1);
	if (!(admin_class.StrLength())) return ires;  
	if (ires.StrLength()){
		int admin = admin_manager.admin(admin_class, name);
		if (!admin) {
			if (admin_manager.admin("expired", name)) {
				cgi_moved(CGIStream, get_value(CGIStream, name, 1));
			} else if (admin_manager.admin("deleted", name)) {
				cgi_error(CGIStream, "alias deleted by it's administrator using the MV4 scripting host");
			} else cgi_error(CGIStream, "typecast container verification has failed, please contact the admin");
		}
		CGIStream.UnLock();
		return ires;
	} else {
		CGIStream.UnLock();
		return "";
	}
}

CString cgi_equiv_manager::get_value(cgiOutStream& CGIStream, const CString& name, int fatal){
	CString result = entry_manager::get_value(name);
	if (!(result.StrLength())&&(fatal)) {    
		if (name == "equiv") output_equivs(CGIStream);
		CString Reject("equivalence access rejected for ");
		Reject+=name;
		cgi_error(CGIStream, Reject);
	}
	else return (result);
	return("");
}


void cgi_equiv_manager::add_equiv(const CString& source, const CString& target){
	if (source.StrLength()){
		if (source[0] != '#') {
			CVector<CString> TokensLeft; source.Tokenizer('+', TokensLeft);
			CVector<CString> TokensRight; target.Tokenizer('+', TokensRight);
			if (TokensLeft.Count() > 1) {
				if (TokensLeft.Count() > TokensRight.Count()) {
					cout << "cgi_equiv_manager::dropping malformed equiv at " << source << "=" << target << elf;
				} else {
					for (int i=1;i<TokensLeft.Count();i++) {
						CString TL(TokensLeft[0]); TL+=TokensLeft[i];
						CString TR(TokensRight[0]); TR+=TokensRight[i];
						entry_manager::set_value(TL, TR);
					}
				}
			} else entry_manager::set_value(source, target);
		}
	}
}

int cgi_equiv_manager::write_full_equiv(const CString& Name, const CString& EMail, 
										const CVector<CString>& FirstSetNames, const CVector<CString>& FirstSetValues,
										const CVector<CString>& SecondSetNames, const CVector<CString>& SecondSetValues){
	if (FirstSetNames.Count() != FirstSetValues.Count()) return 0;
	if (SecondSetNames.Count() != SecondSetValues.Count()) return 0;
	if (!equiv_file.StrLength()) {
		current_cgi_equiv_manager = this;
		equiv_file = entry_manager::make_equiv_path("equiv.struct");
	}

#ifndef ANSI
	chmod(equiv_file.asString(), _S_IREAD | _S_IWRITE);
#endif
    
	int outfile = v_open(equiv_file.asString(), O_WRONLY | O_APPEND | O_CREAT, S_IRWXU);
	if (outfile == -1) {

		perror("<br>Error writing equiv.struct:");

		return 0;

	}
	write(outfile, "\n", strlen("\n"));   
	if (Name.StrLength() || EMail.StrLength()) {
		write(outfile, "# ", strlen("# "));
		write(outfile, Name.asString(), Name.StrLength());
		write(outfile, " <", strlen(" <"));
		write(outfile, EMail.asString(), EMail.StrLength());
		write(outfile, ">\n", strlen(">\n"));             
	}
	int i;
	if (FirstSetNames.Count()) {
		for (i=0;i<FirstSetNames.Count();i++) { if (i) write(outfile, "+", strlen("+")); write(outfile, FirstSetNames[i].asString(), FirstSetNames[i].StrLength());}
		write(outfile, ",", strlen(","));
		for (i=0;i<FirstSetValues.Count();i++) { if (i) write(outfile, "+", strlen("+")); write(outfile, FirstSetValues[i].asString(), FirstSetValues[i].StrLength());}
		write(outfile, "\n", strlen("\n"));   
	}
	if (SecondSetNames.Count()) {
		for (i=0;i<SecondSetNames.Count();i++) { if (i) write(outfile, "+", strlen("+")); write(outfile, SecondSetNames[i].asString(), SecondSetNames[i].StrLength());}
		write(outfile, ",", strlen(","));
		for (i=0;i<SecondSetValues.Count();i++) { if (i) write(outfile, "+", strlen("+")); write(outfile, SecondSetValues[i].asString(), SecondSetValues[i].StrLength());}
		write(outfile, "\n", strlen("\n"));      
	}
	close(outfile);
	return 1;
}

void cgi_equiv_manager::fill_equiv(void){
	current_cgi_equiv_manager = this;
	equiv_file = entry_manager::make_equiv_path("equiv.struct");
	nextgen_v2 hfile(equiv_file);
	CString s;
	while (!hfile.feofc()) {
		s = hfile.read_line();    
		s.StrTrim32();
		if (!(s.StrLength())) continue;
		if (s[0] == '#') {
			s.StrTrim('#','#');
			if ((s.isSubStr('<')>=0)&&(s.isSubStr('>')>=0)) Email+=s;
		} else {
			CVector<CString> Tokens; s.Tokenizer(',', Tokens);
			if (Tokens.Count()==2) {
				add_equiv(Tokens[0], Tokens[1]);
			} //else cout << "cgi_equiv_manager::fill_equiv() - malformed equiv at " << s << elf;
		}
	} 
#ifdef proxy
	Proxy = get_equiv("Proxy", 0);
#endif
}

int cgi_equiv_manager::delete_template(cgiOutStream& CGIStream, const CString& Alias) {
    
	nextgen_v2 hfile(equiv_file);

#ifndef ANSI
	chmod(equiv_file.asString(), _S_IREAD | _S_IWRITE);
#endif
    
	CString CommaAlias(Alias); CommaAlias+="+Parent+,";
	CString HTMLCommaAlias(Alias); HTMLCommaAlias+="HTML++";
        
	int sPos, j, Result = 0;
    
    // CGIStream << "{<pre>" << hfile.contained.asString() << "</pre>}";
    
    int nLines = 0;
    
    CString EquivContents;
    for (int i=0;i<hfile.contained.StrLength();i++) {
        if (hfile.contained[i] == 10) {
            if (nLines < 2) {
                EquivContents += (char) 13;
                EquivContents += (char) 10;
            }
            nLines++;
        } else if ((hfile.contained[i] == 13) || ((hfile.contained[i] < ' ') && (hfile.contained[i] >= 0))) {
            continue;
        } else {
            nLines = 0;
            EquivContents += hfile.contained[i];
        }
    }
    
	sPos = EquivContents.isSubStr(CommaAlias);    
    // CGIStream << "[" << CommaAlias << "](" << sPos << ")";
	if (sPos >= 0) {
		for (j = sPos;j<EquivContents.StrLength();j++) 
            if (EquivContents[j] == 10) 
                break;
		while ((j < EquivContents.StrLength())&&(EquivContents[j] > 0)&&(EquivContents[j] < ' ')) 
            j++;
		if (j != sPos) {
			EquivContents.StrDelete(sPos, j - sPos);
		}
        CGIStream << "[Alias]";
		Result++;
	}
    
	sPos = EquivContents.isSubStr(HTMLCommaAlias);
    // CGIStream << "[" << HTMLCommaAlias << "(" << sPos << ")";
	if (sPos >= 0) {
		for (j = sPos;j<EquivContents.StrLength();j++) 
            if (EquivContents[j] == 10) 
                break;
		while ((j < EquivContents.StrLength())&&(EquivContents[j] > 0)&&(EquivContents[j] < ' ')) j++;
		if (j != sPos) {
			EquivContents.StrDelete(sPos, j - sPos);
		}
        CGIStream << "[HtmlAlias]";
		Result++;
	}
    
	if (sPos >= 0) {    
		sPos--;
		while ((sPos >= 0)&&(EquivContents[sPos] < ' ')) 
            sPos--;        
		for (j = sPos;j>=0;j--) {      
			if ((EquivContents[j]>0)&&(EquivContents[j] < ' ')) break;
		}
		if ((j!=sPos)&&(j >= 0)) {
			j++;      
            // CGIStream << "[# - " << EquivContents[j] << "]";
			if (EquivContents[j] == '#') {
				EquivContents.StrDelete(j,sPos-j+1);
                CGIStream << "[Email]";
				Result++;
			}
		}
	}
	
	int outfile = v_open(equiv_file.asString(), O_WRONLY | O_TRUNC | O_CREAT, S_IRWXU);  
	if (outfile == -1) {
		CGIStream << "<br>Error writing equiv.struct:";
		return 0;
	}
    
	write(outfile, EquivContents.asString(), EquivContents.StrLength());
	close(outfile);
    
    if (Result != 3) {
        CGIStream << "{<pre>" << EquivContents.asString() << "</pre>}";
    }    
    
	return (Result == 3);
}

int cgi_equiv_manager::write_template(const CString& Template, const CString& Alias, const CString& OldTemplate){  
	nextgen_v2 hfile(equiv_file);

#ifndef ANSI
	chmod(equiv_file.asString(), _S_IREAD | _S_IWRITE);
#endif
    
	int outfile = v_open(equiv_file.asString(), O_WRONLY | O_TRUNC | O_CREAT, S_IRWXU);   

	if (outfile == -1) {

		perror("<br>Error writing equiv.struct:");

		return 0;

	}
	

	CString Line;
    CString PlusAlias(Alias); PlusAlias+="HTML++";
    CString CommaAlias(Alias); CommaAlias+="HTML,";
    int Result = 0;    
    if (!hfile.void_read) {         
		while (!hfile.feofc()) {
			Line = hfile.read_line();
			if (Line.StrLength() && (!Result)) {
				if (Line.StartsWith(PlusAlias) || Line.StartsWith(CommaAlias)) {
					int iPos = Line.isSubStr(OldTemplate);
					if (iPos > 0) {	      
						Line.StrDelete(iPos, OldTemplate.StrLength());
						if (Template[Template.StrLength()-1] != '/') Line.StrInsert(iPos, "/");
						Line.StrInsert(iPos, Template);
						Result = 1;
					}	    
				}
			}
			write(outfile, Line.asString(), Line.StrLength());
			write(outfile, "\n", strlen("\n"));
		}      
		return Result;
    }
    close(outfile);	
	return 0;  
}

int cgi_equiv_manager::write_email(const CString& Name, const CString& Email, const CString& Alias){    
	nextgen_v2 hfile(equiv_file);

#ifndef ANSI

	chmod(equiv_file.asString(), _S_IREAD | _S_IWRITE);

#endif
	int outfile = v_open(equiv_file.asString(), O_WRONLY | O_TRUNC | O_CREAT, S_IRWXU);   

	if (outfile == -1) {

		perror("<br>Error writing equiv.struct:");

		return 0;

	}

	CString First;
	CString Second;
	CString PlusAlias(Alias); PlusAlias+="+";
	CString CommaAlias(Alias); CommaAlias+=",";
	int Result = 0;     
	if (!hfile.void_read) {
		First = hfile.read_line();
		while (!hfile.feofc()) {
			Second = hfile.read_line();
			if (First.StrLength() && (First[0] == '#') && (!Result)) {
				if (Second.StartsWith(PlusAlias) || Second.StartsWith(CommaAlias)) {
					First = "# "; First+=Name; First+=" <"; First+=Email; First+=">";
					Result = 1;
				}
			}
			write(outfile, First.asString(), First.StrLength());
			write(outfile, "\n", strlen("\n"));
			First = Second;	
		}
		write(outfile, Second.asString(), Second.StrLength());
		write(outfile, "\n", strlen("\n"));
		close(outfile);
		return Result;
	}
	close(outfile);
	return 0;  
}

int cgi_equiv_manager::replace_equiv(const CString& Name, const CString& Value, int Append){
	nextgen_v2 hfile(equiv_file);  
#ifndef ANSI

	chmod(equiv_file.asString(), _S_IREAD | _S_IWRITE);

#endif

	int outfile = v_open(equiv_file.asString(), O_WRONLY | O_TRUNC | O_CREAT, S_IRWXU);  
	if (outfile == -1) {

		perror("<br>Error writing equiv.struct:");

		return 0;

	}

	CString Line;    
	CString CommaAlias(Name); CommaAlias+=",";
	int Result = 0;    
	if (!hfile.void_read) {         
		while (!hfile.feofc()) {
			Line = hfile.read_line();
			if (hfile.feofc() && (!Line.StrLength())) break;
			if (Line.StrLength() && (!Result)) {	  
				if (Line.StartsWith(CommaAlias)) {
					if (Value.StrLength()) {
						Line = CommaAlias;
						Line += Value;    
						Result = 1;
					} else {
						Result = 1;
						continue;
					}
				}
			}
			//cout << "[" << Line << "]<br>\n";
			write(outfile, Line.asString(), Line.StrLength());
			write(outfile, "\n", strlen("\n"));	
		}
	}
	if (((!Result) && Append) && (Value.StrLength())) {
		//cout << "[" << CommaAlias << Value << "]<br>\n";
		write(outfile, CommaAlias.asString(), CommaAlias.StrLength());
		write(outfile, Value.asString(), Value.StrLength());
		write(outfile, "\n", strlen("\n"));      
		return 1;
	}
	return Result;
}
