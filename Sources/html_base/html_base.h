#ifndef html_base_h
#define html_base_h

#include "../nextgen/nextgen_v2.h"
#include "../cstring/cstring.h"

template <class T>
class html_base{
private:
  CString html_document;
  void retreive_html_document(const CString&, int);
  void traverse_tags(cgiOutStream&, T *, const CString& s);
  void ip_banning(cgiOutStream&, T *);
protected:
  CString contents;
public:
  html_base(cgiOutStream& CGIStream, CString&, const CVector<CString>& argv, T * container_class);
  html_base(cgiOutStream& CGIStream, const CString& e_html_document, T * container_class, int argc, char * argv[]);
  html_base(cgiOutStream& CGIStream, const CString&, T *, const CVector<CString>&, int);
  html_base(cgiOutStream& CGIStream, const CString&, T*, const CVector<CString>&);
  void html_base_init(cgiOutStream& CGIStream, const CString&, T*, const CVector<CString>&, int);
  ~html_base(void);
};

#endif
