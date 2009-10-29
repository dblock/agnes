#ifndef article_archive_h
#define article_archive_h

#include <sys/types.h> 
#include <sys/stat.h>
#include <fcntl.h>

#include <platform/platform.h>
#include <cstring/cstring.h>
#include <cvector/cvector.h>
#include <mozilla/mozilla_v2.h>

#include "../article_manager/article_manager.h"

class article_archive {
public:
  article_archive(cgiOutStream& CGIStream, const CString& _root, const CString& _name, pParam_v2 * _params, article_manager * _template_manager, article_tree * head, agnes_article * limit, bool bTar);
  void displace_tree(cgiOutStream& CGIStream, const CString& full_alias, article_tree * head);
  article_archive(const CString& _root, const CString& _name, pParam_v2 * _params, article_manager * _template_manager);
  ~article_archive(void);
  void archive_link(cgiOutStream& CGIStream);
  CString full_root;
  ilist <article_tree *> articles;	
  void fill_headers(void);
  CString clean_name;	
  CString dirty_name();
#ifdef ANSI
  struct stat * stats;	
#else
  struct _stat * stats;
#endif	
  int operator > (const article_archive&);
  int operator < (const article_archive&);  
  bool find_article(const CString& node);
  bool find_article(const CString& node, article_tree * head);
private:	
  CString root;
  CString name;
  CString cmd;
  pParam_v2 * params;
  article_manager * template_manager;
protected:
  void common_init(const CString& _root, const CString& _name, pParam_v2 * _params, article_manager * _template_manager);
};

class article_archive_manager {
public:
  ilist <article_archive *> archives;
  article_archive_manager(const CString& _root, pParam_v2 * _params, article_manager * _template_manager);
  ~article_archive_manager(void);
  void generate_push(cgiOutStream& CGIStream);
  void generate_pull(cgiOutStream& CGIStream);
  void create_article_archive(cgiOutStream& CGIStream, const CString& _name, pParam_v2 * _params, article_manager * _template_manager, article_tree * _head, agnes_article * _limit, bool bTar);
  void pull_archives(const CString& _root, const CString& _name, pParam_v2 * _params, article_manager * _template_manager);
  void merge_archives(cgiOutStream& CGIStream, const CString& _root, const CString& _name, pParam_v2 * _params, article_manager * _template_manager, bool bTar);
#ifdef ANSI
  static void detar_archive(cgiOutStream& CGIStream, const CString& root, const CString& archive);
  static void tar_archive(cgiOutStream& CGIStream, const CString& root, const CString& archive);
#endif
  bool find_article(const CString& node, CString& archive);
private:    
  CString root;
  pParam_v2 * params;
  article_manager * template_manager;
  CString init_structure(cgiOutStream& CGIStream, const CString& _root, const CString& _name);
  void move_directory(cgiOutStream& CGIStream, const CString& target, const CString& source);
protected:
  void init_archives(void);
};

#endif
