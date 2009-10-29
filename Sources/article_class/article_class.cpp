#include <agnespch.hpp>

agnes_article::agnes_article(const CVector<CString>& _article_descriptor){
	got_inside = 0;
	parent = 0;
	traverse_descriptor(_article_descriptor);
}

agnes_article::~agnes_article(void){

}

void agnes_article::traverse_descriptor(const CVector<CString>& _agnes_descriptor){
	contents = _agnes_descriptor;
	if (contents.Count() < 6) { malformed = 1; return; }
	cut(contents[1], 512);
	cut(contents[3], 64);
	cut(contents[4], 64);
	cut(contents[5], 64);
	
	if (contents.Count() == 7) contents+="0"; // migrating moderate flag from old style aGNeS containers
	if (contents.Count() == 8) contents+="";
	if (contents.Count() == 9) contents+="";

	if (contents.Count() < 10) malformed = 1; else {
		malformed = 0;
		mlevel_generator();
	}
}


void agnes_article::article_descriptor(ofstream * container){
	for (int i=0;i<contents.Count() - 1;i++)
		(* container) << contents[i] << "\t";
	(* container) << contents[contents.Count()-1] << endl;
}

void agnes_article::mlevel_generator(void){
	CVector<CString> article_c_levels; article_node().Tokenizer('.', article_c_levels);
	article_levels.Clear();
	for (int i=0;i<article_c_levels.Count();i++) {
		int level = article_c_levels[i].Val();
		if (!level) {
			malformed = 1;
			return;
		}
		article_levels+=level;
	}
}

#ifdef ANSI
void agnes_article :: agnes_article_detar(const CString& root, const CString& archive) {
    if (!archive.StrLength())
        return;
       
    CString Command("tar fxz \"");
    Command += root;
    Command += archive;    
    Command += ".tar.gz\" ";
    Command += " --directory=\"";
    Command += root;
    Command += "\" \"";
    Command += archive;
    Command.append_backslash();    
    Command += article_node();
    Command += "\"";

    // cout << Command.asString() << endl;
    
    system(Command.asString());
}
#endif

CString& agnes_article::get_inside(const CString& root, const CString& archive, bool bTar){
	if (got_inside.StrLength()) 
        return (got_inside);
    
#ifdef ANSI
	if (bTar)
	  agnes_article_detar(root, archive);
#endif
    
	CString lRoot(root);
	if (archive.StrLength()) {
		lRoot+=archive;    
		lRoot.append_backslash();
	}
	lRoot += article_node();
	nextgen_v2 a_inside(lRoot);

	if (a_inside.contained.StrLength()) {    
        int DelimPos = a_inside.contained.isSubStr('\0');
        if (DelimPos >= 0) {
            got_inside = a_inside.contained.Copy(0, DelimPos);
        } else got_inside = a_inside.contained;
    }
    
	return(got_inside);
}

void agnes_article::write_contents(const CString& root, const CString& article_contents){
  CString lRoot(root);
  lRoot+=article_node();
  ofstream container_file(lRoot.asString(), ios::out | ios::trunc, filebuf::openprot);
  container_file << article_contents;
  container_file << '\0';
  article_descriptor(& container_file);
}

void agnes_article::send_parent_mail(agnes_article * article_parent, article_manager * template_manager, bool bTar){
	CEmail follow_bymail;
	if (article_parent->author_mail().StrLength()) {
                if (author_mail().StrLength() && !template_manager->hideEMAIL) {
                  follow_bymail.from = author_mail(); 
		} else {
                  follow_bymail.from = template_manager->get_option("MAIL-FROM");
                  if (!follow_bymail.from.StrLength())
                    follow_bymail.from = "anonymous@localhost";
                }

		follow_bymail.to = article_parent->author_mail();
		if (!template_manager->hideNAME) follow_bymail.name = author_name();
		else follow_bymail.name = "anonymous";
		follow_bymail.subject = article_title();
		follow_bymail.contents += template_manager->get_option("MAIL-HEADER");        
		follow_bymail.contents += article_parent->article_title();
		if (!template_manager->hideDATE) {
			follow_bymail.contents += " (";
			follow_bymail.contents += template_manager->shift_date(article_parent->article_date());
			follow_bymail.contents += ")\n";
		}
		if (!template_manager->hideNAME) follow_bymail.contents += author_name();
		if (author_mail().StrLength() && (!template_manager->hideEMAIL)){
			follow_bymail.contents += " (";
			follow_bymail.contents += author_mail();
			follow_bymail.contents += ") ";
		} else follow_bymail.contents += " ";
		follow_bymail.contents += template_manager->get_option("MAIL-WROTE");
		follow_bymail.contents += "\n\n";
		follow_bymail.contents += article_title();
		follow_bymail.contents += " - ";
		follow_bymail.contents +=  template_manager->shift_date(article_date());
		follow_bymail.contents += "\n\n";
		follow_bymail.contents += get_inside(template_manager->current_root, 0, bTar);
		follow_bymail.contents += "\n";
		follow_bymail.contents += template_manager->get_option("MAIL-FOOTER");
		CString ExtMailFooter(template_manager->equivs->entry_manager::get_value("MAIL-FOOTER"));
		ExtMailFooter.FormatC();
		follow_bymail.contents += ExtMailFooter;
		follow_bymail.contents += "\n[email generated by aGNeS (c) Vestris Inc., http://www.vestris.com]\n";
	}
	CString mailServer = template_manager->get_value("MailServer");
	if (!(mailServer.StrLength())) mailServer = template_manager->equivs->entry_manager::get_value("MailServer");
	if (!(mailServer.StrLength())) mailServer = "mail.vestris.com";
	follow_bymail.Send(mailServer);
}

int agnes_article::has_unmoderated_replies(void) const {
	if (parent) {
		for (int i=0;i<(* parent)[id]->replies.Count();i++)
			if (!((* parent)[id]->replies[i]->article->article_moderated()))
				return 1;
		return 0;
	} else return 0;
}
