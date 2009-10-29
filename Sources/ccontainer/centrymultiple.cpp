#include <agnespch.hpp>

CEntryMultiple::CEntryMultiple(const CEntryMultiple& Source) : Entries(CString("")) {
  // cout << "CEntryMultiple::CEntryMultiple()[" << Source.Name << "]" << endl;
  Name = Source.Name;
  Entries = Source.Entries;
}

CEntryMultiple::CEntryMultiple(void) : Entries(CString("")) {
  // cout << "CEntryMultiple::CEntryMultiple(void)" << endl;
}

CEntryMultiple::CEntryMultiple(const CString& cName) : Entries(CString("")) {
  // cout << "CEntryMultiple::CEntryMultiple(Name)" << endl;
  Name = cName;
}

CEntryMultiple& CEntryMultiple::operator=(const CEntryMultiple& Elt) {
  // cout << "CEntryMultiple::operator=(const)" << endl;
  Name = Elt.Name; 
  Entries = Elt.Entries; 
  return (* this); 
}

int CEntryMultiple::operator!=(const CEntryMultiple& Elt) {
  return (Name != Elt.Name);
}

ostream& operator<<(ostream& os, const CEntryMultiple&) { 
  return os; 
}


