#ifndef C_ENTRYMULTIPLE_H
#define C_ENTRYMULTIPLE_H

#include <cstring/cstring.h>
#include <chashtable/chashtable.h>

class CEntryMultiple{
public:
  CString Name;
  CStringHashTable Entries;
  CEntryMultiple(void);
  CEntryMultiple(const CString&);
  CEntryMultiple& operator=(const CEntryMultiple&);
  CEntryMultiple(const CEntryMultiple&);
  int operator!=(const CEntryMultiple&);
};

#endif
