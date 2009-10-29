#include <agnespch.hpp>

cgi_entry_manager::cgi_entry_manager(void){
#ifdef _DEBUG
  if (debug) cout << "cgi_entry_manager::cgi_entry_manager() - constructor" << elf;
#endif
}

cgi_entry_manager::~cgi_entry_manager(void){
#ifdef _DEBUG
  if (debug) cout << "cgi_entry_manager::~cgi_entry_manager() - destructor" << elf;
#endif
}

