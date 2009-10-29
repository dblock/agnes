#include <agnespch.hpp>

extern CCookieManager * CurrentCookieManager;

CString article_manager::article_title(cgiOutStream& CGIStream, agnes_article * article){
	if (article->article_moderated() && (ext_param::get_option("MODERATED").Val()) && (state != A_EDIT_SELECTED)) {
		return ext_param::get_option("MODERATED-SUBJECT");    
	} else return article->article_title();
}

CString article_manager::author_name(cgiOutStream& CGIStream, agnes_article * article){
	if (article->article_moderated() && (ext_param::get_option("MODERATED").Val()) && (state != A_EDIT_SELECTED)) {
		return ext_param::get_option("MODERATED-NAME");    
	} else return article->author_name();
}

CString article_manager::author_mail(cgiOutStream& CGIStream, agnes_article * article){
	if (article->article_moderated() && (ext_param::get_option("MODERATED").Val()) && (state != A_EDIT_SELECTED)) {
		return ext_param::get_option("MODERATED-EMAIL");    
	} else return article->author_mail();
}

void article_manager::title_link(cgiOutStream& CGIStream, agnes_article * article, int cmpl, const CString& mess){
	if (article){
		if ((state == A_EDIT)||(state == A_ARCHIVE)) CGIStream << input_.o("TYPE","radio","NAME","victim","VALUE",article->article_node().asString(), 0) << " ";
		else if (state == A_DELETE) CGIStream << input_.o("TYPE","checkbox","NAME","victim","VALUE",article->article_node().asString(),0) << " ";
		if (cmpl) faces_list->show_face(CGIStream, article->article_face());
		CString call_local(getenv("SCRIPT_NAME")); call_local+="?"; call_local+=params->argv[1]; call_local+="+"; call_local+=params->argv[1]; call_local+="HTML";
		
		if ((!article->has_parent()) && AExpand.StrLength() && header_html){      
			CGIStream << a_.o("NAME",article->article_node().asString(), 0) << !a_;	
			if (!(article->node_starts(AExpand))) {
				int l_Show = (ext_param::get_option("MODERATED").Val())?
					((*(article->parent))[article->id]->article->has_unmoderated_replies()):
					((*(article->parent))[article->id]->replies.Count());
		
				//if ((*(article->parent))[article->id]->replies.Count()) {
				if (l_Show) {
					CString ExpandLocal(call_local);					
					int l_CurrentPage = get_option("PAGE").Val();
					if (l_CurrentPage) {
						ExpandLocal+="+page";
						ExpandLocal+=l_CurrentPage;
					}
					ExpandLocal+="+expand";
					ExpandLocal+=article->article_node();
					if (archive_category.StrLength()) { ExpandLocal+="+archive+"; ExpandLocal+=archive_category;  }
					ExpandLocal+="#"; ExpandLocal+=article->article_node();
					CGIStream << a_.o("HREF",ExpandLocal.asString(), "TARGET", ext_param::get_option("SOURCE").asString(), 0) 
						  << ext_param::get_option("EXPAND-THREAD") << !a_ << "\n";
				} else CGIStream << ext_param::get_option("NOEXPAND-THREAD") << "\n";
			} else CGIStream << ext_param::get_option("EXPANDED-THREAD") << "\n";      
		}
    
		call_local+="Article";
		if (archive_category.StrLength()) { call_local+="+archive+"; call_local+=archive_category;  }
		call_local+="+";
		call_local+=article->article_node();
		call_local+=params->c_params();
		CGIStream << a_.o("HREF",call_local.asString(),"TARGET", ext_param::get_option("TARGET").asString(),0);
		if (!cmpl) {CGIStream << mess;} else {CGIStream << ext_param::get_value("OPEN") << article_title(CGIStream, article) << ext_param::get_value("CLOSE");}
		CGIStream << !a_;  
	}
}

void article_manager::follow_link(cgiOutStream& CGIStream, agnes_article * article, const CString& mess){
	if (article && (!article->article_moderated())){
		CString call_local(getenv("SCRIPT_NAME")); 
		call_local+="?"; 
		call_local+=params->argv[1];
		call_local+="+"; 
		call_local+=params->argv[1];
		call_local+="HTMLFollowUp+";
		call_local+=article->article_node(); 
		call_local+=params->c_params();
		CGIStream << a_.o("HREF", call_local.asString(), "TARGET", ext_param::get_option("TARGET").asString(), 0) << mess << !a_;
	} else CGIStream << mess;
}

void article_manager::article_link(cgiOutStream& CGIStream, agnes_article * article, int decal) {	
	if (!article) return;
	
	
	/*
	  if (decal) for (int j = 1; j < article->article_levels.Count() - globDecal; j++) 
	  CGIStream << img_.o("SRC",ext_param::get_option("BLANK").asString(),"WIDTH","25", "ALT", " ", 0);
	*/
	if (decal) {
		int Quantity = article->article_levels.Count() - globDecal - 1;
		if (Quantity > 0) {
			CString Tmp; Tmp+=Quantity*25;
			CGIStream << img_.o("SRC",ext_param::get_option("BLANK").asString(),"HEIGHT","10","WIDTH",Tmp.asString(), "ALT", " ", 0);
		}
	}
	
	
	if (article->article_moderated()) CGIStream << ext_param::get_option("MODERATED-TEXT");
	
	if (
	    (!old_hours||recent_structure->how_old_hours(article->article_date()) <= old_hours) &&
	    (!old_minutes||recent_structure->how_old_minutes(article->article_date()) <= old_minutes) &&
	    (!old_seconds||recent_structure->how_old_seconds(article->article_date()) <= old_seconds) &&
	    (!old_limit||recent_structure->how_old(article->article_date()) < old_limit)
	    ){
		title_link(CGIStream, article, 1, 0);
		if (!hideEMAIL || !hideNAME || !hideDATE) CGIStream << font_.o("size","-1",0) << " - ";
		recent_structure->article_recent(CGIStream, article->article_date());
		if (ext_param::get_option("IP").Val()) CGIStream << "[" << article->article_ip() << "/" << article->article_host() << "]";
		if (author_mail(CGIStream, article).StrLength()) {	  
			if (!hideEMAIL) {
				CString e_mail("mailto:");e_mail+=author_mail(CGIStream, article);
				CGIStream << a_.o("HREF", e_mail.asString(), 0);
				if (!hideNAME) CGIStream << ext_param::get_option("OPEN-NAME") << author_name(CGIStream, article) << ext_param::get_option("CLOSE-NAME");
				else CGIStream << ext_param::get_option("OPEN-NAME") << author_mail(CGIStream, article) << ext_param::get_option("CLOSE-NAME");
				CGIStream << !a_;
			} else CGIStream << ext_param::get_option("OPEN-NAME") << author_name(CGIStream, article) << ext_param::get_option("CLOSE-NAME");	
		} else if (!hideNAME) {
			CGIStream << " " << ext_param::get_option("OPEN-NAME") << author_name(CGIStream, article) << ext_param::get_option("CLOSE-NAME") << " ";
		}
		if (!hideDATE) CGIStream << ext_param::get_option("OPEN-DATE") << " (" << shift_date(article->article_date()) << ")" << ext_param::get_option("CLOSE-DATE") << !font_;
		if (ext_param::get_option("FLINK").Val()) follow_link(CGIStream, article, ext_param::get_option("FOLLOW"));
	} else {
		CGIStream << font_.o("size","-2",0) << article_title(CGIStream, article);
		if (!hideEMAIL || !hideNAME || !hideDATE) CGIStream << " - ";
		if (!hideNAME) CGIStream << author_name(CGIStream, article);
		else if (!hideEMAIL) CGIStream << author_mail(CGIStream, article);
		if (!hideDATE) CGIStream << " (" << shift_date(article->article_date()) << ")";
		CGIStream << !font_ ;
	}
}

void article_manager::article_link(cgiOutStream& CGIStream, agnes_article * article){
	if (article){
		CGIStream << tr_ << td_;
		article_link(CGIStream, article, 1);
		CGIStream << !td_ << !tr_ << "\n";
		CGIStream.flush();
	}
}

void article_manager::article_collapse(cgiOutStream& CGIStream, agnes_article * article, article_tree * tree, int level){
	CString call_local(getenv("SCRIPT_NAME")); call_local+="?"; call_local+=params->argv[1]; call_local+="+"; call_local+=params->argv[1]; call_local+="HTML";
	if (archive_category.StrLength()) { call_local+="+archive+"; call_local+=archive_category;  }      
	call_local+="+collapse"; CString c1(call_local); CString c2(call_local); CString c3(call_local); 
	c2+=(level+1);      
	c1+=(level-1);
	c3+="0";
  
	if ((ext_param::get_option("COLLAPSE-LESS").StrLength()) && (level > 1)) {
		CGIStream << a_.o("HREF", c1.asString(), "TARGET", ext_param::get_option("SOURCE").asString(), 0)
			  << ext_param::get_option("COLLAPSE-LESS")
			  << !a_;
	}
	if (ext_param::get_option("COLLAPSE-ALL").StrLength()) {	
		CGIStream << a_.o("HREF", c3.asString(), "TARGET", ext_param::get_option("SOURCE").asString(), 0)
			  << ext_param::get_option("COLLAPSE-ALL")
			  << !a_;
	}	
	if (ext_param::get_option("COLLAPSE-MORE").StrLength() && ((*(article->parent))[article->id]->replies.Count())) {
		CGIStream << a_.o("HREF", c2.asString(), "TARGET", ext_param::get_option("SOURCE").asString(), 0)
			  << ext_param::get_option("COLLAPSE-MORE")
			  << !a_;
	}
	output_recent(CGIStream, tree);
}

void article_manager::article_link(cgiOutStream& CGIStream, agnes_article * article, article_tree * tree, int level){
	if (article){
		CGIStream << tr_ << td_;
		article_link(CGIStream, article, 1);        
		if (AExpand.StrLength()) {
			if (!(article->node_starts(AExpand))) article_collapse(CGIStream, article, tree, level);
		} else {
			if ((tree->replies.count() && collapse && level && (level >= collapse))) article_collapse(CGIStream, article, tree, level);
		}   
	}
	CGIStream << !td_ << !tr_ << "\n";
	CGIStream.flush();
}

void article_manager::output_recent(cgiOutStream& CGIStream, article_tree * tree) {
	for (int m = 0; m<tree->replies.count();m++) {
		recent_structure->article_recent(CGIStream, ((tree->replies[m])->article)->article_date());
		output_recent(CGIStream, tree->replies[m]);
	}
}

void article_manager::output_articles(cgiOutStream& CGIStream, article_tree * tree, int tabled, int level){  
	if (tree)
		if ((!(tree->article->article_moderated()))||(state!=A_NORMAL)||(ext_param::get_option("MODERATED-SHOW").Val())){
			if (tabled) CGIStream << table_.o("WIDTH",ext_param::get_option("TWIDTH").asString(),0) << "\n";
			article_link(CGIStream, tree->article, tree, level);
    
			if ((!(collapse && level && (level >= collapse))) || (AExpand.StrLength()))
				if ((!AExpand.StrLength()) || ((tree->article)->node_starts(AExpand))) {     
					if (ext_param::get_option("REVERSE").Val())
						for (int m = tree->replies.count(); m; m--) output_articles(CGIStream, tree->replies[m-1], 0, level+1);
					else for (int m = 0; m<tree->replies.count();m++) output_articles(CGIStream, tree->replies[m], 0, level+1);
				}
			if (tabled) CGIStream << !table_ << "\n";    
		}
}

void article_manager::output_articles(cgiOutStream& CGIStream){  
	CString call_local(getenv("SCRIPT_NAME")); call_local+="?"; call_local+=params->argv[1]; call_local+="+"; call_local+=params->argv[1]; call_local+="HTML";  call_local+=params->c_params();
	if ((state == A_ARCHIVE)||(state == A_EDIT)||(state == A_DELETE)) {
		CGIStream << form_.o("METHOD","post","ACTION",call_local.asString(),0) << "\n" << table_;    
		if (state == A_EDIT) CGIStream << tr_ << td_.o("COLSPAN","2", 0) << "(edit an article)" << input_.o("TYPE","hidden","NAME","action","VALUE","edit",0) << "\n" << !td_ << !tr_ ;
		else if (state == A_ARCHIVE) CGIStream << tr_ << td_.o("COLSPAN","2", 0) << "(select the last node to archive - articles are in ascending order)" << input_.o("TYPE","hidden","NAME","action","VALUE","archive",0) << !td_ << !tr_ << "\n";
		else if (state == A_DELETE) CGIStream << tr_ << td_.o("COLSPAN","2", 0) << "(delete article(s))" << input_.o("TYPE","hidden","NAME","action","VALUE","delete",0) << !td_ << !tr_ << "\n";	
		CGIStream << tr_ << td_.o("COLSPAN","2", 0) << input_.o("TYPE","submit","NAME","post","VALUE","proceed",0) << " \n"
			  << input_.o("TYPE","reset","NAME","reset","VALUE","recycle",0) << "\n" << !td_ << !tr_;
		if (state == A_ARCHIVE)	CGIStream << tr_ << td_ << "alias: " << !td_ << td_ << input_.o("TYPE","text","NAME","alias",0) << !tr_ << !td_ << "\n";
		CGIStream << tr_ << td_ << "password: " << !td_ << td_ << input_.o("TYPE","password","NAME","pass","VALUE", params->get_entry("pass").asString(), 0) << "\n" << !tr_ << !td_;	
		CGIStream << !table_;
	}

	/*
	  get the page parameters
	*/
	int l_CurrentPage = get_option("PAGE").Val();
	int l_PageCount = get_option("ARTICLES-PAGE").Val();  
	/*
	  adjust article bounds
	*/
	int Min, Max;
	if ((l_CurrentPage > 0)&&(l_PageCount > 0)) {
		if (ext_param::get_option("REVERSE").Val()) {    
			Max = articles.count() - (l_PageCount*(l_CurrentPage-1));
			Min = articles.count() - (l_PageCount*l_CurrentPage) + 1;      
		} else {
			Min = (l_PageCount*(l_CurrentPage-1));
			Max = (l_PageCount*l_CurrentPage)-1;
		}
		if (Max >= articles.count()) Max = articles.count() - 1;
		if (Min <= 0) Min = 0;
	} else {
		Max = articles.count() - 1;
		Min = 0;
	}

	/*
	  show articles
	*/
	if (ext_param::get_option("REVERSE").Val()) {    
		for (int i = Max; i >= Min; i--) {
			if (i!=Max) CGIStream << ext_param::get_option("GROUPBREAK");
			output_articles(CGIStream, articles[i], 1, 1);
		}
	} else {   
		for (int i = Min; i <= Max; i++) {
			if (i!=Min) CGIStream << ext_param::get_option("GROUPBREAK");
			output_articles(CGIStream, articles[i], 1, 1);
		}
	}
 
	/*
	  if (state == A_DELETE) {
	  CGIStream << br_ << "(delete archive(s)):" << br_;
	  generate_push(CGIStream);
	  }
	*/

	if ((state == A_ARCHIVE)||(state == A_EDIT)||(state == A_DELETE)) CGIStream << !form_;
}

void article_manager::read_faces(void){
	faces_list = new faces_manager(ext_param::get_option("EXTFACES"));
}

void article_manager::generate_home(cgiOutStream& CGIStream){  
	CString call_local(getenv("SCRIPT_NAME")); call_local+="?"; call_local+=params->argv[1]; call_local+="+"; call_local+=params->argv[1]; call_local+="HTML";
	if (current_article) {
		call_local += params->c_params();
	} else {
	    call_local+=reset_page_one(params->c_params());  
	}
	CGIStream << a_.o("HREF", call_local.asString(), "TARGET", ext_param::get_option("SOURCE").asString(), 0);
	CGIStream << ext_param::get_option("HOME") << !a_;
}

void article_manager::new_link(cgiOutStream& CGIStream){
	CString call_local(getenv("SCRIPT_NAME")); call_local+="?"; call_local+=params->argv[1]; call_local+="+"; call_local+=params->argv[1]; call_local+="HTMLPostNew";  call_local+=params->c_params();
	CGIStream << a_.o("HREF", call_local.asString(), "TARGET", ext_param::get_option("TARGET").asString(), 0);
	CGIStream << ext_param::get_option("NEW") << !a_;  
}

void article_manager::output_current_thread(cgiOutStream& CGIStream){
	if (current_article) {
		int collapse_old = collapse;    
		CString AExpandOld(AExpand);
		AExpand.Free();
		collapse = 0;
		CString oldFlink = ext_param::get_value("FLINK");
		ext_param::set_value("FLINK","0");
		globDecal = current_article->article_levels.Count() - 1;
		output_articles(CGIStream, get_tree(CGIStream, current_article->article_levels), 1, 0);
		globDecal = 0;
		ext_param::set_value("FLINK",oldFlink);
		collapse = collapse_old;
		AExpand = AExpandOld;
	}
}

CString article_manager::check_archive(const CString& root){
	return check_dir(root, "archives");
}

CString article_manager::check_contents(const CString& root){
	return check_dir(root, "contents");
}

CString article_manager::check_dir(const CString& root, const CString& dir){
	CString target(root);
	if (_chdir(target.asString()) == -1){
		make_directory(target);
	}	
	target+=dir;
	target.append_backslash();
	if (_chdir(target.asString()) == -1) {
		make_directory(target);
	}
	target.append_backslash();
	return target;
}

article_tree * article_manager::get_tree(cgiOutStream& CGIStream, article_tree * tree, const CVector<int>& zlist){
	for (int i=0;i<tree->replies.count();i++){
		if (!((tree->replies[i])->article->private_compare(zlist))) return tree->replies[i];
		else if ((tree->replies[i]->article)->has_node(zlist))
			return get_tree(CGIStream, tree->replies[i], zlist);
	}
	return 0;
}

article_tree * article_manager::get_tree(cgiOutStream& CGIStream, const CString& node){  
	if (!(node.StrLength())) {CGIStream << "attempt to retrieve a tree from void node" << elf; return 0;}
	CVector<int> zlist; agnes_article::mlevel_generator(node, zlist);
	return get_tree(CGIStream, zlist);
}

article_tree * article_manager::get_tree(cgiOutStream& CGIStream, const CVector<int>& zlist){
	for (int i=0;i<articles.count();i++){
		if (!((articles[i]->article)->private_compare(zlist))) return articles[i];
		else if ((articles[i]->article)->has_node(zlist)) return get_tree(CGIStream, articles[i], zlist);
	}
	return 0;
}

void article_manager::generate_followups_table(cgiOutStream& CGIStream, article_tree * current_tree, int Inside) {
  int cmpl = ext_param::get_option("COMPLETE").Val();
  if (Inside) {
	if (cmpl == 0)
	  CGIStream << tr_ << td_.o("ALIGN","right",0) << ext_param::get_option("FMSG") << !td_ << td_ << "\n";
	else if (cmpl != -1) 
	  CGIStream << tr_ << td_.o("ALIGN","right","VALIGN","top",0) << ext_param::get_option("FMSG") << !td_ << td_ << "\n";  
	if (cmpl!=-1) {
	  generate_followups(CGIStream, current_tree, cmpl, (cmpl == 2)); 
	  CGIStream << !td_ << !tr_;
	}
  } else {
	CGIStream << table_;
	generate_followups(CGIStream, current_tree, cmpl, 1);
	CGIStream << !table_;
  }
}

void article_manager::generate_contents(cgiOutStream& CGIStream){
	if (!current_article) { CGIStream << "unable to retreive article structure [" << article_node << "]" << elf; return;}
	CGIStream << table_.o("BORDER","0",0) << "\n";
	CGIStream << tr_ << td_.o("ALIGN","right",0) << ext_param::get_option("SUBJECT") << !td_ << td_ << article_title(CGIStream, current_article) << !td_ << !tr_;
	CGIStream << tr_ << td_.o("ALIGN","right",0) << ext_param::get_option("NAME") << !td_ << td_;
	if (author_mail(CGIStream, current_article).StrLength()) {    
		if (!hideEMAIL) {  
			CString e_mail("mailto:"); e_mail+=author_mail(CGIStream, current_article);
			CGIStream << a_.o("HREF", e_mail.asString(), 0) << author_name(CGIStream, current_article) << !a_;
		} else CGIStream << author_name(CGIStream, current_article);    
	} else CGIStream << author_name(CGIStream, current_article);
	CGIStream << !td_ << !tr_ << "\n";
	CGIStream << tr_ << td_.o("ALIGN","right",0) << ext_param::get_option("DATE") << !td_ << td_ << shift_date(current_article->article_date()) << !tr_ << "\n";
	article_parent_row(CGIStream);

	if (state!=A_PREVIEW) generate_followups_table(CGIStream, current_tree, 1);

	CGIStream << tr_ << td_ << !td_ << td_;
	CGIStream << ext_param::get_value("SEPARATOR") << "\n";
	generate_inside(CGIStream, current_article, 0);
	CGIStream << !td_ << !tr_;
	CGIStream << !table_ << "\n";
  
	if (state == A_PREVIEW) {
		CString call_local(getenv("SCRIPT_NAME")); call_local+="?"; call_local+=params->argv[1]; call_local+="+"; call_local+=params->argv[1]; call_local+="HTML"; call_local+=params->c_params();
		CGIStream << form_.o("ACTION", call_local.asString(), "METHOD", "POST", "TARGET", ext_param::get_option("SOURCE").asString(), 0);
		for (int i=0;i<params->entries_count();i++) {
			if ((params->get_name(i))!="preview") 
				CGIStream << input_.o("TYPE","hidden","NAME",params->get_name(i).asString(), "VALUE",params->get_entry(i).escape_url().asString(), 0) << "\n";
			else CGIStream << input_.o("TYPE","submit","NAME","send","VALUE", ext_param::get_value("POST").asString(), 0);
		}
		CGIStream << !form_;
	}
}

void article_manager::article_parent_row(cgiOutStream& CGIStream){
	if (ext_param::get_option("PARENT-ROW").Val()==1) {
		article_tree * parent_tree = 0;
		CVector<int> Id = current_article->article_levels;
		Id.DeleteElt(Id.Count()-1);
		if (Id.Count()) parent_tree = get_tree(CGIStream, Id);
		if (parent_tree) {
			CString oldFlink = ext_param::get_value("FLINK");
			ext_param::set_value("FLINK","0");
			CGIStream << tr_ << td_.o("ALIGN","right",0) << ext_param::get_option("POSTBY") << !td_ << td_;
			article_link(CGIStream, parent_tree->article, 0);
			CGIStream << !td_ << !tr_ << "\n";
			ext_param::set_value("FLINK",oldFlink);
		}
	}
}

void article_manager::generate_followups(cgiOutStream& CGIStream, article_tree * current_tree, int cmpl, int full){
	int l_ShowModerated = ext_param::get_option("MODERATED-SHOW").Val();
	for (int i=0;i<current_tree->replies.count();i++){
		if ((!(current_tree->replies[i]->article->article_moderated()) || l_ShowModerated)) {
			if (i) if (cmpl) CGIStream << br_; else CGIStream << " ";
			CString iVal; iVal+=i+1;
			if (!full) title_link(CGIStream, current_tree->replies[i]->article, cmpl, iVal);
			else article_link(CGIStream, current_tree->replies[i]->article, 0);
		}
	}
}

void article_manager::generate_inside(cgiOutStream& CGIStream, agnes_article * article, int type){
	CString output;

	CString Moderated = ext_param::get_option("MODERATED");
	if ((article->article_moderated()) && 
	    (state != A_EDIT_SELECTED) && 
	    (Moderated.Val())
	    ){
		output = "article is moderated, an administrator will release it as soon as possible";
	} else {
		if (archive_category.StrLength()) output = article->get_inside(archives_root, archive_category, bTar);
		else output = article->get_inside(contents_root, 0, bTar);
	}  

	CGIStream << ext_param::get_option("OPEN-ARTICLE");
	if (!(output.StrLength())) {
	  CGIStream << "unable to retrieve article contents (" << article->article_node() << ")" << "\n"; 
	  return;
	}
	int forward = 0;
	for (int i = 0; i<output.StrLength(); i++){
		if ((output[i]!='>')&&(type == 0)) forward = 1; else forward = 0;
		if (forward) CGIStream << ext_param::get_value("ABEG");
		if ((type == 1)&&(state!=A_EDIT_SELECTED)) CGIStream << "&gt;";
		while ((output[i]!=(char) 10)&&(i<output.StrLength())) { CGIStream << output[i]; i++; }
		if (forward) CGIStream << ext_param::get_value("AEND");
		if (type == 0) CGIStream << br_ << "\n";		
	}
	CGIStream << ext_param::get_option("CLOSE-ARTICLE");
}

void article_manager::generate_parent(cgiOutStream& CGIStream, int FullParent){
	if (current_article){
		article_tree * parent_tree = 0;
		CVector<int> Id = current_article->article_levels;
		Id.DeleteElt(Id.Count()-1);
		if (Id.Count()) parent_tree = get_tree(CGIStream, Id);
		if (parent_tree) {
			if (FullParent) article_link(CGIStream, parent_tree->article, 0);
                        else title_link(CGIStream, parent_tree->article, 0, ext_param::get_value("PARENT"));
		} else CGIStream << ext_param::get_value("NOPARENT");
	}
}

void article_manager::generate_next(cgiOutStream& CGIStream){
	if (current_article){
		agnes_article * next = current_article->get_next();
		if (next) title_link(CGIStream, next, 0, ext_param::get_value("NEXT"));
		else CGIStream << ext_param::get_value("NONEXT");
	}
}

void article_manager::generate_prev(cgiOutStream& CGIStream){
	if (current_article){
		agnes_article * prev = current_article->get_prev();
		if (prev) title_link(CGIStream, prev, 0, ext_param::get_value("PREV"));
		else CGIStream << ext_param::get_value("NOPREV");
	}
}

void article_manager::post_common(cgiOutStream& CGIStream){
	CString mod_value;
	faces_list->show_table(CGIStream, current_article, ext_param::get_value("EXTFACES-COLS").Val()); 
	if ((current_article)&&(current_article->follow_mail())) mod_value = "CHECKED"; else mod_value.Free();

	if (ext_param::get_value("BYMAIL").StrLength()) {
		CGIStream << tr_ << td_.o("ALIGN","right",0) << "&nbsp;" << !td_ << td_ << p_ 
			  << input_.o("TYPE","checkbox","NAME","bymail", "VALUE","on", "", mod_value.asString(), 0) << " " << ext_param::get_value("BYMAIL") << !p_ << !td_ << !tr_ << "\n";
	}
 
	if ((state == A_EDIT_SELECTED)&&(ext_param::get_option("MODERATED").Val())){
		if ((current_article)&&(current_article->article_moderated())) mod_value = "CHECKED"; else mod_value.Free();
		CGIStream << tr_ << td_.o("ALIGN","right",0) << "&nbsp;" << !td_ << td_ << p_
			  << input_.o("TYPE","checkbox","NAME","moderate", "VALUE", "on", "", mod_value.asString(), 0) << " " << ext_param::get_value("MODERATED-CHECK")
			  << !p_ << !td_ << !tr_ << "\n";
	}
     
	CGIStream << tr_ << td_.o("ALIGN","right",0) << "&nbsp;" << !td_ << td_ << p_
		  << input_.o("TYPE","submit","NAME","send","VALUE", ext_param::get_value("POST").asString(), 0);
	if (state!=A_PREVIEW) {
                CString PreviewButtonText = ext_param::get_value("PREVIEW");
                if (PreviewButtonText.StrLength()) {
                  CString newEquiv(params->argv[1]); newEquiv+="HTMLPreview";
                  if (equivs->get_value(CGIStream, newEquiv, 0).StrLength())
                    CGIStream << input_.o("TYPE","submit","NAME","preview","VALUE",PreviewButtonText.asString(), 0);
                }
	}
	CGIStream << input_.o("TYPE","reset","NAME","clear","VALUE", ext_param::get_value("CLEAR").asString(), 0)
              << !td_ << !tr_ << "\n";
}

void article_manager::post_name_row(cgiOutStream& CGIStream) {	
	if (user_manager && (user_manager->GetUsermode())) {
		CGIStream << tr_ << td_.o("ALIGN","right",0) << ext_param::get_value("NAME") << !td_
			  << td_ << input_.o("TYPE","hidden","NAME","name","VALUE",user_manager->GetUsername().asString(),0)
			  << ext_param::get_option("OPEN-NAME")
			  << user_manager->GetUsername()
			  << ext_param::get_option("CLOSE-NAME")
			  << !td_ << !tr_ << "\n";
	} else {
		CGIStream << tr_ << td_.o("ALIGN","right",0) << ext_param::get_value("NAME") << !td_
			  << td_ << input_.o("TYPE","text","SIZE",ext_param::get_value("SIZE").asString(),"NAME","name","VALUE",CurrentCookieManager->Get("NAME").asString(),0) 
			  << !td_ << !tr_ << "\n";
	}
}

void article_manager::post_new(cgiOutStream& CGIStream){
	CString call_local(getenv("SCRIPT_NAME")); call_local+="?"; call_local+=params->argv[1]; call_local+="+"; call_local+=params->argv[1]; call_local+="HTML";  call_local+=params->c_params();
	CGIStream << form_.o("ACTION", call_local.asString(), "METHOD", "POST", "TARGET", ext_param::get_option("SOURCE").asString(), 0);
	CGIStream.flush();
	CGIStream << input_.o("TYPE","hidden","NAME","bbs-alias","VALUE",params->argv[1].asString(),0) << "\n";
	CGIStream << input_.o("TYPE","hidden","NAME","action","VALUE","new article",0) << table_.o("BORDER","0",0) << "\n";
	CGIStream << tr_ << td_.o("ALIGN","right",0) << ext_param::get_value("SUBJECT") << !td_ << td_
		  << input_.o("TYPE","text","SIZE",ext_param::get_value("SIZE").asString(),"NAME","subject",0) << !td_ << !tr_ << "\n";

	post_name_row(CGIStream);

	CGIStream << tr_ << td_.o("ALIGN","right",0)<< ext_param::get_value("MAIL") << !td_
		  << td_ << input_.o("TYPE","text","SIZE",ext_param::get_value("SIZE").asString(),"NAME","mail","VALUE",CurrentCookieManager->Get("MAIL").asString(),0) << !td_ << !tr_ << "\n";
	CGIStream << tr_ << td_.o("ALIGN","right",0)<<"&nbsp;" << !td_
		  << td_ << textarea_.o("NAME","article","WRAP","virtual","ROWS",ext_param::get_value("ROWS").asString(),"COLS",ext_param::get_value("COLS").asString(),0);
	CGIStream << !textarea_ << !td_ << !tr_ << "\n";
	post_common(CGIStream);
	CGIStream << !table_ << !form_;
}

void article_manager::post_follow(cgiOutStream& CGIStream){
	CString new_subject;
	CString new_name;
	CString new_mail;  
	if (state != A_EDIT_SELECTED) {
		if (current_article->article_moderated()) cgi_error(CGIStream, "cannot reply to a moderated article");   
		if (!(current_article->article_title().StartsWith(ext_param::get_option("RE-TAG")))) new_subject = ext_param::get_option("RE-TAG"); else new_subject.Free();
		new_subject += current_article->article_title();
	} else new_subject = current_article->article_title();

	CString call_local(getenv("SCRIPT_NAME")); call_local+="?"; call_local+=params->argv[1]; call_local+="+"; call_local+=params->argv[1]; call_local+="HTML+"; call_local+=current_article->article_node(); call_local+=params->c_params();
	CGIStream << form_.o("ACTION", call_local.asString(), "METHOD", "POST", "TARGET", ext_param::get_option("SOURCE").asString(), 0);

	if (state == A_EDIT_SELECTED){
		CGIStream << "editing article node [" << current_article->article_node() << "]" << elf;
		CGIStream << br_ << "password: " << input_.o("TYPE","password","NAME","pass","VALUE",params->get_entry("pass").asString(), 0) << "\n" << hr_;
		CGIStream << input_.o("TYPE","hidden","NAME","action","VALUE","edit post",0);
		ext_param::set_value("POSTBY","");
		new_mail = author_mail(CGIStream, current_article);
		new_name = author_name(CGIStream, current_article);
	} else {
		new_name = CurrentCookieManager->Get("NAME").asString();
		new_mail = CurrentCookieManager->Get("MAIL").asString();
		CGIStream << input_.o("TYPE","hidden","NAME","action","VALUE","followup",0);
	}

	CGIStream << input_.o("TYPE","hidden","NAME","bbs-alias","VALUE",params->argv[1].asString(),0) << "\n";
	CGIStream << input_.o("TYPE","hidden","NAME","parent","VALUE",current_article->article_node().asString(),0);
	CGIStream << table_.o("BORDER","0",0);

	if (ext_param::get_option("PARENT-POST").Val()) {
		CGIStream << tr_ << td_.o("ALIGN","right",0);
		CGIStream << ext_param::get_option("POSTBY") << !td_ << "\n" << td_;
		ext_param::set_value("FLINK","0");
		article_link(CGIStream, current_article, 0);
		CGIStream << !td_ << !tr_;
	}
	CGIStream << tr_ << td_.o("ALIGN","right",0) << ext_param::get_value("SUBJECT") << !td_ << td_
		  << input_.o("TYPE","text","SIZE",ext_param::get_value("SIZE").asString(),"NAME","subject","VALUE", new_subject.asString(), 0) << !td_ << !tr_ << "\n";
	/*
	  CGIStream << tr_ << td_.o("ALIGN","right",0) << ext_param::get_value("NAME") << !td_
	  << td_ << input_.o("TYPE","text","SIZE",ext_param::get_value("SIZE").asString(),"NAME","name","VALUE",new_name.asString(), 0) << !td_ << !tr_ << "\n";
	*/
	if (state == A_EDIT_SELECTED) {		
		CGIStream << tr_ << td_.o("ALIGN","right",0) << ext_param::get_value("NAME") << !td_
			  << td_ << input_.o("TYPE","text","SIZE",ext_param::get_value("SIZE").asString(),"NAME","name","VALUE",new_name.asString(),0)
			  << !td_ << !tr_ << "\n";
	} else post_name_row(CGIStream);

	CGIStream << tr_ << td_.o("ALIGN","right",0)<< ext_param::get_value("MAIL") << !td_
		  << td_ << input_.o("TYPE","text","SIZE",ext_param::get_value("SIZE").asString(),"NAME","mail","VALUE", new_mail.asString(), 0) << !td_ << !tr_ << "\n";
	CGIStream << tr_ << td_.o("ALIGN","right",0)<<"&nbsp;" << !td_
		  << td_ << textarea_.o("NAME","article","WRAP","virtual","ROWS",ext_param::get_value("ROWS").asString(),"COLS",ext_param::get_value("COLS").asString(),0);
	if ((state == A_EDIT_SELECTED) || (!ext_param::get_value("NOQUOTE-REPLY").Val())) generate_inside(CGIStream, current_article, 1);
	CGIStream << "\n" << !textarea_ << !td_ << !tr_ << "\n";
	post_common(CGIStream);
	CGIStream << !table_ << !form_;
}

void article_manager::generate_categories_search_row(cgiOutStream& CGIStream) {
	if (ext_param::get_value("SEARCH CATEGORIZE").Val()) {
		if ((faces_list) && (faces_list->Count())) {
			CGIStream << table_ 
				  << tr_ << td_.o("ALIGN","right",0) << ext_param::get_value("SEARCH CATEGORY") << "\n" << !td_      
				  << td_.o("ALIGN","left",0)
				  << select_.o("","MULTIPLE","NAME","SEARCH-CATEGORY","SIZE","5",0);
			//CGIStream << option_.o("VALUE","","","SELECTED",0) << ext_param::get_value("SEARCH CATEGORY ALL") << !option_ << "\n";
			for (int i=0;i<faces_list->Count();i++) {
				CGIStream << option_.o("VALUE",int_to_str(i).asString(),0) << faces_list->comment(i) << !option_ << "\n";
			}
			CGIStream << !select_
				  << !td_ << !tr_ << !table_ << "\n";
		} 
	}
}

void article_manager::generate_regexp_search(cgiOutStream& CGIStream){
	CString call_local(getenv("SCRIPT_NAME")); call_local+="?"; call_local+=params->argv[1]; call_local+="+"; call_local+=params->argv[1]; call_local+="HTMLSearch";  call_local+=params->c_params();
	CGIStream << form_.o("ACTION", call_local.asString(), "METHOD", "POST", "TARGET", ext_param::get_option("TARGET").asString(), 0)
		  << input_.o("TYPE","hidden","NAME","action","VALUE","regexp-search",0) << "\n"
		  << table_ << tr_ << td_.o("ALIGN","center",0) << ext_param::get_value("SEARCH") << "\n"
		  << input_.o("TYPE","text","SIZE",ext_param::get_value("SEARCH SIZE").asString(),"NAME","search",0) << !td_ << "\n"
		  << td_ << input_.o("TYPE","submit","NAME","submit_search","VALUE",ext_param::get_value("SEARCH COMMAND").asString(), 0) << "\n";
	if (ext_param::get_value("SEARCH CLEAR").StrLength()) CGIStream << input_.o("TYPE","reset","NAME","reset_search","VALUE",ext_param::get_value("SEARCH CLEAR").asString(), 0) << "\n";
	CGIStream << !td_ << !tr_ << !table_ << "\n";
	generate_categories_search_row(CGIStream); 
	CGIStream << table_ << "\n"
		  << td_.o("ALIGN","right","WIDTH","50%",0) 
		  << ext_param::get_value("SEARCH TITLES") << " " << input_.o("TYPE","checkbox","NAME","search_titles",0) << "\n"
		  << !td_ << td_.o("ALIGN","left","WIDTH","50%",0)
		  << ext_param::get_value("SEARCH ARCHIVES") << " " << input_.o("TYPE","checkbox","NAME","search_archives",0) << "\n"
		  << !td_ << !tr_ << !table_ << !form_ << "\n";
}


void article_manager::generate_search(cgiOutStream& CGIStream){
	CString call_local(getenv("SCRIPT_NAME")); call_local+="?"; call_local+=params->argv[1]; call_local+="+"; call_local+=params->argv[1]; call_local+="HTMLSearch";  call_local+=params->c_params();
	CGIStream << form_.o("ACTION", call_local.asString(), "METHOD", "POST", "TARGET", ext_param::get_option("TARGET").asString(), 0)
		  << input_.o("TYPE","hidden","NAME","action","VALUE","search",0) << "\n"
		  << table_ << tr_ << td_.o("ALIGN","center",0) << ext_param::get_value("SEARCH") << "\n"
		  << input_.o("TYPE","text","SIZE",ext_param::get_value("SEARCH SIZE").asString(),"NAME","search",0) << !td_ << "\n"
		  << td_ << input_.o("TYPE","submit","NAME","submit_search","VALUE",ext_param::get_value("SEARCH COMMAND").asString(), 0) << "\n";
	if (ext_param::get_value("SEARCH CLEAR").StrLength()) CGIStream << input_.o("TYPE","reset","NAME","reset_search","VALUE",ext_param::get_value("SEARCH CLEAR").asString(), 0) << "\n";
	CGIStream << !td_ << !tr_ << !table_ << "\n";
	generate_categories_search_row(CGIStream); 
	CGIStream << table_ << "\n"
		  << td_.o("ALIGN","right","WIDTH","50%",0) << ext_param::get_value("SEARCH TITLES") << " "
		  << input_.o("TYPE","checkbox","NAME","search_titles",0) << br_ << "\n"
		  << ext_param::get_value("SEARCH ARCHIVES") << " " << input_.o("TYPE","checkbox","NAME","search_archives",0)
		  << !td_ << td_.o("ALIGN","left","WIDTH","50%",0) << input_.o("TYPE","checkbox","NAME","search_whole_only",0)
		  << " " << ext_param::get_value("SEARCH WHOLE") << br_ << "\n"
		  << input_.o("TYPE","checkbox","NAME","search_case_sens",0) << " " <<  ext_param::get_value("SEARCH CASESENS") << "\n"
		  << !td_ << !tr_ << !table_ << !form_ << "\n";
}

time_t& article_manager::shift_date_struct(time_t& Time) {
	int ShiftFactor = ext_param::get_option("TIMESHIFT").Val();
	if (ShiftFactor != 0) 
		Time+=(ShiftFactor * 3600);
	return Time;
}

CString article_manager::shift_date(const CString& Date){
	int ShiftFactor = ext_param::get_option("TIMESHIFT").Val();
	if ((ShiftFactor != 0)&&(Date.StrLength())) {
		char sep, weekday[10], month[10], hour[10];	
		static char * months[] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
		static char * weekdays[] = {"Sun", "Mon","Tue","Wed","Thu","Fri","Sat"};
		int year;
		struct tm thisdate;
		memset(&thisdate, 0, sizeof(thisdate));
		sscanf(Date.asString(), "%s%s%d%s%d", weekday, month, &thisdate.tm_mday, hour, &year);
		for (int i=0;i<12;i++) if (!strcmpi(month, months[i])) {thisdate.tm_mon = i; break;}
		for (int j=0;j<7;j++) if (!strcmpi(weekday, weekdays[j])) {thisdate.tm_wday = j + 1; break;}
		thisdate.tm_year = year - 1900;
		sscanf(hour, "%d%c%d%c%d", &thisdate.tm_hour, &sep, &thisdate.tm_min, &sep, &thisdate.tm_sec);		
		time_t thistime = mktime(&thisdate);
		thistime+=(ShiftFactor * 3600);
		CString Result = asctime(localtime(&thistime));
		Result.StrTrim32();
		return Result;
	} else return Date;
}

void article_manager::generate_pages_count(cgiOutStream& CGIStream) {

    int PageCount = 1;
    int CurrentPage = ext_param::get_value("PAGE").Val();
    
	if (CurrentPage) {
        PageCount = ext_param::get_value("ARTICLES-PAGE").Val();
        if (PageCount)
            PageCount = articles.count() / PageCount + 1;
    }
    
    CGIStream << PageCount;
}
    
void article_manager::generate_pages(cgiOutStream& CGIStream) {
	int l_CurPage = ext_param::get_value("PAGE").Val();
	if (!l_CurPage) 
        return;

	CString call_local(getenv("SCRIPT_NAME")); 
    call_local += "?"; 
    call_local += params->argv[1]; 
    call_local += "+"; 
    call_local += params->argv[1]; 
    call_local += "HTML"; 
    
	CVector<CString> ParamVector = params->c_params().Tokenizer('+');
	for (int i=0; i < ParamVector.Count(); i++) {
		if (!ParamVector[i].StartsWith("page") && (ParamVector[i].StrLength())) {
			call_local += "+";
			call_local += ParamVector[i];
		}
    }
    
	if (archive_category.StrLength()) { 
        call_local += "+archive+"; 
        call_local += archive_category;  
    }
    
	call_local += "+page";
    
	int l_PageCount = ext_param::get_value("ARTICLES-PAGE").Val();
    if (! l_PageCount)
        return;
    
    int l_Pages = articles.count() / l_PageCount + 1;    
    
    // previous page
    if (l_CurPage > 1) {
        CString Lnk(call_local);
        Lnk += l_CurPage - 1;
        CGIStream << a_.o("HREF", Lnk.asString(), "TARGET", ext_param::get_option("SOURCE").asString(), 0)
                  << ext_param::get_option("PAGE-PREV") << !a_ << "\n";
    }
    
    // page links
    for (int Page = 1; Page <= l_Pages; Page++) {
        CString Lnk(call_local); 
        Lnk += Page;
        if (Page == l_CurPage) {
            CGIStream << Page << "\n";
        } else {
            CGIStream << a_.o("HREF", Lnk.asString(), "TARGET", ext_param::get_option("SOURCE").asString(), 0)
                      << Page << !a_ << "\n";
        }
    }
    
    // next page
    if (l_CurPage < l_Pages) {
        CString Lnk(call_local);
        Lnk += l_CurPage + 1;
        CGIStream << a_.o("HREF", Lnk.asString(), "TARGET", ext_param::get_option("SOURCE").asString(), 0) 
                  << ext_param::get_option("PAGE-NEXT") << !a_ << "\n";
    }
}

CString article_manager::reset_page_one(const CString& cmd){
	CString AddParams(cmd);
    
	int pPos = AddParams.isSubStr("+page");
	if (pPos == -1) 
        return AddParams;
    
    pPos += strlen("+page");
    int jPos = pPos; 
    while ((AddParams.StrLength() > jPos) && (isdigit(AddParams[jPos]))) 
        jPos++; 
    if (jPos != pPos) {
        AddParams.StrDelete(pPos, jPos - pPos);
        AddParams.StrInsert(pPos, "1");
    }
    
	return AddParams;
}

void article_manager::set_user_manager(CUserManager * i_user_manager){
	user_manager = i_user_manager;
}
