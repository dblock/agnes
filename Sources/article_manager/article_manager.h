#ifndef article_manager_h
#define article_manager_h

#include <ext_param/ext_param.h>
#include <cusermanager/CUserManager.h>

class recent_manager;
class agnes_article;
class faces_manager;
class article_archive_manager;

#include <slist/slist_v2.h>
#include <cdate/cdate.h>

#define A_DELETE 10
#define A_EDIT 11
#define A_ARCHIVE 12
#define A_NORMAL 9
#define A_EDIT_SELECTED 13
#define A_SEARCH 8
#define A_MERGE 14
#define A_PULL 15
#define A_PREVIEW 16
#define REGEXP_SEARCH 17

typedef struct _search_results{
	CString contents;
	agnes_article * article;
	int occurences;
	inline int operator>(const _search_results& ) const { return 0; }
	inline int operator<(const _search_results& ) const { return 0; }
} search_results;

typedef struct _article_tree {
	ilist <_article_tree *> replies;
	agnes_article * article;
	inline int operator< (const _article_tree& ) const { return 0; }
	inline int operator> (const _article_tree& ) const { return 0; }
} article_tree;

class article_manager : public ext_param {
 public:
        bool bTar;
	CDate Current;
	int hideEMAIL;
	int hideNAME;
	int hideDATE;
	article_manager(cgiOutStream& CGIStream, const CString& _article_container, pParam_v2 * _params, cgi_equiv_manager * _equivs);
	~article_manager(void);
	pParam_v2 * params;
	cgi_equiv_manager * equivs;
	recent_manager * recent_structure;
	void traverse_post(cgiOutStream& CGIStream);
	void traverse_cline(cgiOutStream& CGIStream, const CString& s);
	int traverse_tags(cgiOutStream& CGIStream, const CString& s);
	void traverse_after_options(cgiOutStream& CGIStream);  
	CString check_dir(const CString&, const CString&);
	void output_articles(cgiOutStream& CGIStream);
	void output_banner(cgiOutStream& CGIStream);
	void output_articles(cgiOutStream& CGIStream, article_tree * tree, int, int);
	void write_header(ofstream * container_file, article_tree * tree);
	inline void write_header(cgiOutStream& CGIStream) { write_header_fatal(CGIStream, 0); }
	CString contents_root;
	CString current_root;
	CString archives_root;
	ilist<article_tree *> articles;
	void insert_article(agnes_article * article);
	CString shift_date(const CString&);
	time_t& shift_date_struct(time_t& Time);
	int state;
	void GetTerm(CString& Term);
	static CString reset_page_one(const CString&);
	void set_user_manager(CUserManager * i_user_manager);
 private:  
	CUserManager * user_manager;
	int postedModerated;
	int globDecal;
	int article_count;
	int old_limit;
	int old_hours;
	long old_minutes;
	long old_seconds;
	int collapse;
	int total_occurences;
    int banner_shown;
	cgi_access_manager cgi_access;
	article_archive_manager * archives;
	article_tree * current_tree;
	agnes_article * current_article;
	CString archive_category;
	CString root;
	int header_html;
	CString article_node;	
	faces_manager * faces_list;
	CString article_container;
	CString AExpand;
	CString ACollapse;
	void read_header(cgiOutStream& CGIStream);		
	void insert_article(agnes_article * article, article_tree * tree);		
	void article_link(cgiOutStream& CGIStream, agnes_article * article);
	void article_link(cgiOutStream& CGIStream, agnes_article * article, article_tree * tree, int level);
	void article_collapse(cgiOutStream& CGIStream, agnes_article * article, article_tree * tree, int level);
	void article_parent_row(cgiOutStream& CGIStream);
	void article_link(cgiOutStream& CGIStream, agnes_article * article, int decal);
	void post_name_row(cgiOutStream& CGIStream);
	void generate_home(cgiOutStream& CGIStream);
	void read_faces(void);
	void new_link(cgiOutStream& CGIStream);
	void generate_push(cgiOutStream& CGIStream);
	CString check_archive(const CString&);
	CString check_contents(const CString&);	
	void generate_contents(cgiOutStream& CGIStream);
	void output_current_thread(cgiOutStream& CGIStream);
	void generate_followups(cgiOutStream& CGIStream, article_tree * current_tree, int cmpl, int full);
	void generate_followups_table(cgiOutStream&, article_tree *, int);
	article_tree * get_tree(cgiOutStream& CGIStream, const CString& node);
	article_tree * get_tree(cgiOutStream& CGIStream, const CVector<int>& zlist);
	article_tree * get_tree(cgiOutStream& CGIStream, article_tree * tree, const CVector<int> & zlist);
	void title_link(cgiOutStream& CGIStream, agnes_article * article, int, const CString&);
	void generate_inside(cgiOutStream& CGIStream, agnes_article * article, int plain);
	void generate_next(cgiOutStream& CGIStream);
	void generate_prev(cgiOutStream& CGIStream);
	void generate_parent(cgiOutStream& CGIStream, int FullParent);
	void follow_link(cgiOutStream& CGIStream, agnes_article * article, const CString& mess);
	void post_new(cgiOutStream& CGIStream);
	void post_common(cgiOutStream& CGIStream);
	void post_follow(cgiOutStream& CGIStream);
	inline void post_new_article(cgiOutStream& CGIStream) { post_article_general(CGIStream, articles); }
	void post_followup_article(cgiOutStream& CGIStream);
	int last_article_id(ilist <_article_tree *> & articles);
	void post_article_general(cgiOutStream& CGIStream, ilist <_article_tree *> & articles);
	void edit_article(cgiOutStream& CGIStream);
	void edit_post(cgiOutStream& CGIStream);
	void delete_articles(cgiOutStream& CGIStream);
	void archive_post(cgiOutStream& CGIStream);
	void search_post(cgiOutStream& CGIStream);
	void regexp_post(cgiOutStream& CGIStream);
	void generate_search(cgiOutStream& CGIStream);
	void generate_regexp_search(cgiOutStream& CGIStream);
	void generate_categories_search_row(cgiOutStream& CGIStream);
	void archive_article(cgiOutStream& CGIStream);
	CVector<int> get_search_categories(void);
	void search_generate_results(cgiOutStream& CGIStream);
	void regexp_generate_results(cgiOutStream& CGIStream);
	void search_generate_recurse(cgiOutStream& CGIStream, CVector<CString>& search_strings, ilist <search_results *> & search_res, ilist <article_tree *> & articles, const CVector<int>& Categories);
	void regexp_generate_recurse(cgiOutStream& CGIStream, const CString&, ilist <search_results *> & search_res, ilist <article_tree *> & articles, const CVector<int>& Categories);
	int search_text(cgiOutStream& CGIStream, CString&, const CVector<CString>&, const CString& case_sens, const CString& whole_words);
	int regexp_search_text(cgiOutStream& CGIStream, const CString&, CString&);
	void output_and_clean(cgiOutStream& CGIStream, ilist <search_results *> & search_res, int total_occurences);
	void br_output(cgiOutStream& CGIStream, const CString& i_str);
	void generate_archive_pull(cgiOutStream& CGIStream, const CString& action);
	void archive_merge(cgiOutStream& CGIStream);
	void archive_pull(cgiOutStream& CGIStream);
	void write_header_fatal(cgiOutStream& CGIStream, int fatal);
	void post_article_preview(cgiOutStream& CGIStream, agnes_article * an_article);
	CString article_title(cgiOutStream& CGIStream, agnes_article * an_article);
	CString author_name(cgiOutStream& CGIStream, agnes_article * an_article);
	CString author_mail(cgiOutStream& CGIStream, agnes_article * an_article);  
	void output_recent(cgiOutStream& CGIStream, article_tree * tree);  
	CString& Map(const CString& Expression, CString& Target);
	CString Map(const CString& Expression);
	void generate_pages(cgiOutStream& CGIStream);
	void auto_archive(void);
    void generate_pages_count(cgiOutStream& CGIStream);
    int traverse_tags_common(cgiOutStream& CGIStream, const CString& s);        
};

#endif
