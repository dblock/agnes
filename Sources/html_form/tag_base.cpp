#include <agnespch.hpp>

cgiOutStream& operator<<(cgiOutStream& os, const html_tag& tag){
  return tag.output(os);
}

ostream& operator<<(ostream & os, const html_tag & tag){
  return tag.output(os);
}

CString html_tag::o(char * option, char * value, ...) const {
  CString res("<");
  res+=name;
  va_list options;
  va_start(options, value);
  do {
    if ((!option)||(!value)) {
      //cgiOut << "html_tag::o() - malformed class: opt [" << option << "][" << value << "]" << elf;
    } else if (strlen(value)&&strlen(option)) {
      res += " ";
      res += option;
      res += "=\"";
      res += value;
      res += "\"";
    } else if (strlen(value)) {
      res += " ";
      res += value;
    }
  } while (((option = va_arg(options, char *))!=0)&&((value = va_arg(options, char *))!=0));
  va_end(options);
  res += ">";
  return res;
}

ostream & html_tag::output(ostream & os) const {
  os << "<" << name << ">";
  return os;
}

cgiOutStream& html_tag::output(cgiOutStream& os) const {
  os << "<" << name << ">";
  return os;
}

html_tag::html_tag(char * a_name, ...){
  name = a_name;
}

html_tag::~html_tag(void){
}

CString html_tag::operator!(void) const {
  CString close("</");
  close+=name;
  close+=">";
  return close;
}
