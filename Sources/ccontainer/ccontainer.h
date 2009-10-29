#ifndef ccontainer_h
#define ccontainer_h

#include <platform/platform.h>
#include <fcntl.h>
#include <platform/cgi_aid.h>
#include <cvector/cvector.h>
#include <cstring/cstring.h> 

#include "centrymultiple.h"
#include <chashtable/chashtable.h>

class entry_manager {
  friend ostream& operator<<(ostream& out, const entry_manager&);
public:
  CStringHashTable Singles;
  CHashTable<CEntryMultiple> Multiples;

  inline void add_value(const CString& name, const CString& value) { Singles.Add(name, value); }
  CString make_equiv_path(const CString& structure);
  CString Root;
  entry_manager(void);
  entry_manager(entry_manager&);
  ~entry_manager(void);
  
  inline void set_value(const CString& name, const CString& value) { set_value(name, value, 0, 0); }
  inline void set_value(const CString& name, const CString& value, int fail, int warning) { Singles.Set(name, value, fail, warning); }

  inline CString get_value(const CString& name) const { return Singles.Search(name); }
  inline CString get_value(int index) const { return Singles.Get(index); }
  inline CString get_name(int index) const { return Singles.GetKey(index); } 
  
  void add_value(const CString& class_name, const CString& name, const CString& value);
  void set_value(const CString& class_name, const CString& name, const CString& value, int fail, int warning);
  inline void set_value(const CString& class_name, const CString& name, const CString& value) { set_value(class_name, name, value, 0, 0); }
  CString get_value(const CString& class_name, const CString& name) const;
  CString get_value(const CString& class_name, int index) const;
  inline CString get_name(int class_index, int index) const { return Multiples[class_index].Entries.GetKey(index); }
  CString get_name(const CString& class_name, int index) const;
  inline CString get_value(int class_index, int index) const { return Multiples[class_index].Entries.Get(index); }
  
  inline CString get_class(int index) const { return Multiples[index].Name; }

  inline int class_count(void) const { return Multiples.Count(); }
  inline int class_count(int class_index) const { return Multiples[class_index].Entries.Count(); }
  int class_count(const CString& class_name);

  inline int find_single(const CString& class_name) const { return Singles.Contains(class_name); }
  inline int find_multiple(const CString& class_name) const { return Multiples.Contains(class_name); }
  inline int entries_count(void) { return Singles.Count(); }
  void clear(void);

  void del_value(const CString& class_name);
};

#endif
