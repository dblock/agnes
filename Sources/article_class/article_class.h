#ifndef article_class_h
#define article_class_h

#include <platform/platform.h>
#include <platform/cgi_extern.h>
#include <slist/slist_v2.h>

class agnes_article;

#include "../article_manager/article_manager.h"

class agnes_article {
 private:
	CVector<CString> contents;
 public:  
#ifdef ANSI 
	static inline int max(int i, int j) { if (i>j) return i; else return j; }
#endif	
	CVector<int> article_levels;
	
	inline int node_starts(const CString& node) const { if (contents[0].Pos('.') == -1) return (contents[0] == node); else return (contents[0].StartsWith(node)); }
	
	inline CString& article_node(void) { return contents[0]; }
	inline CString& article_title(void) { return contents[1]; }
	inline CString& article_date(void) { return contents[2]; }
	inline CString& author_name(void) { return contents[3]; }
	inline CString& author_mail(void) { return contents[4]; }
	inline int follow_mail(void) { return contents[5].Val(); }
	inline int article_face(void) { return contents[6].Val(); }
	inline int article_moderated(void) { return contents[7].Val(); }
	inline CString& article_ip(void) { return(contents[8]); }
	inline CString& article_host(void) { return(contents[9]); }
	inline int private_compare(const CVector<int>& levels, int n) const;
	inline int private_compare(agnes_article * target) const { return private_compare(target, max(article_levels.Count(), target->article_levels.Count())); }
	inline int private_compare(agnes_article * target, int n) const { return private_compare(target->article_levels, n); }
	inline int private_compare(const CVector<int>& levels) const { return private_compare(levels, max(article_levels.Count(), levels.Count())); }
	inline CString& cut(CString& i, int sz) { if (i.StrLength() > sz) i.StrDelete(sz, i.StrLength()); return i; }
	inline void set_article_node(const CString& i) { contents[0] = i; }
	inline void set_article_title(const CString& i) { contents[1] = i; cut(contents[1], 512);}
	inline void set_article_date(const CString& i) { contents[2] = i; }
	inline void set_author_name(const CString& i) { contents[3] = i; cut(contents[3], 64); }
	inline void set_author_mail(const CString& i) { contents[4] = i; cut(contents[4], 64); }
	inline void set_follow_mail(const CString& i) { contents[5] = i; cut(contents[5], 64); }
	inline void set_article_face(const CString& i){ contents[6] = i; }
	inline void set_article_moderated(const CString& i) { contents[7] = i; }
	inline void set_article_ip(const CString& i) { contents[8] = i; }
	inline void set_article_host(const CString& i) { contents[9] = i; }

	int malformed;

	inline int is_node(agnes_article * target) const { if (article_levels.Count() > target->article_levels.Count()) if (!private_compare(target, target->article_levels.Count())) return 1; return 0; }
	inline int has_node(const CVector<int>& levels) const { if (article_levels.Count() < levels.Count()) if (!private_compare(levels, article_levels.Count())) return 1; return 0; }
	inline int is_child(agnes_article * target) const { if (article_levels.Count() == target->article_levels.Count() + 1) if (is_node(target)) return 1; return 0; }
	inline int has_parent(void) const { CVector<int> Id = article_levels; Id.DeleteElt(Id.Count()-1); return Id.Count(); }
	void article_descriptor(ofstream * container);
	agnes_article(const CVector<CString>& Descriptor);
	~agnes_article(void);

	CString got_inside;
	CString& get_inside(const CString& root, const CString& archive, bool bTar);
	void write_contents(const CString& root, const CString& article_contents);
	inline agnes_article * get_next(void) const { if (!parent) return 0; else if (id == parent->count() - 1) return(0); else return(((*parent)[id + 1])->article); }
	inline agnes_article * get_prev(void) const { if (id == 0) return 0; else if (parent) return(((*parent)[id - 1])->article); else return 0; }
	void send_parent_mail(agnes_article *, article_manager * template_manager, bool bTar);

	ilist <article_tree *> * parent;
	article_tree * parent_tree;
	int id;

	int has_unmoderated_replies(void) const;
	inline static void mlevel_generator(const CString& node, CVector<int>& vector);

#ifdef ANSI
    void agnes_article_detar(const CString& root, const CString& archive);
#endif
    
 protected:
	void mlevel_generator(void);
	void traverse_descriptor(const CVector<CString>&);
};

inline void agnes_article::mlevel_generator(const CString& node, CVector<int>& vector) {
	CVector<CString> Result; node.Tokenizer('.', Result);
	for (int i = 0;i<Result.Count();i++) 
		vector += Result[i].Val();
}

inline int agnes_article::private_compare(const CVector<int>& levels, int n) const {
	for (register int m = 0; m < n;m++){	
		if (levels.Count() <= m) return(1);
		else if (article_levels.Count() <= m) return(-1);
		else if (article_levels[m] < levels[m]) return(-1);
		else if (article_levels[m] > levels[m]) return(1);
	}
	return 0;
}

#endif
