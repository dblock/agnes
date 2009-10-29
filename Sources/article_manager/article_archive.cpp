#include <agnespch.hpp>

int article_archive::operator > (const article_archive& Other){
	return name > Other.name;
}

int article_archive::operator < (const article_archive& Other){
	return name < Other.name;
}

CString article_archive::dirty_name(void){
	return(name);
}

void article_archive::fill_headers(void){
	if (articles.count()) return;
	CString article_container = root;
	article_container+=name;
	article_container+=".list";
	nextgen_v2 article_header_file(article_container);
	ilist <article_tree *> articles_main = template_manager->articles;
	template_manager->articles = articles;
	CVector<CString> Tokens;
	CString Line;

	while (!article_header_file.feofc()){
		article_header_file.read_line(Line);
		Line.Tokenizer('\t',Tokens);
		agnes_article * an_article = new agnes_article(Tokens);
		if (!an_article->malformed) template_manager->insert_article(an_article);
	}
	articles = template_manager->articles;
	template_manager->articles = articles_main;
}

void article_archive::common_init(const CString& _root, const CString& _name, pParam_v2 * _params, article_manager * _template_manager){
	assert(_root.StrLength());
	assert(_name.StrLength());  
	root = _root;
	name = _name;
	params = _params;
	full_root = root;
	full_root.append_backslash();
	full_root+=name;
	CString list_name = full_root;
	list_name+=".list";
	full_root.append_backslash();
	template_manager = _template_manager;
	clean_name = _name;
    clean_name.Replace('_',' ');
#ifndef ANSI
	stats = new struct _stat;
	_stat(list_name.asString(), stats);
#else
	stat(list_name.asString(), stats = new struct stat);
#endif	
	cmd = getenv("SCRIPT_NAME"); cmd+="?"; cmd+=params->argv[1]; cmd+="+"; cmd+=params->argv[1]; cmd+="HTML+archive+"; cmd+=name; 
	cmd += article_manager::reset_page_one(params->c_params());
}

void article_archive::displace_tree(cgiOutStream& CGIStream, const CString& full_alias, article_tree * head){
  CString DotNode = head->article->article_node();
  DotNode += ".";
  
#ifdef ANSI  
  DIR * hFile;
  struct dirent * c_file;
  hFile = opendir(template_manager->contents_root.asString());
  while ((c_file = readdir(hFile))) {
    CString d_name(c_file->d_name);
    if ((head->article->article_node() == d_name) || d_name.StartsWith(DotNode)) {
      CString SourceFq = template_manager->contents_root;
      SourceFq += d_name;
      CString TargetFq = full_alias;
      TargetFq += d_name;
      movefile(SourceFq, TargetFq);
    }
  }
  closedir(hFile);  
#else  
  CString FindQuery = template_manager->contents_root;
  FindQuery += head->article->article_node();
  FindQuery += ".*";
  struct _finddata_t c_file;
  long hFile;
  if((hFile = _findfirst(FindQuery.asString(), &c_file )) == -1L ) 
    return;
  else do {
    CString d_name(c_file.name);
    CString SourceFq = template_manager->contents_root;
    SourceFq += d_name;
    CString TargetFq = full_alias;
    TargetFq += d_name;
    movefile(SourceFq, TargetFq);
  } while( _findnext( hFile, &c_file ) == 0);
  _findclose( hFile );
#endif
}

article_archive::article_archive(cgiOutStream& CGIStream, const CString& _root, const CString& _name, pParam_v2 * _params, article_manager * _template_manager, article_tree * _head, agnes_article * _limit, bool bTar){
  
	int i;

    for (i=0;i<_name.StrLength();i++) {
        if (!isalnum(_name[i])) {
            if ((_name[i] != ' ') && 
                (_name[i] != '.') && 
                (_name[i] != '-')) {
                cgi_error(CGIStream, "archive name must consist of plain english characters, spaces, dots and dashes");
            }
        }
    }
    
	CString dirty = _name;
	dirty.Replace(' ','_');
	common_init(_root, dirty, _params, _template_manager);
	CString full_alias_list = _root;	
	full_alias_list+=dirty;
	CString full_alias(full_alias_list);
	full_alias.append_backslash();
	
	CGIStream << "[considering new archive structure (" << full_alias << ")]" << elf;

	full_alias_list+=".list";	
    
	if (nextgen_v2::FileExists(full_alias_list))
        cgi_error(CGIStream, "error generating archive - alias already exists");
    
	template_manager->check_dir(_root, dirty);

	ofstream container_file(full_alias_list.asString(), ios::out | ios::trunc, filebuf::openprot);
	if (!container_file.is_open())
	  cgi_error(CGIStream, "unable to open header file");
	
	ilist <_article_tree *> articles;

	int green_light = 0;
	for (i = (* (_head->article->parent)).count();i;i--){		
	  if ((* (_head->article->parent))[i - 1]->article == _limit) green_light = 1;
	  if (green_light) {
	    articles+=(* (_head->article->parent))[i - 1];
	    (* (_head->article->parent))-=(i - 1);
	  }
	}
	
	CGIStream << "[randomizing v2 magic number for node direct hit]" << elf; CGIStream.flush();
	template_manager->write_header(CGIStream);

	CGIStream << "[transferring an article structure of " << articles.count() << " headers]" << elf;
	CGIStream << "[reversing circular gaussian references]" << elf;
	
	for (int m = articles.count(); m; m--) {
	  displace_tree(CGIStream, full_alias, articles[m - 1]);
	  template_manager->write_header(& container_file, articles[m - 1]);
	}

#ifdef ANSI
	if (bTar)
	  article_archive_manager :: tar_archive(CGIStream, _root, dirty);
#endif
    
	CGIStream << "[HAL 16000 was happy to do this job for your, have a nice day...]" << elf;
}

article_archive::article_archive(const CString& _root, const CString& _name, pParam_v2 * _params, article_manager * _template_manager){  
	common_init(_root, _name, _params, _template_manager);
}

article_archive::~article_archive(void){
}

void article_archive::archive_link(cgiOutStream& CGIStream){
	CGIStream << a_.o("HREF",cmd.asString(),"TARGET",template_manager->get_option("SOURCE").asString(),0) << clean_name << !a_ << "\n";
}

bool article_archive::find_article(const CString& node, article_tree * head) {
  if (head->article->article_node().StartsWith(node)) {
    
    if (head->article->article_node() == node)
      return true;
  
    for (int n = 0; n < head->replies.count();n++) {
      if (find_article(node, head->replies[n])) {
	return true;
      }
    }
  }

  return false;
}

bool article_archive::find_article(const CString& node) {

  for (int m = 0; m < articles.count(); m++) {
    if (find_article(node, articles[m]))
      return true;
  }
  
  return false;
}

//--- article_archive_manager

void article_archive_manager::create_article_archive(cgiOutStream& CGIStream, const CString& _name, pParam_v2 * _params, article_manager * _template_manager, article_tree * _head, agnes_article * _limit, bool bTar){
	archives += new article_archive(CGIStream, root, _name, _params, _template_manager, _head, _limit, bTar );
}

int article_date_cmp(const void * first, const void * second){
	return((* (article_archive **) first)->stats->st_mtime - (* (article_archive **) second)->stats->st_mtime);
}

void article_archive_manager::init_archives(void){  
	CString lroot(root);
#ifdef ANSI  
	DIR * hFile;
	struct dirent * c_file;
	hFile = opendir(lroot.asString());
	while ((c_file = readdir(hFile))) {
		CString d_name(c_file->d_name);
		if (d_name.EndsWith(".list")) {
            d_name.StrDelete(d_name.StrLength() - 5, 5);
            if (d_name.StrLength())
                archives += new article_archive(root, d_name, params, template_manager);            
        }
	}
	archives.sort(article_date_cmp);
	closedir(hFile);
#else  
	lroot+="*.list";
	struct _finddata_t c_file;
	long hFile;
	if((hFile = _findfirst(lroot.asString(), &c_file )) == -1L ) return;
	else do {
		CString d_name(c_file.name);
        d_name.StrDelete(d_name.StrLength() - 5, 5);
        archives += new article_archive(root, d_name, params, template_manager);
	} while( _findnext( hFile, &c_file ) == 0);
	archives.sort(article_date_cmp);
	_findclose( hFile );
#endif
}

article_archive_manager::article_archive_manager(const CString& _root, pParam_v2 * _params, article_manager * _template_manager){
	params = _params;
	root = _root;
	template_manager = _template_manager;  
	init_archives();
}

article_archive_manager::~article_archive_manager(void){
}

void article_archive_manager::generate_push(cgiOutStream& CGIStream){	
	if (template_manager->state == A_DELETE) {
		CString call_local(getenv("SCRIPT_NAME")); call_local+="?"; call_local+=template_manager->params->argv[1]; call_local+="+"; call_local+=template_manager->params->argv[1]; call_local+="HTML";  call_local+=template_manager->params->c_params();
		CGIStream << form_.o("METHOD","post","ACTION",call_local.asString(),0) << "\n" << table_;    
		CGIStream << tr_ << td_.o("COLSPAN","2", 0) << "(delete archive(s))" << input_.o("TYPE","hidden","NAME","action","VALUE","delete",0) << !td_ << !tr_ << "\n";
		CGIStream << tr_ << td_.o("COLSPAN","2", 0) << input_.o("TYPE","submit","NAME","post","VALUE","proceed",0) << " \n"
			  << input_.o("TYPE","reset","NAME","reset","VALUE","recycle",0) << "\n" << !td_ << !tr_;		
		CGIStream << tr_ << td_ << "password: " << !td_ << td_ << input_.o("TYPE","password","NAME","pass","VALUE", params->get_entry("pass").asString(), 0) << "\n" << !tr_ << !td_;
		CGIStream << !table_;
	}
	
	CString ABefore = template_manager->get_option("OPEN-ARCHIVE");
	CString AAfter = template_manager->get_option("CLOSE-ARCHIVE");
	CString ABetween = template_manager->get_option("BETWEEN-ARCHIVE");
	if (!template_manager->get_option("REVERSE-ARCHIVE").Val()) {	
	   for (int i=0;i<archives.count();i++){
		if (i) CGIStream << ABetween;
		CGIStream << ABefore;
		if (template_manager->state == A_DELETE) CGIStream << input_.o("TYPE","checkbox","NAME","victim_archive","VALUE",archives[i]->dirty_name().asString(),0) << " ";
		archives[i]->archive_link(CGIStream);    
		CGIStream << AAfter;
	   }
 	} else {
	   for (int i=archives.count()-1;i>=0;i--){
		if (i != archives.count()) CGIStream << ABetween;
		CGIStream << ABefore;
		if (template_manager->state == A_DELETE) CGIStream << input_.o("TYPE","checkbox","NAME","victim_archive","VALUE",archives[i]->dirty_name().asString(),0) << " ";
		archives[i]->archive_link(CGIStream);    
		CGIStream << AAfter;
	   }
 	}

	if (template_manager->state == A_DELETE) CGIStream << !form_;
}

void article_archive_manager::generate_pull(cgiOutStream& CGIStream){
	for (int i=0;i<archives.count();i++){		
		CGIStream << input_.o("TYPE","checkbox","NAME","index","VALUE",archives[i]->dirty_name().asString(),0) << " ";
		archives[i]->archive_link(CGIStream);
		CGIStream << !input_ << elf;
	}
} 

CString article_archive_manager::init_structure(cgiOutStream& CGIStream, const CString& _root, const CString& _name){
	CString dirty(_name);
	dirty.Replace(' ','_');
	CString full_alias_list(_root);
	full_alias_list+=dirty;
	CString full_alias(full_alias_list);
    
	CGIStream << "[considering new archive structure (" << full_alias << ")]" << elf;
    
    full_alias_list += ".list";
    
	if (nextgen_v2::FileExists(full_alias_list))
        cgi_error(CGIStream, "error generating archive - alias already exists");
    
	template_manager->check_dir(_root, dirty);
	return(full_alias);
}

void article_archive_manager::pull_archives(const CString& , const CString& , pParam_v2 * , article_manager *){
	return;
	/*
	  cgi_error("the pull function has not been implemented yet");    
	  init_structure(_root, _name);     
	*/
}

int file_date_cmp(const CString& first, const CString& second){
#ifdef ANSI
	struct stat s_first;
	struct stat s_second;
#else
	struct _stat s_first;
	struct _stat s_second;
#endif
	CString first_list(first);
	CString second_list(second);
	first_list+=".list";
	second_list+=".list";    
#ifdef ANSI
	stat(first_list.asString(), & s_first);
	stat(second_list.asString(), & s_second);
#else
	_stat(first_list.asString(), & s_first);
	_stat(second_list.asString(), & s_second);
#endif  
	return(s_first.st_mtime - s_second.st_mtime);
}

#ifdef ANSI
void article_archive_manager::detar_archive(cgiOutStream& CGIStream, const CString& root, const CString& archive) {
    CString Command("tar fxz ");
    Command += root;
    Command.append_backslash();
    Command += archive;    
    Command += ".tar.gz ";
    Command += " --directory=";
    Command += root;
    
    // CGIStream << Command << "<br>";
    
    system(Command.asString());
}

void article_archive_manager::tar_archive(cgiOutStream& CGIStream, const CString& root, const CString& archive) {
    
    if (!archive.StrLength())
        return;
    
    CString Command("tar fcz ");
    Command += archive;    
    Command += ".tar.gz ";
    Command += " --directory=";
    Command += root;
    Command += " ";
    Command += archive;
    Command += ".list ";
    Command += archive;
    Command.append_backslash();
    Command += "*";
    
    // CGIStream << Command << "<br>";
    
    system(Command.asString());
    
    Command = "rm -r ";
    Command += root;
    Command.append_backslash();
    Command += archive;
    
    // CGIStream << Command << "<br>";
    
    system(Command.asString());
}
#endif
    
void article_archive_manager::merge_archives(cgiOutStream& CGIStream, const CString& _root, const CString& _name, pParam_v2 * _params, article_manager * _template_manager, bool bTar){
	int i;
    
    for (i=0;i<_name.StrLength();i++) {
        if (!isalnum(_name[i])) {
            if ((_name[i] != ' ') && 
                (_name[i] != '.') && 
                (_name[i] != '-')) {
                cgi_error(CGIStream, "archive name must consist of plain english characters, spaces, dots and dashes");
            }
        }
    }
    
    if (! _name.StrLength()) {
        cgi_error(CGIStream, "archive name cannot be empty");        
    }    
    
	CString full_alias = init_structure(CGIStream, _root, _name);
    
	CString full_alias_list(full_alias);
	full_alias.append_backslash();
	full_alias_list+=".list";
	CVector<CString> merge_containers;
    
	CGIStream << "randomizing magic level 4 number for direct hit at " << br_ << full_alias << br_ << full_alias_list << elf;

	for (i=0; i< _params->entries_count();i++){	  
		if (_params->get_name(i) == "index") {
			merge_containers += _params->get_entry(i);
		}
	}
    
    CStringQSort(merge_containers, &file_date_cmp);    
    
	ofstream container_file(full_alias_list.asString(), ios::out | ios::trunc, filebuf::openprot);
    
	for (int j=0;j<merge_containers.Count();j++){
#ifdef ANSI
	  if (bTar)
	    detar_archive(CGIStream, _root, merge_containers[j]);
#endif
        CString current_container_list(merge_containers[j]);    
        current_container_list += ".list";
		ifstream current_container_file(current_container_list.asString(), ios::in, ios::
#ifdef linux
						bin
#else
						binary
#endif
						);
		container_file << current_container_file.rdbuf();
		CGIStream << "transagregating container: [" << current_container_list << "]" << elf;
        current_container_file.close();
        
		move_directory(CGIStream, full_alias, merge_containers[j]);
        
        unlink(current_container_list.asString());
        
        CString TarredArchive(merge_containers[j]);
        TarredArchive += ".tar.gz";
        unlink(TarredArchive.asString());
	}

    container_file.close();
    
	CString dirty(_name);
	dirty.Replace(' ','_');
#ifdef ANSI
    if (bTar)
      tar_archive(CGIStream, root, dirty);    
#endif
}

void article_archive_manager::move_directory(cgiOutStream& CGIStream, const CString& _target, const CString& _source){
	CString source(_source);
	CString target(_target);
	CGIStream << br_ << "moving structure " << br_ << source << br_ << target;
	source.append_backslash();
	target.append_backslash();
	CString lroot(source);
  
#ifdef ANSI  
	DIR * hFile;
	struct dirent * c_file;
	hFile = opendir(lroot.asString());
    if (hFile) {    
        while ((c_file = readdir(hFile))) {
            CString tdir(lroot);
            tdir+=c_file->d_name;
            CString d_name(c_file->d_name);
            if ((d_name!=".")&&(d_name!="..")) {
	      CGIStream << br_ << "moving " << source << c_file->d_name;
	      CString fname(source);
	      fname+=c_file->d_name;	      
	      movefile(fname, target);
            }
        }
        closedir(hFile);
    }
#else  
	lroot+="*.*";
	struct _finddata_t c_file;
	long hFile;
	if((hFile = _findfirst(lroot.asString(), &c_file )) == -1L ) return;
	else do {
		CString d_name(c_file.name);
		if ((d_name!=".")&&(d_name!="..")) {
		  // CGIStream << br_ << "moving " << source << c_file.name;
		  CString fname(source);
		  fname += c_file.name;
		  movefile(fname, target);
		}
	} while( _findnext( hFile, &c_file ) == 0);
	_findclose( hFile );
#endif
	CGIStream.flush();
	source.remove_backslash();
	rmdir(source.asString());
}

bool article_archive_manager::find_article(const CString& node, CString& archive) {
  if (! archives.count())
    return false;
  
  for (int i=archives.count()-1;i>=0;i--) {
    
    archives[i]->fill_headers();

    if (archives[i]->find_article(node)) {
      archive = archives[i]->dirty_name();
      return true;
    }
  }

  return false;
}





