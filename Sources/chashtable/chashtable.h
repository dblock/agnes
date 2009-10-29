#ifndef C_HASHTABLE_H
#define C_HASHTABLE_H

#include "../cstring/cstring.h"
#include "../platform/platform.h"
#include "../platform/cgi_aid.h"

template <class V>
class CHashTable {
public:
  CHashTable(V zero);
  ~CHashTable();
  int GPow2(int);
  V * Add(const CString&, const V&);
  int Contains(const CString&) const;
  V Search(const CString&) const;
  V * SearchRef(const CString&) const;
  V * GetRef(int);
  int Hash(const CString&, int) const;
  float LoadFactor() const;
  int Count(void) const;
  int Size(void) const;
  int Grow(int new_size = 0);
  void Clear();
  V operator[](int) const;
  V Get(int) const;
  CString GetKey(int) const;
  CHashTable& operator=(const CHashTable&);
  V * Set(const CString&, const V&);
  V * Set(const CString&, const V&, int, int);
  CHashTable& Clone(const CHashTable&);
  CHashTable(const CHashTable&);
  void Del(const CString&);
  void Del(int);
  void DelHash(int);
private:
  int ndx;
  int cached_ndx;
  int size_;
  int nElements_;
  CString * Karray_;
  V * Varray_;
  V Vzero_;
};

#define CStringHashTable CHashTable<CString>

#endif
