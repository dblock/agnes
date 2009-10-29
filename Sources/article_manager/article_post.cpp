#include <agnespch.hpp>

void article_manager::edit_article(cgiOutStream& CGIStream){
	cgi_access.check_access(CGIStream, params->get_entry("pass"), params->argv[1]);
	header_html = 0;
	state = A_EDIT_SELECTED;
}

void article_manager::edit_post(cgiOutStream& CGIStream){
	article_tree * tree = get_tree(CGIStream, params->get_entry("parent"));
	if (!tree) cgi_error(CGIStream, "unable to find the edited article node");

	CString article_subject = params->get_entry("subject");
	CString author_name = params->get_entry("name");
	CString author_mail = params->get_entry("mail");
	CString article_face = params->get_entry("face");
	CString follow_mail = params->get_entry("bymail");
	CString moderated = params->get_entry("moderate");  
	/*
	  CString ip = getenv("REMOTE_ADDR");
	  CString Via;
	  Via = getenv("HTTP_VIA"); {if (Via.StrLength()) ip+="/via:";ip+=Via;}
	  Via = getenv("HTTP_X_FORWARDED_FOR"); {if (Via.StrLength()) ip+="/xff:";ip+=Via;}
	  Via = getenv("HTTP_FORWARDED"); {if (Via.StrLength()) ip+="/fwd:";ip+=Via;}  
	  CString host = getenv("REMOTE_HOST");
	*/
	if (!(article_subject.StrLength())) article_subject = "no subject";  
	if (!(author_name.StrLength())) if (author_mail.StrLength()) author_name = author_mail; else author_name = "anonymous";  
	//if (!(author_mail.StrLength())) author_mail.Free();  
	if (!(article_face.StrLength())) article_face = "0";
	if (!(follow_mail.StrLength())) follow_mail = "0"; else follow_mail = "1";
	if (!(moderated.StrLength())) moderated = "0"; else moderated = "1";
	//if (!(ip.StrLength())) ip.Free();
	//if (!(host.StrLength())) host.Free();

	CString article_contents = params->get_entry("article");
	if (!(article_contents.StrLength())) cgi_error(CGIStream, "please write something");
	
	tree->article->set_article_title(article_subject);
	tree->article->set_author_name(author_name);
	tree->article->set_author_mail(author_mail);
	tree->article->set_article_face(article_face);
	tree->article->set_follow_mail(follow_mail);
	tree->article->set_article_moderated(moderated);
	//tree->article->set_article_ip(ip);
	//tree->article->set_article_host(host);

	if (!tree->article->malformed){
		write_header(CGIStream);
		tree->article->write_contents(contents_root, article_contents);
	}
	state = A_EDIT;
}

void article_manager::delete_articles(cgiOutStream& CGIStream){
	cgi_access.check_access(CGIStream, params->get_entry("pass"), params->argv[1]);
	for (int i=params->entries_count();i;i--){
		if (params->get_name(i-1) == "victim") {
			CString remEntry = params->get_entry(i-1);
			CGIStream << "removing node (with children) [" <<remEntry << "]" << elf;
			article_tree * tree = get_tree(CGIStream, params->get_entry(i-1));
			if (tree){
				if (tree->article->parent) 
					for (int j=0;j<tree->article->parent->count();j++)
						if ((*(tree->article->parent))[j]==tree) {
							(*(tree->article->parent))-=j;
							break;
						}
			}
		} 
		else if (params->get_name(i-1) == "victim_archive"){
			CString remEntry = params->get_entry(i-1);
			if (!remEntry.StrLength())
				continue;
			CGIStream << "removing archive [" << remEntry << "]" << elf;
			CString archive_source(archives_root);
			archive_source+=params->get_entry(i-1);
			
			CString ListFile(archive_source);
			ListFile += ".list";
			unlink(ListFile.asString()); 

			CString Command("rm -r ");
			Command += archive_source;
			system(Command.asString());

			CString TarFile(archive_source);
			TarFile += ".tar.gz";
			unlink(TarFile.asString());
			
		}
	}
	write_header(CGIStream);
}

void article_manager::post_article_general(cgiOutStream& CGIStream, ilist <_article_tree *> & articles){
	int node = last_article_id(articles) + 1;
  
    CString bbs_post_alias = params->get_entry("bbs-alias");
    CString bbs_this_alias = params->argv[1];
    
    if (bbs_post_alias.StrLength() && bbs_this_alias.StrLength() && (bbs_this_alias != bbs_post_alias)) {
        cgi_error(CGIStream, "you are using a form from one board to post an article to an another");
        exit(0);
    }
    
	int smail = 0;
	CString article_subject = params->get_entry("subject").StrTrim32();
	CString author_name = params->get_entry("name").StrTrim32();

	if (user_manager && (user_manager->GetUsermode())) {
		if (user_manager->GetUsername() != author_name) {
			cgi_error(CGIStream, "illegal username");
			exit(0);
		}
	}

	CString author_mail = params->get_entry("mail").StrTrim32();
	CString article_face = params->get_entry("face").StrTrim32();
	CString follow_mail = params->get_entry("bymail").StrTrim32();
	CString ip = getenv("REMOTE_ADDR");
	CString Via;
	Via = getenv("HTTP_VIA"); {if (Via.StrLength()) ip+="/via:";ip+=Via;}
	Via = getenv("HTTP_X_FORWARDED_FOR"); {if (Via.StrLength()) ip+="/xff:";ip+=Via;}
	Via = getenv("HTTP_FORWARDED"); {if (Via.StrLength()) ip+="/fwd:";ip+=Via;}
	CString host = getenv("REMOTE_HOST");
  
	if (!(article_subject.StrLength())) {
		if (ext_param::get_option("FORCE-SUBJECT").Val()) cgi_error(CGIStream, "article subject is required");
		article_subject = "no subject";
	}
	if (!(author_name.StrLength())) {
		if (ext_param::get_option("FORCE-NAME").Val()) cgi_error(CGIStream, "author name is required");
		/* if (author_mail.StrLength()) author_name = author_mail; else */ author_name = "anonymous";
	}
	if (!(author_mail.StrLength())) {
		if (ext_param::get_option("FORCE-EMAIL").Val()) cgi_error(CGIStream, "author email is required");    
	} else {
		CMail Mail(author_mail);
		if (!Mail.isGoodInternet()) cgi_error(CGIStream, "a valid email address is required");  
	}
  
	if (!(article_face.StrLength())) article_face = "0";
	if (!(follow_mail.StrLength())) follow_mail = "0"; else follow_mail = "1";
	if (!(ip.StrLength())) ip.Free();
	if (!(host.StrLength())) host.Free();
  
	if (article_subject.Pos('<') >= 0) article_subject = ext_param::get_option("DUCON SUBJECT");
	if (author_name.Pos('<') >= 0) author_name = ext_param::get_option("DUCON NAME");
	if (author_mail.Pos('<') >= 0) author_mail = ext_param::get_option("DUCON MAIL");
  
	CString article_contents = params->get_entry("article");


	if (!(article_contents.StrLength())) cgi_error(CGIStream, "please write something");
  
	if (node > 1) {
		agnes_article * previous = articles[articles.count() - 1]->article;
		if (previous){
			CString previous_contents = previous->get_inside(contents_root, archive_category, bTar);
			if (previous_contents == article_contents) cgi_error(CGIStream, "please avoid posting an article twice");
		}
	}	
  
	article_tree * parent_tree = 0;
	CString article_node; article_node+=node;
	CString parent_node = params->get_entry("parent");
	if (parent_node.StrLength()){
		parent_tree = get_tree(CGIStream, parent_node);
		if (parent_tree) 
			if (parent_tree->article->follow_mail()) smail = 1;
		parent_node+=".";
		parent_node+=article_node;
		article_node = parent_node;
	}
	
	//char * descriptor = new char[64 + strlensecure(article_subject) + strlensecure(author_name)+strlensecure(author_mail)+strlensecure(article_face)+strlensecure(follow_mail) + strlensecure(ext_param::get_option("MODERATED"))];
	struct tm *newtime; time_t aclock; time( &aclock );	newtime = localtime( &aclock );
	CString article_date(asctime(newtime));
	article_date.StrTrim32();
	CVector<CString> descriptor;
	descriptor += article_node;
	descriptor += article_subject;
	descriptor += article_date;
	descriptor += author_name;
	descriptor += author_mail;
	descriptor += follow_mail;
	descriptor += article_face;
	descriptor += ext_param::get_option("MODERATED");
	descriptor += ip;
	descriptor += host;
	agnes_article * an_article = new agnes_article(descriptor);
	
	if (!an_article->malformed) {
		insert_article(an_article);
		if (params->get_entry("preview").StrLength()) {
			an_article->got_inside = article_contents;
			post_article_preview(CGIStream, an_article);
		} else {
			if (an_article->article_moderated()) postedModerated = 1;
			write_header(CGIStream);
			an_article->write_contents(contents_root, article_contents);
			CString mailServerEquivs = equivs->get_value(CGIStream, "MailServer", 0);
			CString mailServerParams = ext_param::get_option("MailServer");
			if (!(mailServerParams.StrLength())) ext_param::set_value("MailServer", mailServerEquivs);
			if ((smail)&&(parent_tree)) an_article->send_parent_mail(parent_tree->article, this, bTar);
		}
	} 
	
}

void article_manager::auto_archive(void) {
  if (archive_category.StrLength()) return;
  if (article_count < 400) return;
  int threshold_index = articles.count()/2 - 1;
  if (threshold_index <= 0) return;
  current_tree = articles[threshold_index];
  current_article = current_tree->article;
  if (!archives) archives = new article_archive_manager(archives_root, params, this);	
  cgiOutStream localCGIStream;
  localCGIStream.toSTALL();
  time_t l_time; time(&l_time);  
  CString NewAlias("AutoArchive "); 
  NewAlias += asctime(localtime(&l_time));
  NewAlias.Replace(':','-');
  archives->create_article_archive(localCGIStream, NewAlias.StrTrim32(), params, this, current_tree, current_article, bTar);  
}

void article_manager::archive_post(cgiOutStream& CGIStream){
	cgi_access.check_access(CGIStream, params->get_entry("pass"), params->argv[1]);	
	CString Victim = params->get_entry("victim");
	if (Victim.Pos('.') >= 0) cgi_error(CGIStream, "please select a topmost node of a thread");
	current_tree = get_tree(CGIStream, Victim);
	if (!current_tree) {
		//CGIStream << "[archiving entire structure]" << elf;
		//current_tree = articles[articles.count() - 1];
		//if (current_tree) current_article = current_tree->article;
		cgi_error(CGIStream, "archiving full structure has been disabled, please select a node");
	} else {
		current_article = current_tree -> article;
		if ((!current_article)||(!(current_article->get_next()))) {
			cgi_error(CGIStream, "archiving full structure has been disabled, please select a non-terminal node");
		}
		CGIStream << "[archiving inside and up to node (" << current_article->article_node() << ")]" << elf;
	}	
	if (!archives) archives = new article_archive_manager(archives_root, params, this);	
	archives->create_article_archive(CGIStream, params->get_entry("alias").StrTrim32(), params, this, current_tree, current_article, bTar);
}

void article_manager::search_post(cgiOutStream& CGIStream){
	state = A_SEARCH;
}

void article_manager::regexp_post(cgiOutStream& CGIStream){
	state = REGEXP_SEARCH;
}

int article_manager::regexp_search_text(cgiOutStream& CGIStream, const CString& RegExpExpression, CString& TargetExpression){
	/*
	  REGEXP
	*/
	int Result = 0;
	int nPos;
	int offset;
	int pReplaceLen;
	CRegExp r;
	char * str = TargetExpression.asString();
	r.RegComp(RegExpExpression.asString());
	CVector<int> rVector;
	while ((nPos = r.RegFind(str)) != -1){           
		pReplaceLen = r.GetFindLen();
		offset = str-TargetExpression.asString()+nPos;
		rVector+=offset;
		rVector+=offset+pReplaceLen;
		str+=nPos + pReplaceLen;    
		Result++;
	} 
	if (Result) for (int m=rVector.Count()-1;m>0;m-=2) {
		TargetExpression.StrInsert(rVector[m], "</b>");
		TargetExpression.StrInsert(rVector[m-1], "<b>");
	}  
	return Result;
}

int article_manager::search_text(cgiOutStream& CGIStream, CString& search_string, const CVector<CString>& SearchWords, const CString& case_sens, const CString& whole_words){
  int Pos = 0;
  int Found = 0;
  int FinalResult = 0;
  CString SearchWord;
  CString SearchString;
  // force copy
  SearchString.SetBuf(search_string.asString(), search_string.StrLength());
  if (!case_sens.StrLength()) 
    SearchString.UpCase();
    
  for (register int i=0;i < SearchWords.Count(); i++) {    
    SearchWord = SearchWords[i];
    Found = 0;
    if (!SearchWord.StrLength())
      continue;
    if (!case_sens.StrLength()) 
      SearchWord.UpCase();    

    while ((Pos = SearchString.isSubStr(SearchWord.asString(), Pos)) >= 0) {
      // whole words
      if (whole_words.StrLength())
        if (((Pos != 0) && (SearchString[Pos-1] > ' ')) ||
            ((Pos + SearchWord.StrLength() < SearchString.StrLength()) && (SearchString[Pos + SearchWord.StrLength()] > ' '))) {
          Pos++;
          continue;
        }
      // insert mark      
      SearchString[Pos] = '\1';
      for (register int i=1;i<(int) (SearchWord.StrLength()-1);i++)
        SearchString[i + Pos] = '\2';
      SearchString[Pos + SearchWord.StrLength() - 1] = '\1';
      Found++;
      Pos += SearchWord.StrLength();        
    }
    FinalResult += Found;
  }
  
  if (FinalResult)
    {
      search_string.SetSize(search_string.StrLength() + FinalResult * 7 + 1);
      register short Open = 0;
      for (register int i=SearchString.StrLength() - 1; i>=0; i--)
        if (SearchString[i] == '\1') {
          if (Open)
            search_string.StrInsert(i, "<b>");
          else
            search_string.StrInsert(i+1, "</b>");
          Open = !Open;
        }
    }
  return FinalResult;  
}


void article_manager::regexp_generate_recurse(cgiOutStream& CGIStream, const CString& RegExpExpression, ilist <search_results *> & search_res, ilist <article_tree *>& articles, const CVector<int>& Categories){
	/*
	  REGEXP
	*/  
	for (int i=0;i<articles.count();i++) {
		if ((!Categories.Count()) || (Categories.Contains(articles[i]->article->article_face()))) {
			int inside_result = 0;
			int result = regexp_search_text(CGIStream, RegExpExpression, articles[i]->article->article_title());
			result += regexp_search_text(CGIStream, RegExpExpression, articles[i]->article->author_name());	
			CString contents;
			if (!(articles[i]->article->article_moderated())) contents = articles[i]->article->get_inside(current_root, archive_category, bTar);
			if (contents.StrLength()) {
				if (!(params->get_entry("search_titles")).StrLength()) {
					inside_result = regexp_search_text(CGIStream, RegExpExpression, contents);
					result += inside_result;			
				}		
				if (result) {
					total_occurences+=result;
					search_results * element = new search_results;	
					element->occurences = result;
					if (inside_result) element->contents = contents; else element->contents.Free();
					element->article = articles[i]->article;
					search_res+=element;
				}
			}   
		}
		regexp_generate_recurse(CGIStream, RegExpExpression, search_res, articles[i]->replies, Categories);
	}
}

void article_manager::search_generate_recurse(cgiOutStream& CGIStream, CVector<CString>& search_strings, ilist <search_results *> & search_res, ilist <article_tree *> & articles, const CVector<int>& Categories){
	for (int i=0;i<articles.count();i++) {
		if ((!Categories.Count()) || (Categories.Contains(articles[i]->article->article_face()))) {
			int inside_result = 0;
			int result = search_text(CGIStream, articles[i]->article->article_title(), search_strings, params->get_entry("search_case_sens"), params->get_entry("search_whole_only"));		
			result += search_text(CGIStream, articles[i]->article->author_name(), search_strings, params->get_entry("search_case_sens"), params->get_entry("search_whole_only"));	
			CString contents;
			if (!(articles[i]->article->article_moderated())) contents = articles[i]->article->get_inside(current_root, archive_category, bTar);
			if (contents.StrLength()) {
				if (!(params->get_entry("search_titles")).StrLength()) {
					inside_result = search_text(CGIStream, contents, search_strings, params->get_entry("search_case_sens"), params->get_entry("search_whole_only"));
					result += inside_result;			
				}		
				if (result) {
					total_occurences+=result;
					search_results * element = new search_results;	
					element->occurences = result;
					if (inside_result) element->contents = contents; else element->contents.Free();
					element->article = articles[i]->article;
					search_res+=element;
				}
			}
		}
		search_generate_recurse(CGIStream, search_strings, search_res, articles[i]->replies, Categories);
	}		
}

void article_manager::regexp_generate_results(cgiOutStream& CGIStream){
	/*
	  REGEXP
	*/
	CString search_string = params->get_entry("search");
	search_string.StrTrim32();
	if (!(search_string.StrLength())) {
		//cgi_error(CGIStream, "nothing to search");
		CGIStream << ext_param::get_value("SEARCH NOTHING") << "\n";
		return;
	}

	CVector<int> Categories = get_search_categories();

	ilist <search_results *> search_res;	  
	ext_param::set_value("FLINK","0");
	total_occurences = 0;
	CGIStream << "[" << search_string << "]" << elf;
	CGIStream << hr_ << ext_param::get_option("SEARCH THREAD");
	regexp_generate_recurse(CGIStream, search_string, search_res, articles, Categories);
	output_and_clean(CGIStream, search_res, total_occurences); 
	if (!archives) archives = new article_archive_manager(archives_root, params, this);
	if (params->get_entry("search_archives").StrLength())
		for (int a=0;a<archives->archives.count();a++){
			total_occurences = 0;
			CGIStream << hr_ << ext_param::get_option("SEARCH IN ARCHIVE") << " [" << archives->archives[a]->clean_name << "] ";
			archives->archives[a]->fill_headers();
			current_root = archives->archives[a]->full_root;		
			regexp_generate_recurse(CGIStream, search_string, search_res, archives->archives[a]->articles, Categories);
			archive_category = archives->archives[a]->dirty_name();
			output_and_clean(CGIStream, search_res, total_occurences);
			archive_category.Free();
		}
}

CVector<int> article_manager::get_search_categories(void) {
	CVector<int> Result;
	for (int i=0;i<params->entries_count();i++) {
		if (params->get_name(i) == "SEARCH-CATEGORY") {
			Result += params->get_entry(i).Val();
		} 
	}
	return Result;
}

void article_manager::search_generate_results(cgiOutStream& CGIStream){
	CString search_string = params->get_entry("search");  
	search_string.StrTrim32();
	if (!(search_string.StrLength())) {
		//cgi_error(CGIStream, "nothing to search");
		CGIStream << ext_param::get_value("SEARCH NOTHING") << "\n";
		return;
	}
	CVector<CString> search_strings; search_string.Tokenizer(' ', search_strings);
	ilist <search_results *> search_res;	
	for (int k=0;k<search_strings.Count();k++) {
		CGIStream << "[" << search_strings[k] << "]";
		if (!(params->get_entry("search_case_sens").StrLength()))
			search_strings[k].UpCase();
	}

	CVector<int> Categories = get_search_categories();
  
	ext_param::set_value("FLINK","0");
	total_occurences = 0;
	CGIStream << hr_ << ext_param::get_option("SEARCH THREAD");
	search_generate_recurse(CGIStream, search_strings, search_res, articles, Categories);
	output_and_clean(CGIStream, search_res, total_occurences); 

	if (!archives) archives = new article_archive_manager(archives_root, params, this);
	if (params->get_entry("search_archives").StrLength())
		for (int a=0;a<archives->archives.count();a++){
			total_occurences = 0;
			CGIStream << hr_ << ext_param::get_option("SEARCH IN ARCHIVE") << " [" << archives->archives[a]->clean_name << "] ";
#ifdef ANSI
			if (bTar)
			  article_archive_manager :: detar_archive(CGIStream, archives_root, archives->archives[a]->dirty_name());
#endif
			archives->archives[a]->fill_headers();
			current_root = archives->archives[a]->full_root;		
			search_generate_recurse(CGIStream, search_strings, search_res, archives->archives[a]->articles, Categories);
			archive_category = archives->archives[a]->dirty_name();
			output_and_clean(CGIStream, search_res, total_occurences);
			archive_category.Free();
		}
}

void article_manager::br_output(cgiOutStream& CGIStream, const CString& i_str){
	for (int i = 0; i<i_str.StrLength();i++)
		if (i_str[i] == '\n') {CGIStream << elf;} else {CGIStream << (char) i_str[i];}
}

int search_res_compare(const void * source, const void * target){
	return(- (* (search_results **) source)->occurences + (* (search_results **) target)->occurences);
}

void article_manager::output_and_clean(cgiOutStream& CGIStream, ilist <search_results *> & search_res, int total_occurences){	
	if (!search_res.count()) {CGIStream << ext_param::get_option("SEARCH NOT FOUND") << !font_;}
	else {
		CGIStream << " - " << total_occurences << ext_param::get_option("SEARCH FOUND") << hr_ << "\n";		
		search_res.sort(&search_res_compare);
		for (int m=0;m<search_res.count();m++){
			CGIStream << font_.o("SIZE","2",0) << "(" << search_res[m]->occurences << ") " << !font_;		
			article_link(CGIStream, search_res[m]->article, 0);			
			CGIStream << elf;
			if (search_res[m]->contents.StrLength()){
				CGIStream << blockquote_ << font_.o("SIZE","2",0);				
				br_output(CGIStream, search_res[m]->contents);				
				CGIStream << !font_ << !blockquote_;
			}
		}
	}
	search_res.clear();
}

void article_manager::archive_merge(cgiOutStream& CGIStream){
	if (!archives) archives = new article_archive_manager(archives_root, params, this);
	cgi_access.check_access(CGIStream, params->get_entry("pass"), params->argv[1]);	
	archives->merge_archives(CGIStream, archives_root, params->get_entry("alias"), params, this, bTar);
	archives = new article_archive_manager(archives_root, params, this);
}

void article_manager::archive_pull(cgiOutStream& CGIStream){
	if (!archives) archives = new article_archive_manager(archives_root, params, this);
	cgi_access.check_access(CGIStream, params->get_entry("pass"), params->argv[1]);	
	archives->pull_archives(archives_root, params->get_entry("alias"), params, this);
}

void article_manager::post_article_preview(cgiOutStream& CGIStream, agnes_article * an_article){
	header_html = 0;
	state = A_PREVIEW;
	CString newEquiv(params->argv[2]); newEquiv+="Preview";
	current_article = an_article;
	current_tree = an_article->parent_tree;
	article_node = current_article->article_node();
	html_base<article_manager> agnes_forum_preview(CGIStream, equivs->get_value(CGIStream, newEquiv), this, params->argv, 0);
	common_exit(CGIStream);
}

