#include <agnespch.hpp>

template <class T>
void html_base<T>::traverse_tags(cgiOutStream& CGIStream, T * container_class, const CString& s){	

#define DOLLAR_LEFT "<$--"
#define DOLLAR_RIGHT "--$>"
#define DOLLAR_MIDDLE "-$-"

  CString Result;
  if (1) {
    int p_left = 0;
    int left = s.isSubStr(DOLLAR_LEFT, 0);
    while (left >= 0) { 
      int right = s.isSubStr(DOLLAR_RIGHT, left);
      if (right >= 0) {
        Result+=s.Copy(p_left, left - p_left);
        left+=strlen("<$--");
        CString Extract = s.Copy(left, right-left);
        if (Extract.StartsWith("RANDOM")){
          Extract.StrDelete(0, strlen("RANDOM"));
          CVector<CString> RandomTokens;
          Extract.Tokenizer(DOLLAR_MIDDLE, RandomTokens);
          struct tm *newtime; time_t aclock;
          time(&aclock); newtime = localtime(&aclock);
          int RNDVal = newtime->tm_sec % RandomTokens.Count();
          Result+=RandomTokens[RNDVal];	
        } else {
          Result+=DOLLAR_LEFT;
          Result+=Extract;
          Result+=DOLLAR_RIGHT;
        }
        p_left = right + (sizeof(DOLLAR_RIGHT)-1);
        left = s.isSubStr(DOLLAR_LEFT, p_left);      
      } else break;
    }
    Result+=s.Copy(p_left, s.StrLength());
  }

#define EXCL_LEFT "<!--"
#define EXCL_RIGHT "-->"

  if (1) {
    int p_left = 0;
    int left = Result.isSubStr(EXCL_LEFT, 0);
    while (left >= 0) {
      int right = Result.isSubStr(EXCL_RIGHT, left);
      if (right >= 0) {      
	CGIStream << Result.Copy(p_left, left - p_left);
	left+=(sizeof(EXCL_LEFT)-1);
	CString Extract = Result.Copy(left, right-left);
	if (!(Extract.StartsWith("SET"))) {
	  if ((!Extract.StrLength())||(!(container_class->traverse_tags(CGIStream, Extract)))) 
	    CGIStream << EXCL_LEFT << Extract << EXCL_RIGHT;
	}
        p_left = right + (sizeof(EXCL_RIGHT)-1);
        left = Result.isSubStr(EXCL_LEFT, p_left);      
      } else break;
    }
    CGIStream << Result.Copy(p_left, Result.StrLength());
  }
}

template <class T>
html_base<T>::html_base(cgiOutStream& CGIStream, const CString& a_html_document, T * container_class, const CVector<CString>& argv){
  html_base_init(CGIStream, a_html_document, container_class, argv, 1);
}

template <class T>
html_base<T>::html_base(cgiOutStream& CGIStream, const CString& a_html_document, T * container_class, const CVector<CString>& argv, int process_post){
  html_base_init(CGIStream, a_html_document, container_class, argv, process_post);
}

template <class T>
void html_base<T>::ip_banning(cgiOutStream& CGIStream, T * container_class) {
  CString RemoteHost = getenv("REMOTE_HOST");
  CString RemoteAddr = getenv("REMOTE_ADDR");
  CString SError("Remote IP banned<br>The script's Webmaster has denied your IP from access.<br><font color=\"red\">Please do not send email to Vestris Inc., we only provide the BBS service.</font>: ");
  CString IPBan(container_class->get_value("BAN-IP"));
  CVector<CString> IPBanVector; IPBan.Tokenizer(',',IPBanVector);
  for (int j=0;j<IPBanVector.Count();j++)
    if (IPBanVector[j].StrLength()) {
      int l_Num = 1;
      for (int l=IPBanVector[j].StrLength()-1;l>=0;l--)
	if (!isdigit(IPBanVector[j][l]) && (IPBanVector[j][l]!='.')) {
	  l_Num = 0;
	  break;
	}    
      if (l_Num) {
	//CGIStream << "comparing " << IPBanVector[j] << " with " << RemoteAddr << elf;
	if (RemoteAddr.StartsWith(IPBanVector[j])) {
	  SError += RemoteAddr; SError+=" ("; SError+=IPBanVector[j]; SError+=")";
	  cgi_error(CGIStream, SError);
	}
      } else {
	//CGIStream << "comparing " << IPBanVector[j] << " with " << RemoteHost << elf;
	if (RemoteHost.EndsWith(IPBanVector[j])) {
	  SError += RemoteHost; SError+=" ("; SError+=IPBanVector[j]; SError+=")";
	  cgi_error(CGIStream, SError);
	}
      }      
    }
}

template <class T>
void html_base<T>::html_base_init(cgiOutStream& CGIStream, const CString& a_html_document, T * container_class, const CVector<CString> & argv, int process_post){ 
  html_document = a_html_document;  
  for (int i=3;i<argv.Count(); i++) {
    container_class->traverse_cline_internal(CGIStream, argv[i]);
    container_class->traverse_cline(CGIStream, argv[i]);
  }  
  retreive_html_document(container_class->equivs->get_value(CGIStream, "Cache", 0), container_class->get_value("CACHED").Val());
  if (!contents.StrLength()) 
      cgi_error(CGIStream, "error retreiving html document");
  container_class->traverse_options(contents);    
  ip_banning(CGIStream, container_class);
  container_class->traverse_after_options(CGIStream);
  if ((process_post)&&(container_class->params->isPost() == 1)) {
      container_class->traverse_post(CGIStream);
  }
  traverse_tags(CGIStream, container_class, contents);
  common_footer(CGIStream, 1);    
}

template <class T>
html_base<T>::html_base(cgiOutStream& CGIStream, CString& cContents, const CVector<CString>& argv, T * container_class){
  contents = cContents;  
  for (int i=3;i<argv.Count(); i++) {
    container_class->traverse_cline_internal(CGIStream, argv[i]);
    container_class->traverse_cline(CGIStream, argv[i]);
  }  
  if (!error_raised) container_class->traverse_options(contents);    
  if (!error_raised) container_class->traverse_after_options(CGIStream); 
  if ((!error_raised) && (container_class->params->isPost() == 1)) container_class->traverse_post(CGIStream);
  if (!error_raised) traverse_tags(CGIStream, container_class, contents);  
}

template <class T>
html_base<T>::html_base(cgiOutStream& CGIStream, const CString& a_html_document, T * container_class, int argc, char * argv[]){ 
  html_document = a_html_document;
  for (int i=3;i<argc; i++) {
    container_class->traverse_cline_internal(CGIStream, argv[i]);
    container_class->traverse_cline(CGIStream, argv[i]);
  }
  retreive_html_document(container_class->equivs->get_value(CGIStream, "Cache", 0), container_class->get_value("CACHED").Val());    
  if (!contents.StrLength()) cgi_error(CGIStream, "error retreiving html document");  
  container_class->traverse_options(contents);  
  container_class->traverse_after_options(CGIStream);
  if (container_class->params->isPost() == 1){
    container_class->traverse_post(CGIStream);
  }
  traverse_tags(CGIStream, container_class, contents);
  common_footer(CGIStream, 1);  
}

template <class T>
html_base<T>::~html_base(void){
  
}

template <class T>
void html_base<T>::retreive_html_document(const CString& cache, int cached){    
  nextgen_v2 html(html_document, cache, cached);  
  contents = html.contained;
  for (register int i=0;i<contents.StrLength();i++)
    if (contents[i] == 0)
      contents[i] = ' ';
}

template class html_base<ext_param>;

