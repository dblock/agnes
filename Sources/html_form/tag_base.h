#ifndef html_tag_h
#define html_tag_h

#include <stdarg.h>
#include <cstring/cstring.h>
#include <base_entry/entry.h>

class html_tag : public entry_manager {
public:
  CString name;
  ostream & output(ostream & os) const;
  cgiOutStream& output(cgiOutStream& os) const;
  CString operator!(void) const;
  html_tag(char * name, ...);
  CString o(char * option, char * value, ...) const;
  ~html_tag(void);
};

ostream& operator<<(ostream & os, const html_tag & tag);
cgiOutStream& operator<<(cgiOutStream& os, const html_tag & tag);

#include "html_classes.h"

#endif
