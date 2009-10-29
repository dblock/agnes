#include <agnespch.hpp>

#ifdef DEBUG
#define DBGWRITE(X) X
#else
#define DBGWRITE(X) 
#endif

#define INIT_STRING             \
  m_Size = STR_STATIC_BUFSIZE;  \
  m_Len = 0;                    \
  m_SStr[0] = 0;                \
  m_Str = m_SStr;               \
  m_DStr = 0;


CString::CString(void){
  INIT_STRING;
}

CString::CString(const char * s, int Len){	
  INIT_STRING;
  SetBuf(s, Len);
}

CString::CString(const CString& String){
  INIT_STRING;
  SetBuf(String.m_Str, String.m_Len);
}

CString::~CString(void) {
  DBGWRITE(cout << "CString::~CString(" << m_Len << ")(" << m_Size << ")[" << m_Str << "]" << endl);
  if (m_DStr) 
    delete[] m_DStr;
  DBGWRITE(cout << "CString::~CString() - done." << endl);
}

const long int C1 = 52845;
const long int C2 = 11719;

void CStringQSort(CVector<CString> & vector,  int (*cmp_function) (const CString&, const CString&));
CString bool_to_space(int bvalue);

void applyCStringVector(CVector<CString>& vector, CStringMemberFn memCString){
  for(int i=0;i<vector.Count();i++)
    callMemberFunction(vector.GetElt(i), memCString)();
}

CString& CString::Flush(CString& Target){
  DBGWRITE(cout << "CString::Flush()" << endl);
  Target = * this;
  return Target;
}

CString& CString::MapTerm(GetTermFunction Func, CString& Target) const {
  CString Term;
  int curPos = 0, prevPos = 0;
  CString Prefix;
  CString Postfix;
  CString Elsefix;
  CString CmdFix;
  int Regular, TagChar, IdChar;
  while (curPos < m_Len){
    if (m_Str[curPos] == '\\') {
      Target+=Copy(prevPos, curPos-prevPos);
      curPos++; 
      Target+=m_Str[curPos];
      curPos++;
      prevPos=curPos;
      curPos++;
    } else if ((m_Str[curPos] == '$')||(m_Str[curPos]=='£')) {
      IdChar = m_Str[curPos];
      Prefix.Free();
      Postfix.Free();
      Elsefix.Free();
      CmdFix.Free();
      Target+=Copy(prevPos, curPos-prevPos);
      curPos++;prevPos = curPos;
      while (isalnum(m_Str[curPos]) || (m_Str[curPos] == '_') || (m_Str[curPos] == '.')) curPos++;
      Term = Copy(prevPos, curPos-prevPos);   
      while (Term.StrLength() && ((Term[Term.StrLength()-1] == '.')||(Term[Term.StrLength()-1] == '_'))) {
	curPos--;
	Term.StrDelete(Term.StrLength()-1, 1);
      }  
      prevPos = curPos;
      while ((m_Str[curPos] == '~')||(m_Str[curPos] == '#')||(m_Str[curPos] == '^')||(m_Str[curPos] == '|')) {
	TagChar = m_Str[curPos];
	if (m_Str[curPos+1] == '[') Regular=1; else Regular=0;
	curPos++;prevPos = curPos;
	if (Regular) {
	  prevPos++;
	  Regular++;
	  while (m_Str[curPos]) {
	    if ((m_Str[curPos] == '\\')&&(m_Str[curPos+1]!='\\')) curPos++;
	    curPos++;
	    if (m_Str[curPos] == '[') Regular++;
	    else {
	      if (m_Str[curPos] == ']') Regular--;
	      if (Regular == 1) break;
	    }
	  }
	  if (m_Str[curPos] == ']') curPos++;
	  if (TagChar == '~') Prefix=Copy(prevPos,curPos-prevPos-1).MapTerm(Func);
	  else if (TagChar == '#') Postfix = Copy(prevPos,curPos-prevPos-1).MapTerm(Func);
	  else if (TagChar == '^') Elsefix = Copy(prevPos,curPos-prevPos-1).MapTerm(Func);
	  else if (TagChar == '|') CmdFix = Copy(prevPos,curPos-prevPos-1).MapTerm(Func);
	} else {
	  while ((m_Str[curPos]>' ')&&(m_Str[curPos]!='#')&&(m_Str[curPos]!='~')&&(m_Str[curPos]!='^')&&(m_Str[curPos]!='|')) curPos++;
	  if (TagChar == '~') Prefix=Copy(prevPos,curPos-prevPos).MapTerm(Func);
	  else if (TagChar == '#') Postfix = Copy(prevPos,curPos-prevPos).MapTerm(Func);
	  else if (TagChar == '^') Elsefix = Copy(prevPos,curPos-prevPos).MapTerm(Func);
	  else if (TagChar == '|') CmdFix = Copy(prevPos,curPos-prevPos).MapTerm(Func);
	}
	prevPos = curPos;
      }
      
      //if (Prefix.m_Len) Prefix = Prefix.MapTerm(Func);
      //if (Postfix.m_Len) Postfix = Postfix.MapTerm(Func);
      //if (Elsefix.m_Len) Elsefix = Elsefix.MapTerm(Func);

      Func(Term);
      
      if (CmdFix.StrLength()) {
	CVector<CString> Commands = CmdFix.Tokenizer(',');
	for (int c=0;c<Commands.Count();c++) {
	  if (Commands[c] == "TRIM") Term.StrTrim();
	  else if (Commands[c] == "TRIM32") Term.StrTrim32();
	  else if (Commands[c].StartsWith("IS")) {
	    Commands[c].StrDelete(0, strlen("IS"));
	    if (Term != Commands[c]) Term.Free();	    
	  } else if (Commands[c].StartsWith("NOT")) {
	    Commands[c].StrDelete(0, strlen("NOT"));
	    if (Term == Commands[c]) Term.Free();	    
	  } else if (Commands[c].StartsWith("HAS")) {
	    Commands[c].StrDelete(0, strlen("HAS"));
	    if (Term.isSubStr(Commands[c]) < 0) Term.Free();	    
	  } else if (Commands[c].StartsWith("STARTS")) {
   	    Commands[c].StrDelete(0, strlen("STARTS"));
	    if (!(Term.StartsWith(Commands[c]))) Term.Free();
          } else if (Commands[c].StartsWith("ENDS")) {
            Commands[c].StrDelete(0, strlen("ENDS"));
            if (!(Term.EndsWith(Commands[c]))) Term.Free();
          }
	}
      }

      if (Term.StrLength()){
	Target+=Prefix;
	if (IdChar != '£') Target+=Term;
	Target+=Postfix; 
      } else Target+=Elsefix;
      
    } else curPos++;
  }
  Target+=Copy(prevPos, m_Len);
  return Target;
}

CString& CString::Map(const CString& Expression, const CVector<CString> & SVector, CString& Target) const {
  Target.Free();
  int DPos = Expression.Pos('$');
  int i, PPos=0;
  while (DPos >= 0) {
    Target+=Expression.Copy(PPos, DPos-PPos);
    i=DPos+1;
    while ((Expression[i]<='9')&&(Expression[i]>='0')) i++;
    PPos = i;
    if (i!=DPos) {
      i = Expression.Copy(DPos+1, i-DPos).Val();
      if (i>=0) {
	if (SVector.Count() <= i) cout << "CString::Map(Error):- invalid index in regular expression - [" << (* this) << "][<b>" << i << "</b>]<br>\n";
	else Target+=SVector[i];
      }
    }    
    DPos = Expression.Pos('$', DPos+1);
  }
  Target+=Expression.Copy(PPos, Expression.m_Len);
  return Target;
}

CString& CString::ExtractLine(CString& Target){  
  int iPos = Pos('\n');
  if (iPos >= 0) {
    Target = Copy(0, iPos++);
    StrDelete(0, iPos);   
  } else {
    Flush(Target);
  }
  return Target;
}


CString& CString::Encrypt(void){
  DBGWRITE(cout << "CString::Encrypt()" << endl);
  int len = m_Len;
  if (len){		
    signed char t;
    int key = 1234;
    int i;
    char * tStr = new char[len*2+1];
    for (i=0;i<len;i++){
      t = m_Str[i] ^ (key >> 8);
      tStr[2*i] = (t & 15) + 64;
      tStr[2*i+1] = ((t & 240) >> 4) + 64;
      key = (t + key) * C1 + C2;
    }
    tStr[len*2]='\0';
    Assign(tStr, len * 2, len * 2 + 1);
  }
  return(* this);
}

CString& CString::Decrypt(void){
  DBGWRITE(cout << "CString::Decrypt()" << endl);
  int len = m_Len;
  if (len){
    signed char t;
    int key = 1234;
    int i;
    len/=2;
    char * tStr = new char[len+1];
    for (i=0;i<len;i++){
      t = (m_Str[2*i] - 64)  + ((m_Str[2*i+1] - 64) << 4);
      tStr[i] = t ^ (key >> 8);
      if (!tStr[i]) {
	len = i;
	break;
      }
      key = (t + key) * C1 + C2;
    }
    tStr[len]='\0';
    Assign(tStr, len, len + 1);
  }
  return(*this);
}

CVector<CString> CString::Tokenizer(const CString& iStr, int Sorted) const {
  CVector<CString> retVector;  
  Tokenizer(iStr, retVector, Sorted);
  return retVector;
}

CVector<CString>& CString::Tokenizer(const CString& iStr, CVector<CString>& retVector, int Sorted) const {
  retVector.Clear();
  retVector.Grow(m_Len / 5);
  CString Current(* this);  
  Current.UpCase();
  CString Target(iStr);
  Target.UpCase();
  int i,j = 0;
  i = Current.isSubStr(Target.asString(), j);
  while (i>=0) {
    CString tmp = Copy(j, i-j);
    if (Sorted) retVector.AddEltSorted(tmp); else retVector+=tmp;
    j = i+iStr.m_Len;
    i = Current.isSubStr(Target.asString(), j);
  }
  if (Sorted) retVector.AddEltSorted(Copy(j, m_Len - j)); else retVector+=Copy(j, m_Len - j);
  return(retVector);
}

CVector<CString> CString::Tokenizer(const char c, int Sorted) const {
  CVector<CString> retVector;
  Tokenizer(c, retVector, Sorted);
  return retVector;
}

CVector<CString>& CString::Tokenizer(const char c, CVector<CString>& retVector, int Sorted) const {
  retVector.Clear();
  retVector.Grow(StrLength() / 5);
  int i,j = 0;
  i = Pos(c, j);  
  while (i>=0) {
    if (Sorted) retVector.AddEltSorted(Copy(j,i-j)); else retVector+=Copy(j,i-j);
    j = i+1;
    i = Pos(c, j);
  }
  if (Sorted) retVector.AddEltSorted(Copy(j, m_Len-j)); else retVector+=Copy(j, m_Len-j);
  return (retVector);
}


CString& CString::Copy(int start, int len, CString& ret) const{
  DBGWRITE(cout << "CString::Copy()" << endl);
  ret.Free();
  if ((start<0)||(start>=m_Len)||(len<=0)) {   
    return ret;
  }
  if (start + len > m_Len) len = m_Len - start;
  if (!len) {
    return ret;
  }
  ret.SetSize(len + 1);
  memcpy(ret.m_Str, m_Str+start, len);
  ret.m_Str[len] = 0;
  ret.m_Len = len;
  return ret;
}

CString& CString::StrTrimRight(char l, char r){
  if (m_Len){
    char * iPos = m_Str + m_Len - 1;
    while ((iPos>=m_Str)&&(iPos[0]>=l)&&(iPos[0]<=r)) iPos--;
    StrDelete(iPos-m_Str+1, m_Len-(iPos-m_Str)+1);
  }
  return(*this);
}

CString& CString::StrTrimLeft(char l, char r){
  if (m_Len) {
    char * iPos = m_Str;
    while ((iPos[0]>=l)&&(iPos[0]<=r)&&(iPos[0])) iPos++;
    StrDelete(0, iPos-m_Str);
  }
  return(*this);
}

int CString::isSubStr(const char * s, int startpos) const {
  if (!m_Len) return -1;
  if (startpos >= m_Len) return -1;
  char * res = strstr(m_Str+startpos, s);
  if (res) return (res - m_Str); else return -1;
}

int CString::isSubStrI(const CString& s, int startpos) const{
  if (!m_Len) return -1;
  if ((startpos<0)||(startpos>=m_Len)) return -1;
  CString l(* this);
  CString r(s);
  r.UpCase();
  l.UpCase();
  char * res = strstr(l.m_Str + startpos, r.m_Str);
  if (res) return (res - l.m_Str); else return -1;
}

int CString::isSubStrI(const char * s, int startpos) const{
  CString r(s);
  return isSubStrI(r, startpos);
}

int CString::isSubStrI(const CString& s) const{
  if (!m_Len) return -1;
  CString l(* this);
  l.UpCase();
  CString r(s);
  r.UpCase();
  char * res = strstr(l.m_Str, r.m_Str);
  if (res) return (res - l.m_Str); else return -1;
}

int CString::isSubStrI(char * s) const {
  if (!m_Len) return -1;
  CString l(* this);
  l.UpCase();
  CString r(s);
  r.UpCase();
  char * res = strstr(l.m_Str, r.m_Str);
  if (res) return (res - l.m_Str); else return -1;
}

int CString::isSubStr(const int c) const {
  if (!m_Len) return -1;
  char * res = strchr(m_Str, c);
  if (res) return res - m_Str; else return -1;
}

int CString::isSubStr(char * s) const {
  if (!s||!m_Len) return -1;
  char * res = strstr(m_Str, s);
  if (res) return (res-m_Str); else return -1;
}

int CString::isSubStr(const CString& s) const {
  if (!s.m_Len||!m_Len) return -1;
  char * res = strstr(m_Str, s.m_Str);
  if (res) return (res - m_Str); else return -1;
}

int CString::strNStr(const CString& s) const {
  if ((!m_Len)||(!s.m_Len)) return 0;
  CString Source = (* this);
  CString Target = s;
  return (Source==Target);
}

int CString::operator>=(const CString& s) const {
  if (!m_Len&&!s.m_Len) return 1;
  if (!m_Len) return 0;
  if (!s.m_Len) return 1;

  if (strcmp(m_Str,s.m_Str)>=0) return 1;
  else return 0;
}

int CString::operator<=(const CString& s) const {
  if (!m_Len&&!s.m_Len) return 1;
  if (!m_Len) return 1;
  if (!s.m_Len) return 0;

  if (strcmp(m_Str,s.m_Str)<=0) return 1;
  else return 0;
}

int CString::operator>(const CString& s) const {
  if (!m_Len&&!s.m_Len) return 0;
  if (!m_Len) return 0;
  if (!s.m_Len) return 1;

  if (strcmpi(m_Str,s.m_Str)>0) return 1;
  else return 0;
}


int CString::operator<(const CString& s) const {
  if (!m_Len&&!s.m_Len) return 0;
  if (!m_Len) return 1;
  if (!s.m_Len) return 0;

  if (strcmp(m_Str,s.m_Str)<0) return 1; else return 0;
}

const CString& CString::operator=(const char* pc) {
  if (m_Str == pc) 
    return * this;
  
  DBGWRITE(cout << "CString::operator=(" << pc << ")" << endl);
  int Len = (pc ? strlen(pc) : 0);
  SetSize(Len + 1);
  memcpy(m_Str, pc, Len);
  m_Str[Len] = 0;
  m_Len = Len;
  return *this;
}

const CString& CString::operator=(const CString& s) {		
  if (&s == this)
    return * this;
  
  SetSize(s.m_Len + 1);
  memcpy(m_Str, s.m_Str, s.m_Len + 1);
  m_Len = s.m_Len;
  return *this;	
}

ostream& operator<<(ostream& os, const CString& sStr){
  //os << sStr.asString();
  os.write(sStr.asString(), sStr.StrLength());
  return os;
}

#define BUF_MAX 256

istream& operator>>(istream& is,CString& sStr) {
  char inbuf[BUF_MAX],ch;
  while(1) {
    ch='\n';
    while ((ch=='\n')||(ch==' ')) {
      ch=is.get();
      if ((ch!='\n')&&(ch!=' ')) {
	is.putback(ch);
	break;
      }
    }
    is.getline(inbuf,BUF_MAX-1);
  }
  sStr=inbuf;
  return is;
}

void CString::StrDelete(int start, int count) {	
  if (m_Len&&count){
    if (start+count > m_Len) count = m_Len - start;
    if (count >= m_Len) Free();
    else {
      memmove(m_Str + start, m_Str+start+count, m_Len-start-count+1);
      m_Len -= count;
    }
  }
}

void CString::StrAppend(const char * s, int jLen){
  DBGWRITE(cout << "CString::StrAppend() [" << s << "](" << jLen << ")" << endl);
  if (s) {
    if (jLen < 0) jLen = strlen(s);
    if (!jLen) return;
    SetSize(m_Len + jLen + 1);
    memcpy(m_Str + m_Len, s, jLen + 1);
    m_Len += jLen;
  }
}

void CString::ChrAppend(const char ch) {
    SetSize(m_Len + 2);
    m_Str[m_Len] = ch;
    m_Len++;
    m_Str[m_Len] = 0;
}

CString& CString::operator+=(const char i){
    ChrAppend(i);
    return * this;
}

CString& CString::operator+=(const int i){
  char workbuf[24];
  sprintf(workbuf, "%d", i);
  StrAppend(workbuf);
  return(*this);
}

CString& CString::operator+=(const long i){
  char workbuf[24];
  sprintf(workbuf, "%ld", i);
  StrAppend(workbuf);
  return(*this);
}

CString& CString::operator+=(const float i){
  char workbuf[24];
  sprintf(workbuf, "%f", i);
  StrAppend(workbuf);
  return(*this);
}

CString& CString::operator+=(const double i){
  char workbuf[24];
  sprintf(workbuf, "%e", i);
  StrAppend(workbuf);
  return(*this);
}

int CString::hash_sum(void){
  int sum = 0;
  for (int i=0;i< m_Len;i++) sum+=tolower(m_Str[i]);
  return sum;
}

CString& CString::add_br(void){
  int i=0;
  while ((i=isSubStr("\n", i)) >= 0) {
    int ii = i+1;
    while(m_Str[ii] < ' ') ii++;
    StrDelete(i, ii-i);
    StrInsert(i, "<br>");
    if (ii-i > (signed int) strlen("\n")) StrInsert(i, "<br>");
  }
  return(*this);
}

CString& CString::remove_br(void){
  int i=0;
  while ((i = isSubStr("<br>", i)) >= 0) {
    StrDelete(i, strlen("<br>"));
    StrInsert(i, "\n");
  }
  return(*this);
}

void CString::StrF(void){
  DBGWRITE(cout << "CString::StrF()" << endl);
  if (m_Len) {	
    int i, j;
    char * tstr = new char[m_Len+1];
    for (i = 0, j = 0;i<m_Len;i++) 
      if ((m_Str[i] > 32)&&
	  (m_Str[i] != ' ')&&(m_Str[i] != '!')&&(m_Str[i] != '@')&&
	  (m_Str[i] != '#')&&(m_Str[i] != '$')&&(m_Str[i] != '%')&&
	  (m_Str[i] != '^')&&(m_Str[i] != '*')&&(m_Str[i] != '(')&&
	  (m_Str[i] != ')')&&(m_Str[i] != '-')&&(m_Str[i] != '&')&&
	  (m_Str[i] != '_')&&(m_Str[i] != '-')&&(m_Str[i] != '+')&&
	  (m_Str[i] != '|')&&(m_Str[i] != '~')&&(m_Str[i] != '=')&&
	  (m_Str[i] != '\\')&&(m_Str[i] != '`')&&(m_Str[i] != '{')&&
	  (m_Str[i] != '}')&&(m_Str[i] != '[')&&(m_Str[i] != ']')&&
	  (m_Str[i] != ':')&&(m_Str[i] != ';')&&(m_Str[i] != '"')&&
	  (m_Str[i] != '\'')&&(m_Str[i] != '<')&&(m_Str[i] != '>')&&
	  (m_Str[i] != '?')&&(m_Str[i] != '/')&&(m_Str[i] != ',')&&
	  (m_Str[i] != '.')
	  ) tstr[j++] = m_Str[i];
    tstr[j]='\0';
    Assign(tstr, m_Len, m_Len + 1);
  }
}

void CString::fmakeword(FILE * f, char stop, int *cl) {
  char c;
  while((!feof(f))&&(--(*cl))) {
    c = (char) fgetc(f);
    if (c == stop) {
      return;
    }
    (* this)+=(char) c;
  }
}

char CString::x2c(char * what) {
  char digit;
  digit = (what[0] >= 'A' ? ((what[0] & 0xdf) - 'A')+10 : (what[0] - '0'));
  digit *= 16;
  digit += (what[1] >= 'A' ? ((what[1] & 0xdf) - 'A')+10 : (what[1] - '0'));
  return(digit);
}

CString& CString::Appendc2x(char c, char e){
  char workbuf[24];
  workbuf[0]=e;
  sprintf(workbuf+1, "%02X", c);
  workbuf[3] = 0;
  StrAppend(workbuf, 3);
  return(*this);
}

CString& CString::escape_url(void){
  if (m_Len) {
    CString Tmp;
    int prev = 0;
    for (int i=0;i<m_Len;i++){
      if (!(isalnum(m_Str[i])) && (m_Str[i]!=' ') && (m_Str[i]>0)){
	if (i>prev) Tmp+=Copy(prev, i-prev);
	prev = i+1;
	Tmp.Appendc2x(m_Str[i]);
      }
    }
    Tmp+=Copy(prev, m_Len-prev);    
    (* this) = Tmp;
  }
  return (* this);
}

CString& CString::escape(void){
  if (m_Len) {
    CString Tmp;
    int prev = 0;
    for (int i=0;i<m_Len;i++){
      if (!(isalnum(m_Str[i])) && (m_Str[i]>0)){
	if (i>prev) Tmp+=Copy(prev, i-prev);
	prev = i+1;
	Tmp.Appendc2x(m_Str[i], '%');
      }
    }
    Tmp+=Copy(prev, m_Len-prev);
    (* this) = Tmp;
  }
  return (* this);
}

CString& CString::unescape_url(void) {
  DBGWRITE(cout << "CString::unescape_url()" << endl);
  if (m_Len){
    int x,y;
    for(x=0,y=0;m_Str[y];++x,++y) {
      if(((m_Str[x] = m_Str[y]) == '%')||(m_Str[x]=='`')) {
	m_Str[x] = x2c(&m_Str[y+1]);
	y+=2;
	if (m_Str[x]=='`') {
	  m_Str[x] = x2c(&m_Str[y+1]);
	  y+=2;
	}
      }
    }
    m_Str[x] = '\0';
    m_Len = x;
  }
  return(*this);
}

CString& CString::unescape(void) {
  DBGWRITE(cout << "CString::unescape()" << endl);
  if (m_Len){
    int x,y;
    for(x=0,y=0;m_Str[y];++x,++y) {
      if(((m_Str[x] = m_Str[y]) == '%')) {
        m_Str[x] = x2c(&m_Str[y+1]);
        y+=2;
      }
    }
    m_Str[x] = '\0';
    m_Len = x;
  }
  return(*this);
}

void CString::untreat_stupid(void) {
  DBGWRITE(cout << "CString::untrea_stupid()" << endl);
  if (m_Len){
    int j=0;
    char * result = new char[m_Len+1];
    for (int i=0;i<m_Len;i++)
      if ((m_Str[i]!='\\')||(m_Str[i+1]!='~')) result[j++]=m_Str[i];
    result[j]=0;
    Assign(result, j, m_Len + 1);
  }
}

CString& CString::StrInsert(int pos, const CString& ins){
  DBGWRITE(cout << "CString::StrInsert(" << pos << ")[" << ins << "]" << endl);
  if ((pos <= 0) || (!m_Len)) StrAppend(ins.m_Str, ins.m_Len);
  else if (ins.m_Len) {
    SetSize(m_Len + ins.m_Len + 1);
    
    // memmove(m_Str+pos+ins.m_Len, m_Str + pos, m_Len - pos + 1);
    for (register int i=m_Len - 1;i>=pos;i--)
      m_Str[i + ins.m_Len] = m_Str[i];      
    // memcpy(m_Str+pos, ins.m_Str, ins.m_Len);
    for (register int j=0;j<ins.m_Len;j++)
      m_Str[pos + j] = ins.m_Str[j]; 
    m_Len = m_Len + ins.m_Len;      
  } 
  return (* this);
}

/*
  //#define CString_friend_operator { CString * tmp = new CString(s); (* tmp)+=t; CStringGarbageCollect+=tmp; return(* tmp); }
  
  #define CString_friend_operator { CString tmp = s; s+=t; return s; }
  
  CString operator+(CString& s, CString& t) CString_friend_operator
  CString operator+(CString& s, char * t) CString_friend_operator
  CString operator+(CString& s, int t) CString_friend_operator
  CString operator+(CString& s, double t) CString_friend_operator
  CString operator+(CString& s, float t) CString_friend_operator
  //CString operator+(char * s, CString& t) CString_friend_operator
  CString operator+(CString& s, long t) CString_friend_operator
  CString operator+(int t, CString& s) CString_friend_operator
  CString operator+(double t, CString& s) CString_friend_operator
  CString operator+(float t, CString& s) CString_friend_operator
  CString operator+(long t, CString& s) CString_friend_operator
  */

void CStringQSortInternal(CVector<CString> & vector,  int (*cmp_function) (const CString&, const CString&), int l, int r){  
    int i = l;
    int j = r;
    CString pivot = vector[(l+r)/2]; 
    while (i<=j) {
        while (((* cmp_function)(vector[i], pivot) < 0) &&  (i <= r))
            i++;
        while (((* cmp_function)(vector[j], pivot) > 0) && (j >= l)) 
            j--;
        if (i<=j){
            CString t = vector[i];
            vector[i] = vector[j];
            vector[j] = t;
            i++;
            j--;
        }
    }
    if (l < j)
        CStringQSortInternal(vector, cmp_function, l, j);
    if (l < r)
        CStringQSortInternal(vector, cmp_function, i, r);
}

void CStringQSort(CVector<CString> & vector,  int (*cmp_function) (const CString&, const CString&)){
    if (vector.Count() > 1)
        CStringQSortInternal(vector, cmp_function, 0, vector.Count() - 1);
}

CString bool_to_space(int bvalue){
  if (bvalue) return(" "); else return("");
}

#define ISSPACE(x) ((x == ' ')||(x == '\n')||(x == '\r')||(x == '\t'))
#define ISNOTSPACE(x) ((x != ' ')&&(x != '\n')&&(x != '\r')||(x != '\t'))

CString& CString::ContractSpaces(void){
  DBGWRITE(cout << "CString::ContractSpaces()" << endl);
  if (m_Len > 1) {
    char * target = new char[m_Len+1];
    int i,j;
    for (i=0,j=0;i<m_Len-1;i++) {
	if (ISNOTSPACE(m_Str[i])) target[j++] = m_Str[i];
	else if (ISNOTSPACE(m_Str[i+1])) target[j++] = ' '; 
    }
    target[j] = 0;
    Assign(target, j, m_Len + 1);
  }
  return (*this);
}

CString& CString::Dequote(void) {
  CString Result;
  int curPos = 0;
  int prevPos = 0;  
  while (curPos < m_Len) {    
    if (GetChar(curPos) == '&') {
      Result += Copy(prevPos, curPos - prevPos);      
      prevPos = ++curPos;     
      while ((curPos < m_Len)&&(isalnum(GetChar(curPos)))) curPos++;
      if (GetChar(curPos) != ';') {
	Result += '&';
	curPos = prevPos;
	continue;
      }
      CString Token = Copy(prevPos, curPos - prevPos);      
      prevPos = ++curPos;
      if (Token[0] == '#') {
	CString Val(Token.Copy(1, StrLength()));
	if (Val.StrLength()) {
	  if ((Val[0] == 'X')||(Val[0]=='x')) {
	    Val.StrDelete(0, 1);	  
	  } else {
	    int iVal = Val.Val();
	    if (iVal == 146) Result+='\'';
	    else Result+=(char)(iVal%128);
	  }
	}
      } else if (Token.isSame("nbsp")) Result+=" ";
      else if (Token.isSame("agrave")) Result+="à";
      else if (Token.isSame("aacute")) Result+="á";
      else if (Token.isSame("acirc")) Result+="â";
      else if (Token.isSame("atilde")) Result+="ã";
      else if (Token.isSame("auml")) Result+="ä";
      else if (Token.isSame("aring")) Result+="å";
      else if (Token.isSame("aelig")) Result+="æ";
      else if (Token.isSame("ccedil")) Result+="ç";
      else if (Token.isSame("egrave")) Result+="è";
      else if (Token.isSame("eacute")) Result+="é";
      else if (Token.isSame("ecirc")) Result+="ê";
      else if (Token.isSame("euml")) Result+="ë";
      else if (Token.isSame("igrave")) Result+="ì";
      else if (Token.isSame("iacute")) Result+="í";
      else if (Token.isSame("icirc")) Result+="î";
      else if (Token.isSame("iuml")) Result+="ï";
      else if (Token.isSame("eth")) Result+="ð";
      else if (Token.isSame("ntilde")) Result+="ñ";
      else if (Token.isSame("ograve")) Result+="ò";
      else if (Token.isSame("oacute")) Result+="ó";
      else if (Token.isSame("ocirc")) Result+="ô";
      else if (Token.isSame("otilde")) Result+="õ";
      else if (Token.isSame("ouml")) Result+="ö";
      else if (Token.isSame("divide")) Result+="÷";
      else if (Token.isSame("oslash")) Result+="ø";
      else if (Token.isSame("ugrave")) Result+="ù";
      else if (Token.isSame("uacute")) Result+="ú";
      else if (Token.isSame("ucirc")) Result+="û";
      else if (Token.isSame("uuml")) Result+="ü";
      else if (Token.isSame("yacute")) Result+="ý";
      else if (Token.isSame("thorn")) Result+="þ";
      else if (Token.isSame("yuml")) Result+="ÿ";
      else if (Token.isSame("Agrave")) Result+="À"; 
      else if (Token.isSame("Aacute")) Result+="Á";
      else if (Token.isSame("Acirc")) Result+="Â";
      else if (Token.isSame("Atilde")) Result+="Ã";
      else if (Token.isSame("Auml")) Result+="Ä";
      else if (Token.isSame("Aring")) Result+="Å";
      else if (Token.isSame("AElig")) Result+="Æ";
      else if (Token.isSame("Ccedil")) Result+="Ç";
      else if (Token.isSame("Egrave")) Result+="È";
      else if (Token.isSame("Eacute")) Result+="É";
      else if (Token.isSame("Ecirc")) Result+="Ê";
      else if (Token.isSame("Euml")) Result+="Ë";
      else if (Token.isSame("Igrave")) Result+="Ì";
      else if (Token.isSame("Iacute")) Result+="Í";
      else if (Token.isSame("Icirc")) Result+="Î";
      else if (Token.isSame("Iuml")) Result+="Ï";
      else if (Token.isSame("ETH")) Result+="Ð";
      else if (Token.isSame("Ntilde")) Result+="Ñ";
      else if (Token.isSame("Ograve")) Result+="Ò";
      else if (Token.isSame("Oacute")) Result+="Ó";
      else if (Token.isSame("Ocirc")) Result+="Ô";
      else if (Token.isSame("Otilde")) Result+="Õ";
      else if (Token.isSame("Ouml")) Result+="Ö";
      else if (Token.isSame("times")) Result+="×";
      else if (Token.isSame("Oslash")) Result+="Ø";
      else if (Token.isSame("Ugrave")) Result+="Ù";
      else if (Token.isSame("Uacute")) Result+="Ú";
      else if (Token.isSame("Ucirc")) Result+="Û";
      else if (Token.isSame("Uuml")) Result+="Ü";
      else if (Token.isSame("Yacute")) Result+="Ý";
      else if (Token.isSame("THORN")) Result+="Þ";
      else if (Token.isSame("szlig")) Result+="ß";    
      else if (Token.isSame("iexcl")) Result+="¡";
      else if (Token.isSame("cent")) Result+="¢";
      else if (Token.isSame("pound")) Result+="£";
      else if (Token.isSame("curren")) Result+="¤";
      else if (Token.isSame("yen")) Result+="¥";
      else if (Token.isSame("brvbar")) Result+="¦";
      else if (Token.isSame("sect")) Result+="§";
      else if (Token.isSame("uml")) Result+="¨";
      else if (Token.isSame("copy")) Result+="©";
      else if (Token.isSame("ordf")) Result+="ª";
      else if (Token.isSame("laquo")) Result+="«";
      else if (Token.isSame("not")) Result+="¬";
      else if (Token.isSame("shy")) Result+="­";
      else if (Token.isSame("reg")) Result+="®";
      else if (Token.isSame("macr")) Result+="¯";
      else if (Token.isSame("deg")) Result+="°";
      else if (Token.isSame("plusmn")) Result+="±";
      else if (Token.isSame("sup2")) Result+="²";
      else if (Token.isSame("sup3")) Result+="³";
      else if (Token.isSame("acute")) Result+="´";
      else if (Token.isSame("micro")) Result+="µ";
      else if (Token.isSame("para")) Result+="¶";
      else if (Token.isSame("middot")) Result+="·";
      else if (Token.isSame("cedil")) Result+="¸";
      else if (Token.isSame("sup1")) Result+="¹";
      else if (Token.isSame("ordm")) Result+="º";
      else if (Token.isSame("raquo")) Result+="»";
      else if (Token.isSame("frac14")) Result+="¼";
      else if (Token.isSame("frac12")) Result+="½";
      else if (Token.isSame("frac34")) Result+="¾";
      else if (Token.isSame("iquest")) Result+="¿";        
      else if (Token.isSame("quot")) Result+="\"";
      else if (Token.isSame("gt")) Result+=">";
      else if (Token.isSame("lt")) Result+="<";
      else if (Token.isSame("amp")) Result+="&";
      else {
	Result+="&";
	Result+=Token;
	Result+=";";
      }    
    } else curPos++;
  }
  Result += Copy(prevPos, curPos - prevPos);
  (* this) = Result;
  return (* this);
}

CString& CString::Quote(void) {  
  CString Result;
  for (int i=0;i < m_Len; i++) 
    switch(m_Str[i]) {
    case 'à': Result += "&agrave;"; break;
    case 'á': Result += "&aacute;"; break;
    case 'â': Result += "&acirc;"; break;
    case 'ã': Result += "&atilde;"; break;
    case 'ä': Result += "&auml;"; break;
    case 'å': Result += "&aring;"; break;
    case 'æ': Result += "&aelig;"; break;
    case 'ç': Result += "&ccedil;"; break;
    case 'è': Result += "&egrave;"; break;
    case 'é': Result += "&eacute;"; break;    
    case 'ê': Result += "&ecirc;"; break;
    case 'ë': Result += "&euml;"; break;
    case 'ì': Result += "&igrave;"; break;
    case 'í': Result += "&iacute;"; break;
    case 'î': Result += "&icirc;"; break;
    case 'ï': Result += "&iuml;"; break;
    case 'ð': Result += "&eth;"; break;
    case 'ñ': Result += "&ntilde;"; break;
    case 'ò': Result += "&ograve;"; break;
    case 'ó': Result += "&oacute;"; break;
    case 'ô': Result += "&ocirc;"; break;
    case 'õ': Result += "&otilde;"; break;
    case 'ö': Result += "&ouml;"; break;
    case '÷': Result += "&divide;"; break;
    case 'ø': Result += "&oslash;"; break;
    case 'ù': Result += "&ugrave;"; break;
    case 'ú': Result += "&uacute;"; break;
    case 'û': Result += "&ucirc;"; break;
    case 'ü': Result += "&uuml;"; break;
    case 'ý': Result += "&yacute;"; break;
    case 'þ': Result += "&thorn;"; break;
    case 'ÿ': Result += "&yuml;"; break;
    case 'À': Result += "&Agrave;"; break;
    case 'Á': Result += "&Aacute;"; break;
    case 'Â': Result += "&Acirc;"; break;
    case 'Ã': Result += "&Atilde;"; break;
    case 'Ä': Result += "&Auml;"; break;
    case 'Å': Result += "&Aring;"; break;
    case 'Æ': Result += "&AElig;"; break;
    case 'Ç': Result += "&Ccedil;"; break;
    case 'È': Result += "&Egrave;"; break;
    case 'É': Result += "&Eacute;"; break;
    case 'Ê': Result += "&Ecirc;"; break;
    case 'Ë': Result += "&Euml;"; break;
    case 'Ì': Result += "&Igrave;"; break;
    case 'Í': Result += "&Iacute;"; break;
    case 'Î': Result += "&Icirc;"; break;
    case 'Ï': Result += "&Iuml;"; break;
    case 'Ð': Result += "&ETH;"; break;
    case 'Ñ': Result += "&Ntilde;"; break;
    case 'Ò': Result += "&Ograve;"; break;
    case 'Ó': Result += "&Oacute;"; break;
    case 'Ô': Result += "&Ocirc;"; break;
    case 'Õ': Result += "&Otilde;"; break;
    case 'Ö': Result += "&Ouml;"; break;
    case '×': Result += "&times;"; break;
    case 'Ø': Result += "&Oslash;"; break;
    case 'Ù': Result += "&Ugrave;"; break;
    case 'Ú': Result += "&Uacute;"; break;
    case 'Û': Result += "&Ucirc;"; break;
    case 'Ü': Result += "&Uuml;"; break;
    case 'Ý': Result += "&Yacute;"; break;
    case 'Þ': Result += "&THORN;"; break;
    case 'ß': Result += "&szlig;"; break;
    case '¡': Result += "&iexcl;"; break;
    case '¢': Result += "&cent;"; break;
    case '£': Result += "&pound;"; break;
    case '¤': Result += "&curren;"; break;
    case '¥': Result += "&yen;"; break;
    case '¦': Result += "&brvbar;"; break;
    case '§': Result += "&sect;"; break;
    case '¨': Result += "&uml;"; break;
    case '©': Result += "&copy;"; break;
    case 'ª': Result += "&ordf;"; break;
    case '«': Result += "&laquo;"; break;
    case '¬': Result += "&not;"; break;
    case '­': Result += "&shy;"; break;
    case '®': Result += "&reg;"; break;
    case '¯': Result += "&macr;"; break;
    case '°': Result += "&deg;"; break;
    case '±': Result += "&plusmn;"; break;
    case '²': Result += "&sup2;"; break;
    case '³': Result += "&sup3;"; break;
    case '´': Result += "&acute;"; break;
    case 'µ': Result += "&micro;"; break;
    case '¶': Result += "&para;"; break;
    case '·': Result += "&middot;"; break;
    case '¸': Result += "&cedil;"; break;
    case '¹': Result += "&sup1;"; break;
    case 'º': Result += "&ordm;"; break;
    case '»': Result += "&raquo;"; break;
    case '¼': Result += "&frac14;"; break;
    case '½': Result += "&frac12;"; break;
    case '¾': Result += "&frac34;"; break;
    case '¿': Result += "&iquest;"; break;
    case '\"': Result += "&quot;"; break;
    case '>': Result += "&gt;"; break;
    case '<': Result += "&lt;"; break;
    case '&': Result += "&amp;"; break;
    default: Result += m_Str[i]; break;
    }   
  (* this) = Result;  
  return (* this);
}

static signed char alphabet[] = {
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',   //  0 to  7
  'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',   //  8 to 15
  'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',   // 16 to 23
  'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',   // 24 to 31
  'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',   // 32 to 39
  'o', 'p', 'q', 'r', 's', 't', 'u', 'v',   // 40 to 47
  'w', 'x', 'y', 'z', '0', '1', '2', '3',   // 48 to 55
  '4', '5', '6', '7', '8', '9', '+', '/' }; // 56 to 63

CString& CString::FormatC(void) {
	if (StrLength() < 2) return * this;
	char l = GetChar(0), r;
	for (register int i=1;i<StrLength();i++) {
		r = GetChar(i);
		if (l == '\\') {
			switch(r) {
			case 'n':
				m_Str[i-1] = 10;
				m_Str[i] = 13;
				break;
			}
		}
		l = r;
	}
	return * this;
}

CString& CString::Base64Encode(void) {
  int  bits24;
  int  bits6;  
  int totSize = ((StrLength() - 1 )/3+1)*4 + 1;
  char * out = new char [totSize];  
  int outIndex = 0;
  int i = 0;
  while ((i+3) <= StrLength()) {
    // store the octets
    bits24  = ( (* this) [ i++ ] & 0xFF ) << 16; 
    bits24 |= ( (* this) [ i++ ] & 0xFF ) <<  8; 
    bits24 |= ( (* this) [ i++ ] & 0xFF ) <<  0;
    bits6 = ( bits24 & 0x00FC0000 ) >> 18; 
    out [ outIndex++ ] = alphabet [ bits6 ];
    bits6 = ( bits24 & 0x0003F000 ) >> 12; 
    out [ outIndex++ ] = alphabet [ bits6 ];
    bits6 = ( bits24 & 0x00000FC0 ) >> 6; 
    out [ outIndex++ ] = alphabet [ bits6 ];
    bits6 = ( bits24 & 0x0000003F );
    out [ outIndex++ ] = alphabet [ bits6 ]; 
  }

  if ( StrLength() - i == 2 ) {
    // store the octets 
    bits24  = ( (* this) [ i     ] & 0xFF ) << 16; 
    bits24 |= ( (* this) [ i + 1 ] & 0xFF ) <<  8;
    
    bits6 = ( bits24 & 0x00FC0000 ) >> 18;
    out [ outIndex++ ] = alphabet [ bits6 ]; 
    bits6 = ( bits24 & 0x0003F000 ) >> 12; 
    out [ outIndex++ ] = alphabet [ bits6 ]; 
    bits6 = ( bits24 & 0x00000FC0 ) >> 6; 
    out [ outIndex++ ] = alphabet [ bits6 ];
    
    // padding
    out [ outIndex++ ] = '='; 
  } else if ( StrLength() - i == 1 ) {
    // store the octets 
    bits24 = ( (* this) [ i ] & 0xFF ) << 16;
    
    bits6 = ( bits24 & 0x00FC0000 ) >> 18;
    out [ outIndex++ ] = alphabet [ bits6 ];
    bits6 = ( bits24 & 0x0003F000 ) >> 12; 
    out [ outIndex++ ] = alphabet [ bits6 ];
    
    // padding
    out [ outIndex++ ] = '='; 
    out [ outIndex++ ] = '='; 
  }  
  out[ outIndex] = 0;
  Assign(out, outIndex, totSize);
  return * this;
}

const signed char __pPlain[128]={
  /*   0.. 15 */ -1, -1, -1, -1, -1, -1, -1 ,-1, -1, -1, -1, -1, -1, -1, -1, -1,
  /*  16.. 31 */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
  /*  32.. 47 */ -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
  /*  48.. 63 */ 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1,  0, -1, -1,
  /*  64.. 79 */ -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
  /*  80.. 95 */ 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
  /*  96..111 */ -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
  /* 112..127 */ 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1
  };

CString& CString::Base64Decode(void) {
  enum { START, TWO, FOUR, SIX } eState;
  eState = START;  
  CString Result;
  int iLast = 0, iPlain = 0;
  char c = 0;
  for (int i=0;i<StrLength();i++) {
    c = GetChar(i);
    if ( !isalnum(c) && (c!='+') && (c!='/') ) { break; };
    iPlain = __pPlain[c];
    assert( iPlain!= -1 );
    switch( eState ) {
    case START:
      iLast = ( iPlain << 2 ) & 0xFC;
      eState = TWO;
      break;      
    case TWO:
      Result += (char) ( iLast | ( (iPlain >> 4 ) & 0x03) );
      iLast = ( iPlain << 4 ) & 0xF0;
      eState = FOUR;
      break;				
    case FOUR:
      Result += (char) ( iLast | ( (iPlain >> 2 ) & 0x0F) );
      iLast = ( iPlain << 6 ) & 0xC0;
      eState = SIX;
      break;				
    case SIX:
      Result += (char) ( iLast | ( iPlain & 0x3F) );
      eState = START; 
      break;      
    default:;
    };
    if ( c=='=') { break; };
  };
  (* this) = Result;
  return (* this);
}

void CString::SetBuf(const char * Buf, int Len) {
  if (Len < 0)
    Len = (Buf ? strlen(Buf) : 0);
  if (Len > 0) {
    SetSize(Len + 1);
    memcpy(m_Str, Buf, Len + 1);
    m_Len = Len;
  }    
}

CString& CString::remove_forward(char ch) {
    if (m_Len && (m_Str[m_Len - 1] == ch)) {
        m_Len--;
        m_Str[m_Len] = 0;
    }
    return * this;
}
