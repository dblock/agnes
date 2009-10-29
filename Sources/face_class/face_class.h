#ifndef agnes_face_h
#define agnes_face_h

#include <cstring/cstring.h>

class agnes_face {
 public:
	agnes_face(int, const CString&, const CString&);
	~agnes_face(void);
	inline const CString& link(void) const { return face_link; }
	inline const CString& comment(void) const { return face_message; }
	inline int id() const { return face_id; }
	inline int operator < (const agnes_face& Other) const { return face_id < Other.face_id; }
	inline int operator > (const agnes_face& Other) const { return face_id > Other.face_id; }
 private:
	int face_id;
	CString face_link;
	CString face_message;
};

#endif
