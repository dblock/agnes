#include <agnespch.hpp>

article_manager::article_manager(cgiOutStream& CGIStream, const CString& _article_container, pParam_v2 * _params, cgi_equiv_manager * _equivs):
  ext_param(
	    "MailServer","",
	    "MODERATED", "0",  
	    "MODERATED-SUBJECT","moderated article",
	    "MODERATED-NAME","unspecified author",
	    "MODERATED-EMAIL","",
	    "MODERATED-SHOW", "1",
	    "MODERATED-POST", "Your articles are moderated, the post will not appear at once.",
	    "OLD", "0", 
	    "HOURS", "0",
	    "MINUTES", "0",
	    "SECONDS", "0",
	    "EXTFACES", "",
	    "EXTFACES-COLS", "2",
	    "SOURCE", "",
	    "TARGET", "",	    
	    "AEND", "</b>",
	    "ABEG", "<b>",
	    "BYMAIL", "receive followups by mail",
	    "MODERATED-CHECK", "moderate this article",
	    "MODERATED-TEXT", "[moderated!]&nbsp;",
	    "DAYS", "0",
	    "RECENT", "",
	    "REVERSE", "0",
	    "REVERSE-ARCHIVE", "0",
	    "NONEXT", "",
	    "NOPREV", "",
	    "NOPARENT", "",
	    "PREV", "[prev]",
	    "NEXT", "[next]",
	    "PAGE-NEXT","[next]",
	    "PAGE-PREV","[prev]",
	    "PARENT","[parent]",
	    "SEPARATOR", "",
	    "COMPLETE", "1",
	    "POSTBY", "in response to:",
	    "FLINK", "0",
	    "HOME", "[main page]",
	    "DATE", "date:",
	    "NEW", "[post a new article]",
	    "POST", "Post!",
	    "CLEAR", "Clear!",
	    "PREVIEW", "Preview!",
	    "NAME", "name:",
	    "MAIL", "e-mail:",
	    "SUBJECT", "subject:",
	    "FMSG", "followups:",
	    "SEARCH", "search:",
	    "SEARCH COMMAND", "Search!",
	    "SEARCH CLEAR", "Clear!",
	    "SEARCH TITLES", "titles only",
	    "SEARCH ARCHIVES", "archives",
	    "SEARCH CASESENS", "case sensitive",
	    "SEARCH WHOLE", "whole words only",
	    "SEARCH THREAD","searching in main thread ",
	    "SEARCH IN ARCHIVE","searching in archive",
	    "SEARCH FOUND"," occurence(s) found!",
	    "SEARCH NOT FOUND"," - no occurences found!",
	    "SEARCH CATEGORY","category:",
	    "SEARCH CATEGORIZE","0",
	    "SEARCH CATEGORY ALL","all categories",
	    "SEARCH NOTHING","nothing to search",
	    "SIZE","50",
	    "COLS","50",
	    "ROWS","14",
	    "SEARCH SIZE","30",
	    "OPEN", "",
	    "CLOSE", "",
	    "FOLLOW", "[reply]",
	    "BLANK", "/b.gif\" ALT=\" ",
	    "FORCE_REVERSE","-1",
	    "DUCON SUBJECT", "Wow, I can do HTML!",
	    "DUCON NAME", "Master of Universe",
	    "DUCON MAIL", "master@universe.gov",
	    "IP","0",
	    "IP-PASS", "",
	    "HIDE-IP", "0",
	    "OPEN-ARTICLE","",
	    "CLOSE-ARTICLE","",
	    "OPEN-MAIL","",
	    "OPEN-DATE","",
	    "CLOSE-MAIL","",
	    "CLOSE-DATE", "",
	    "PARENT-ROW","1",
	    "PARENT-POST","1",
	    "TWIDTH","1000%",
	    "FORCE-EMAIL","",
	    "FORCE-NAME","",
	    "FORCE-SUBJECT","",
	    "COLLAPSE-MORE","[+]",
	    "COLLAPSE-ALL","[*]",
	    "COLLAPSE-LESS","[-]",
	    "EXPAND-THREAD","+",
	    "NOEXPAND-THREAD","-",
	    "EXPANDED-THREAD",">",
	    "HIDE-MAIL","",
	    "HIDE-EMAIL","",
	    "RE-TAG","Re:",
	    "NOQUOTE-REPLY","",
	    "MAIL-HEADER","in response to your article: ",
	    "MAIL-FOOTER","",
	    "MAIL-WROTE","wrote:",
	    "ARTICLES-PAGE","0",
	    "PAGE","0",
	    "OPEN-ARCHIVE","<br>",
	    "BETWEEN-ARCHIVE", "",
	    "CLOSE-ARCHIVE", "",
	    "BANNER","popup",
	    "TIMESHIFT","0",
	    0) 
{
  postedModerated = 0;
  user_manager = 0;
  recent_structure = new recent_manager(this);
  current_article = 0;
  globDecal = 0;
  current_tree = 0;
  params = _params;
  equivs = _equivs;
  article_container = _article_container;
  article_node = 0;
  banner_shown = 0;
  CString t(params->argv[1]);
  t+="HTML";
  if (t==params->argv[2]) 
    header_html = 1; 
  else header_html = 0;

  // if ((!header_html)&&(params->argv.Count() == 3)&&(!params->isPost())) cgi_error("non conforming command line parameters");

  root = params->argv[1];
  root+="Parent";
  root = equivs->get_value(CGIStream, root);
  archives_root = check_archive(root);
  contents_root = check_contents(root);
  current_root = contents_root;
  archives = 0;
  state = A_NORMAL;
  if (equivs->get_value(CGIStream, "BLANK", 0).StrLength()) 
    ext_param::set_value("BLANK", equivs->get_value(CGIStream, "BLANK", 0));

  bTar = (equivs->get_value(CGIStream, "NOTAR", 0).StrLength() == 0);
}

article_manager::~article_manager(void){
  
}

void article_manager::traverse_post(cgiOutStream& CGIStream){
  CString action(params->get_entry("action"));
  if (!(action.StrLength())) return;
  if (action == "new article") { auto_archive(); post_new_article(CGIStream); } //--- posting a new article
  else if (action == "followup") { auto_archive(); post_followup_article(CGIStream); } //--- posting a followup
  else if (action == "delete") delete_articles(CGIStream); //--- delete article(s)
  else if (action == "search") search_post(CGIStream); // --- search
  else if (action == "regexp-search") regexp_post(CGIStream); // --- regexp search
  else if (action == "edit") edit_article(CGIStream); //--- edit choice
  else if (action == "edit post") edit_post(CGIStream); //--- edit confirm
  else if (action == "archive") archive_post(CGIStream); //--- archive
  else if (action == "pull") archive_pull(CGIStream);
  else if (action == "merge") archive_merge(CGIStream);
}

void article_manager::traverse_cline(cgiOutStream& CGIStream, const CString& s){    
  if (archive_category == "\1") {
    archive_category.Free();
    for (int i=0;i<s.StrLength();i++) 
      if (s[i] != '\\') 
        archive_category+=s[i]; 
      else if ((i+1 != s.StrLength()) && (s[i+1] == '\\'))
        archive_category+='\\';
  }
  else if (s == "delete") state = A_DELETE; //--- delete
  else if (s.StartsWith("ip")) { ext_param::set_value("IP","1"); ext_param::set_value("IP-PASS", s.Copy(strlen("ip"), s.StrLength())); }
  else if (s.StartsWith("old")&&(!archive_category.StrLength())) {ext_param::set_value("OLD", s.Copy(strlen("old"), s.StrLength())); params->extra_params.AddEltSortedUnique(s);}
  else if (s.StartsWith("hours")&&(!archive_category.StrLength())) {ext_param::set_value("hours", s.Copy(strlen("hours"), s.StrLength())); params->extra_params.AddEltSortedUnique(s);}
  else if (s.StartsWith("minutes")&&(!archive_category.StrLength())) {ext_param::set_value("minutes", s.Copy(strlen("minutes"), s.StrLength())); params->extra_params.AddEltSortedUnique(s);}
  else if (s.StartsWith("seconds")&&(!archive_category.StrLength())) {ext_param::set_value("seconds", s.Copy(strlen("seconds"), s.StrLength())); params->extra_params.AddEltSortedUnique(s);}
  else if (s.StartsWith("page")) {ext_param::set_value("page", s.Copy(strlen("page"), s.StrLength())); params->extra_params.AddEltSortedUnique(s);}
  else if (s.StartsWith("collapse")) {    
    ACollapse = s.Copy(strlen("collapse"), s.StrLength());
    params->extra_params.AddEltSortedUnique(s);      
  } else if (s.StartsWith("expand")) {
    AExpand = s.Copy(strlen("expand"), s.StrLength());
    params->extra_params.AddEltSortedUnique(s);
  } else if (s == "edit") state = A_EDIT;
  else if (s == "merge") state = A_MERGE; //--- merge archives
  else if (s == "pull") state = A_PULL;
  else if (s == "archive") archive_category = "\1";
  else if (s == "forward") {ext_param::set_value("FORCE_REVERSE","0"); params->extra_params.AddEltSortedUnique(s);}
  else if (s == "reverse") {ext_param::set_value("FORCE_REVERSE","1"); params->extra_params.AddEltSortedUnique(s);}
  else article_node = s;
}


article_manager * CurrentAM;

void AMGetTerm(CString& Term){
  CurrentAM->GetTerm(Term);
}

void article_manager::GetTerm(CString& Term){  
  if (Term == "DATE") Term = Current.Map("$day/$month/$year");
  else if (Term == "HOUR") Term = Current.Map("$hour");
  else if (Term == "MIN") Term = Current.Map("$min");
  else if (Term == "DAY") Term = Current.Map("$day");
  else if (Term == "MONTH") Term = Current.Map("$month");
  else if (Term == "YEAR") Term = Current.Map("$year");
  else if (Term == "TIME") Term = Current.Map("$hour:$min"); 
  else if (Term == "IP") Term = getenv("REMOTE_ADDR");  
  else if (Term == "ALIAS") Term = params->argv[1]; 
  else if (Term == "TEMPLATE") Term = params->argv[2];
}

CString& article_manager::Map(const CString& Expression, CString& Target){
  CurrentAM = this;
  Expression.MapTerm(AMGetTerm, Target);
  return Target;
}

CString article_manager::Map(const CString& Expression){
  CString Dummy;
  Map(Expression, Dummy);
  return Dummy;
}

int article_manager::traverse_tags_common(cgiOutStream& CGIStream, const CString& s) {
  if (s == "AGNES-ARTICLE-COUNT") { CGIStream << article_count; }
  else if (s == "AGNES-PAGES") generate_pages(CGIStream);
  else if (s == "AGNES-PAGE") { 
    if (ext_param::get_value("PAGE").Val()) 
      CGIStream << ext_param::get_value("PAGE"); 
    else CGIStream << "1"; 
  } else if (s == "AGNES-PAGES-COUNT") { generate_pages_count(CGIStream); }
  else if (s == "AGNES-BANNER") { output_banner(CGIStream); }
  else if (s.StartsWith("AGNES-")) return 1;
  else return 0;
  return 1;
}

int article_manager::traverse_tags(cgiOutStream& CGIStream, const CString& s){  
  CString archive_category_uc(archive_category);
  archive_category_uc.Replace('_', ' ');
  if (s.StartsWith("DYNAMIC--")) {
    CGIStream << Map(s.Copy(strlen("DYNAMIC--"),s.StrLength()));
  } else if (state == REGEXP_SEARCH){
    if (s == "REGEXP-SEARCH") regexp_generate_results(CGIStream);
    else if (s == "AGNES-HOME") generate_home(CGIStream);
    else if (s == "AGNES-NEWLINK") new_link(CGIStream);
  } else if (state == A_SEARCH){
    if (s == "AGNES-SEARCH") search_generate_results(CGIStream);
    else if (s == "AGNES-HOME") generate_home(CGIStream);
    else if (s == "AGNES-NEWLINK") new_link(CGIStream);
    else return 0;
  } else if (state == A_EDIT_SELECTED) {
    if (s == "AGNES-HEADER") {
      cgi_access.check_access(CGIStream, params->get_entry("pass"), params->argv[1]);
      current_tree = get_tree(CGIStream, params->get_entry("victim"));
      if (!current_tree) cgi_error(CGIStream, "unable to retreive article for edition, please choose at least one article");
      current_article = current_tree->article;
      post_follow(CGIStream);		
    } else if (s.StartsWith("AGNES-")) return 1;		
    else return 0;
  } else if ((state == A_MERGE)||(state == A_PULL)){
    if (s == "AGNES-ARCHIVE") generate_push(CGIStream);
    else if (s == "AGNES-HOME") {if (archive_category.StrLength()) generate_home(CGIStream);}
    else if (s.StartsWith("AGNES-")) return 1;
    else return 0;
  } else if (header_html) {
    if (s == "AGNES-HEADER") {
      output_banner(CGIStream);
      output_articles(CGIStream);
    }
    else if (s == "AGNES-HOME") {if (archive_category.StrLength()) generate_home(CGIStream);}
    else if (s == "AGNES-NEWLINK") {new_link(CGIStream);}
    else if (s == "AGNES-ARCHIVE") generate_push(CGIStream);
    else if (s == "AGNES-ANAME") {if (archive_category.StrLength()) CGIStream << archive_category_uc;}
    else if (s == "AGNES-SEARCH") generate_search(CGIStream);
    else if (s == "REGEXP-SEARCH") generate_regexp_search(CGIStream);
    else if (s == "MODERATED-POST") { if (postedModerated) CGIStream << ext_param::get_value("MODERATED-POST"); }
    else return (traverse_tags_common(CGIStream, s));
  } else {
    if (s == "AGNES-HEADER") output_current_thread(CGIStream);
    else if (s == "AGNES-HOME") generate_home(CGIStream);
    else if (s == "AGNES-SEARCH") generate_search(CGIStream);
    else if (s == "REGEXP-SEARCH") generate_regexp_search(CGIStream);
    else if (s == "AGNES-NEWLINK") new_link(CGIStream);
    else if (s == "AGNES-ANAME") {if (archive_category.StrLength()) CGIStream << archive_category_uc;}
    else if (s == "AGNES-ARTICLE") { generate_contents(CGIStream); }
    else if ((s == "AGNES-PREV")&&(state!=A_PREVIEW)) generate_prev(CGIStream);
    else if ((s == "AGNES-NEXT")&&(state!=A_PREVIEW)) generate_next(CGIStream);
    else if (s == "AGNES-PARENT") generate_parent(CGIStream, 0);
    else if (s == "ARTICLE-PARENT") generate_parent(CGIStream, 1);
    else if ((s == "AGNES-FOLLINK")&&(state!=A_PREVIEW)) {if (!(archive_category.StrLength())) follow_link(CGIStream, current_article, ext_param::get_value("FOLLOW")); }
    else if (s == "AGNES-FACE") {if (current_article) faces_list->show_face(CGIStream, current_article->article_face());}
    else if (s == "AGNES-POSTNEW") post_new(CGIStream);
    else if ((s == "AGNES-FOLLOWUP")&&(state!=A_PREVIEW)) {if (!(archive_category.StrLength()) && current_article) post_follow(CGIStream);}
    else if (s == "ARTICLE-TITLE") {if (current_article) CGIStream << article_title(CGIStream, current_article); }
    else if (s == "ARTICLE-DATE") {if (current_article) CGIStream << shift_date(current_article->article_date()); }
    else if (s == "ARTICLE-AUTHOR") {if (current_article) CGIStream << author_name(CGIStream, current_article); }
    else if (s == "ARTICLE-AUTHOR-MAIL") { if (current_article) CGIStream << author_mail(CGIStream, current_article); }
    else if (s == "ARTICLE-ARTICLE") { if (current_article) generate_inside(CGIStream, current_article, 0); }
    else if (s == "ARTICLE-FOLLOWUPS") { if (current_article) generate_followups_table(CGIStream, current_tree, 0); }
    else if (s == "MODERATED-POST") { if (postedModerated) CGIStream << ext_param::get_value("MODERATED-POST"); }
    else return (traverse_tags_common(CGIStream, s));
  } 	
  return 1;
}

void article_manager::traverse_after_options(cgiOutStream& CGIStream){  
  CGIStream.UnLock();

  if (archive_category=="\1") {
    archive_category.Free();
    state = A_ARCHIVE;
    ext_param::set_value("REVERSE","0");
  } else {
    if (ext_param::get_value("FORCE_REVERSE").Val() >= 0) 
      ext_param::set_value("REVERSE", ext_param::get_value("FORCE_REVERSE"));
  }

  if (archive_category.StrLength()) 
    state = A_NORMAL;  
   
  read_header(CGIStream);
  
  read_faces();
  
  if (article_node.StrLength()) {
    current_tree = get_tree(CGIStream, article_node); 

    if (! current_tree) {
      // article not found, try archives
      if (!archives) 
	archives = new article_archive_manager(archives_root, params, this);
      if (archives->find_article(article_node, archive_category)) {
	read_header(CGIStream);
	current_tree = get_tree(CGIStream, article_node);
      }
    }
  } else current_tree = NULL;
  
  if ((current_tree)&&(state!=A_PREVIEW)) 
    current_article = current_tree->article;

  old_limit = ext_param::get_value("OLD").Val();
  
  long oh = ext_param::get_value("HOURS").ValLong();
  long om = ext_param::get_value("MINUTES").ValLong();
  long os = ext_param::get_value("SECONDS").ValLong();
  old_seconds = os%60;
  old_minutes = (om%60 + os/60)%60;
  old_hours = oh + om/60;
  
  if (ACollapse.StrLength()) {
    collapse = ACollapse.Val();
    if (collapse < 0) collapse = 0;
  } else {
    collapse = 0;
  }
  
  hideEMAIL = ext_param::get_option("HIDE-MAIL").Val() || ext_param::get_option("HIDE-EMAIL").Val();
  hideNAME = ext_param::get_option("HIDE-NAME").Val();
  hideDATE = ext_param::get_option("HIDE-DATE").Val();
  
  if ((ext_param::get_value("IP").Val()) && (ext_param::get_value("HIDE-IP").Val())) {
    cgi_access.check_access(CGIStream, ext_param::get_value("IP-PASS"), params->argv[1]);
  }    
  
  if (ext_param::get_value("PAGE").Val()) {
    if (!ext_param::get_value("ARTICLES-PAGE").Val())
      ext_param::set_value("ARTICLES-PAGE", "25");
  }
}

void article_manager::read_header(cgiOutStream& CGIStream) {
    articles.clear();
    if (archive_category == "\1") 
      cgi_error(CGIStream, "maformed archive request");
    if (archive_category.StrLength()) {
      article_container = archives_root;
      article_container+=archive_category;
      article_container+=".list";
    }
    nextgen_v2 article_header_file(article_container);
    article_count = 0;
    if (article_header_file.feofc()) 
      return;
    CString Line;
    CVector<CString> Tokens;
    while (!article_header_file.feofc()) {
      Line = article_header_file.read_line();
      Line.Tokenizer('\t', Tokens);
      agnes_article * an_article = new agnes_article(Tokens);
      if (!an_article->malformed) { 
	insert_article(an_article); article_count++; 
      }
    }
}
  
void article_manager::write_header(ofstream * container_file, article_tree * tree){
  tree->article->article_descriptor(container_file);
  for (int m = 0; m<tree->replies.count();m++) write_header(container_file, tree->replies[m]);
}

void article_manager::write_header_fatal(cgiOutStream& CGIStream, int fatal){	
  ofstream container_file(article_container.asString(), ios::out | ios::trunc, filebuf::openprot);
  for (int m = 0; m<articles.count();m++) write_header(&container_file, articles[m]);	
}

int article_manager::last_article_id(ilist <_article_tree *> & articles){
  int imax = 0;
  int k = 0;
  for (int i =0;i<articles.count();i++){
    if (!articles[i]->article->malformed){
      k = articles[i]->article->article_levels[articles[i]->article->article_levels.Count()-1];
      if (k > imax) imax = k;
    }
  }
  return k;
}

void article_manager::insert_article(agnes_article * article, article_tree * tree){ 
  if (article->article_levels.Count() == tree->article->article_levels.Count() + 1) {
    article_tree * a_node = new article_tree;
    a_node->article = article;
    article->parent = &tree->replies;
    article->id = tree->replies.count();
    tree->replies+=a_node;
  } else {
    for (int i=tree->replies.count();i;i--){
      if (article->is_node(tree->replies[i-1]->article)) {
	insert_article(article, tree->replies[i-1]);
	return;
      }
    }
  }
}

void article_manager::insert_article(agnes_article * article){
  if (article->article_levels.Count() == 1) {
    article_tree * a_node = new article_tree;
    a_node->article = article;
    article->parent = &articles;
    article->id = articles.count();
    articles+=a_node;
  }
  else {
    for (int i=articles.count();i;i--){
      if (article->is_node(articles[i-1]->article)) {
	insert_article(article, articles[i-1]);
	return;
      }
    }
  }
}

void article_manager::post_followup_article(cgiOutStream& CGIStream){
  CString parent_node(params->get_entry("parent"));
  article_tree * tree = get_tree(CGIStream, parent_node);
  if (tree) post_article_general(CGIStream, tree->replies);
}

void article_manager::generate_archive_pull(cgiOutStream& CGIStream, const CString& action){
  CString call_local(getenv("SCRIPT_NAME")); call_local+="?"; call_local+=params->argv[1]; call_local+="+"; call_local+=params->argv[1]; call_local+="HTML";  call_local+=params->c_params();
  CGIStream 	<< br_ << "Choose archives to " << action << ": " << elf;
  CGIStream	<< form_.o("ACTION", call_local.asString(), "METHOD", "POST", 0);
  CGIStream	<< "password: ";
  CGIStream	<< input_.o("TYPE","password","NAME","pass",0) << !input_ << elf;
  CGIStream	<< "new alias name: ";
  CGIStream	<< input_.o("TYPE","text","NAME","alias",0) << !input_ << elf;
  CGIStream	<< input_.o("TYPE","submit","NAME","submit","VALUE","process",0) << !input_ << " ";
  CGIStream	<< input_.o("TYPE","reset","NAME","reset","VALUE","deselect",0) << !input_ << elf;
  CGIStream 	<< input_.o("TYPE","hidden","NAME","action","VALUE",action.asString(),0) << !input_ << elf;
  if (!archives) archives = new article_archive_manager(archives_root, params, this);
  archives->generate_pull(CGIStream);
  CGIStream << !form_;
}

void article_manager::generate_push(cgiOutStream& CGIStream){
  if (!archives) archives = new article_archive_manager(archives_root, params, this);
  if (state == A_PULL) generate_archive_pull(CGIStream, "pull");
  else if (state == A_MERGE) generate_archive_pull(CGIStream, "merge");
  else archives->generate_push(CGIStream);
}

void article_manager::output_banner(cgiOutStream& CGIStream) {
    
  if (banner_shown)
    return;
    
  if (!equivs->get_value(CGIStream, "NOROTATE", 0).StrLength()) {                                                      
    if (ext_param::get_value("BANNER") == "INLINE") {
      CGIStream << 
	"<script language=JavaScript>\n"                                                                            \
	"var nUrl = Math.round(Math.random() * 100)\n"                                                              \
	"var hrfUrl = 'http://kansas.valueclick.com/redirect?host=hs0040063&size=468x60&b=' + nUrl + '&v=0'\n"      \
	"var imgUrl = 'http://kansas.valueclick.com/cycle?host=hs0040063&size=468x60&b=' + nUrl + '&noscript=1'\n"  \
	"function OpenWindow(){\n"                                                                                  \
	"popupWin = window.open(hrfUrl, 'BannerAd', '')\n"                                                          \
	"}\n"                                                                                                       \
	"document.writeln('<img onClick=OpenWindow() src=' + imgUrl + "                                             \
	"' width=468 height=60 alt=\"Please visit our sponsors.\">');\n"                                            \
	"</script>\n";
    } else {
      CGIStream << 
	"<script language=\"JavaScript\"> function OpenWindow( url, name ) {\n"     \
	"popupWin = window.open(url, name, 'toolbar=no,marginwidth=0,"              \
	"marginheight=0,frameborder=0,vspace=0,hspace=0,width=550,height=80,"       \
	"screenX=50,screenY=50,resizable=no,scrollbars=no,status=no')}\n"           \
	"OpenWindow('http://www.vestris.com/bbs/en/rotate.html','Banner')\n"   \
	"</script>\n";
    }
    banner_shown = 1;
  }
}

#include <slist/slist_v2.cpp>
template class ilist<article_tree *>;
#include <html_base/html_base.cpp>
template class html_base<article_manager>;
template class ilist<search_results *>;
template class ilist<article_archive *>;
template class ilist<agnes_face *>;

