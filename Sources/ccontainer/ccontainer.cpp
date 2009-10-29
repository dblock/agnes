#include <agnespch.hpp>

ostream& operator<<(ostream& os, const entry_manager& s){
	os << s.Singles.Count() << " single pairs" << "n";
	for (int i=0;i<s.Singles.Count();i++)
		os << "\t" << s.get_name(i) << "::" << s.get_value(i) << "n";
	os << s.Multiples.Count() << " recursive entries" << "n";
	for (int j=0;j<s.Multiples.Count();j++){
		os << "\t" << s.Multiples[j].Name << "n";
		for (int l=0;l<s.Multiples[j].Entries.Count();l++)
			os << "\t\t" << s.get_name(j, l) << "::" << s.get_value(j, l) << "n";
	}
	return os;
}

void entry_manager::clear(void){
	Singles.Clear();
	Multiples.Clear();
}

entry_manager::entry_manager(void) : Singles(""), Multiples(CEntryMultiple("")) {
	Root = nextgen_v2::get_rootdir();
}

entry_manager::entry_manager(entry_manager& s) : Singles(""), Multiples(CEntryMultiple("")) {
	Singles = s.Singles;
	Multiples = s.Multiples;
}

entry_manager::~entry_manager(void){

}

void entry_manager::del_value(const CString& class_name){
	int ndx = Multiples.Contains(class_name);  
	if (ndx >= 0) Multiples.DelHash(ndx);  
}

void entry_manager::add_value(const CString& class_name, const CString& name, const CString& value){
	CEntryMultiple * Entry = Multiples.SearchRef(class_name);
	if (!Entry) {
		CEntryMultiple CurrentEntry(class_name);
		Entry = Multiples.Add(class_name, CurrentEntry);
	}
	if (!Entry) {    
		//cgi_error("entry_manager::add_value() failed at CHashTable<CEntryMultiple>.Add()");
	} else {
		Entry->Entries.Add(name, value);
	}
}

void entry_manager::set_value(const CString& class_name, const CString& name, const CString& value, int fail, int warning){
	CEntryMultiple * Entry = Multiples.SearchRef(class_name);
	if (!Entry) {
		//if (fail) cgi_error("entry_manager::set_value() - failed (was fatal)");
		//if (warning) cgiOut << "entry_manager::set_value() - [" << class_name << "] class not found" << "\n";
		CEntryMultiple CurrentEntry(class_name);
		Entry = Multiples.Add(class_name, CurrentEntry);
	}
	Entry->Entries.Set(name, value, fail, warning);
}

CString entry_manager::get_value(const CString& class_name, const CString& name) const{
	CEntryMultiple * Entry = Multiples.SearchRef(class_name);
	if (!Entry) return "";
	return Entry->Entries.Search(name);
}

CString entry_manager::get_value(const CString& class_name, int index) const{
	CEntryMultiple * Entry = Multiples.SearchRef(class_name);
	if (!Entry) return "";
	return Entry->Entries.Get(index);
}

CString  entry_manager::get_name(const CString& class_name, int index) const{
	CEntryMultiple * Entry = Multiples.SearchRef(class_name);
	if (!Entry) return "";
	return Entry->Entries.GetKey(index);
}

int  entry_manager::class_count(const CString& class_name){
	CEntryMultiple * Entry = Multiples.SearchRef(class_name);
	if (!Entry) return 0; 
	else return Entry->Entries.Count();
}
  
CString entry_manager::make_equiv_path(const CString& structure){
	CString retStr = Root;
	retStr.append_backslash();

	retStr+="equiv";

	retStr.append_backslash();
	retStr+=structure;
	return retStr;
}



