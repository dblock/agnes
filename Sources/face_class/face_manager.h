#ifndef faces_manager_h
#define faces_manager_h

#include "face_class.h"
#include <cstring/cstring.h>
#include <slist/slist_v2.h>

class agnes_article;

class faces_manager {
 public:
	faces_manager(const CString& container);
	inline int Count(void) const { return faces_list.count(); }
	CString link(int id);
	CString comment(int id);
	void show_face(cgiOutStream& CGIStream, int id);
	void show_table(cgiOutStream& CGIStream, agnes_article * article, int Cols = 2);
 private:
	ilist <agnes_face *> faces_list;
 protected:
};

#endif
