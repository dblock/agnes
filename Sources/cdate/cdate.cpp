#include <agnespch.hpp>

const char * AMonthsFrench[] = { "Janvier", "Fevrier", "Mars", "Avril", 
								"Mai", "Juin", "Juillet", "Août","Septembre","Octobre",
								"Novembre", "Décembre" };

const char * AMonthsEnglish[] = {"January","February","March","April",
								 "May","June","July","August","September","October",
								 "November","December"};

  
const char * ADaysFrench[] = {	"Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi", "Samedi", "Dimanche" };
const char * ADaysFrenchShort[] = { "Lun", "Mar", "Mer", "Jeu", "Ven", "Sam", "Dim" };
const char * ADaysEnglish[] = {	"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday" };
const char * ADaysEnglishShort[] = {"Mon","Tue","Wed","Thu","Fri","Sat","Sun"};
const char * AMonthsEnglishShort[] = {"Jan","Feb","Mar","Apr", "May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
const char * AMonthsFrenchShort[] = {"Jan","Fev","Mar","Avr","Mai","Jun","Jul","Aoû","Sep","Oct","Nov","Déc"};

const char * CDate::DayFrench(void) const {
  return DayFrench(DayOfWeek());
}

const char * CDate::MonthFrench(void) const {
  return MonthFrench(tm_time->tm_mon);
}

const char * CDate::AscTime(void) const {
  return asctime(tm_time);
}

const char * CDate::DayFrench(int Day) const {
  assert(Day < 7);
  return ADaysFrench[Day];  
}

const char * CDate::MonthFrench(int Month) const {
  assert(Month < 12);
  return AMonthsFrench[Month];
}

const char * CDate::DayEnglishShort(int Day) const {
  assert(Day < 7);
  return ADaysEnglishShort[Day];
}

const char * CDate::DayFrenchShort(int Day) const {
  assert(Day < 7);
  return ADaysFrenchShort[Day];
}

const char * CDate::DayEnglish(int Day) const {
	assert(Day < 7);
	return ADaysEnglish[Day];  
}

const char * CDate::MonthEnglish(int Month) const {
  assert(Month < 12);
  return AMonthsEnglish[Month];
}

const char * CDate::MonthEnglishShort(int Month) const {
  assert(Month < 12);
  return AMonthsEnglishShort[Month];
}

const char * CDate::MonthFrenchShort(int Month) const {
  assert(Month < 12);
  return AMonthsFrenchShort[Month];
}

int CDate::DayOfWeek (int year, int month, int day) const {
  static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
  year -= month < 3;
  return (year + year/4 - year/100 + year/400 + t[month-1] + day)%7;
}

CDate::CDate(struct tm * NewTime){  
  tm_time = NewTime;  
}

CDate::CDate(void){
  time(&time_t_time); 
  tm_time = localtime(&time_t_time);
}

const char * CDate::DayEnglish(void) const {
  return DayEnglish(DayOfWeek());
}

const char * CDate::DayEnglishShort(void) const {
  return DayEnglishShort(DayOfWeek());
}

const char * CDate::DayFrenchShort(void) const {
  return DayFrenchShort(DayOfWeek());
}

const char * CDate::MonthEnglish(void) const {
  return MonthEnglish(tm_time->tm_mon);
}

const char * CDate::MonthEnglishShort(void) const {
  return MonthEnglishShort(tm_time->tm_mon);
}

const char * CDate::MonthFrenchShort(void) const {
  return MonthFrenchShort(tm_time->tm_mon);
}

int CDate::DayOfWeek(void) const {
  return DayOfWeek(tm_time->tm_year, tm_time->tm_mon+1, tm_time->tm_mday);
}

CString& CDate::Year(CString& Target) const {
  Target.Free();  
  Target+=(tm_time->tm_year+1900);
  return Target;
}

CString& CDate::Month(CString& Target) const {
  Target.Free();
  Target+=tm_time->tm_mon+1;
  return Target;
}

CString& CDate::Day(CString& Target) const {
  Target.Free();
  Target+=tm_time->tm_mday;
  return Target;
}

void CDate::GetVariable(CString& Term) const {
  if (Term == "DayEnglish") Term = DayEnglish();
  else if (Term == "DayEng") Term = DayEnglishShort();
  else if (Term == "DayFrench") Term = DayFrench();
  else if (Term == "DayFre") Term = DayFrenchShort();
  else if (Term == "MonthFrench") Term = MonthFrench();
  else if (Term == "MonthEnglish") Term = MonthEnglish();
  else if (Term == "MonthEng") Term = MonthEnglishShort();
  else if (Term == "MonthFre") Term = MonthFrenchShort();  
  else if (Term == "Year") Year(Term);
  else if (Term == "Day") Day(Term);
  else if (Term == "Month") Month(Term);
  else if (Term == "Hour") {char tmp[3]; sprintf(tmp, "%02d", tm_time->tm_hour); Term = tmp; }
  else if (Term == "Min") {char tmp[3]; sprintf(tmp, "%02d", tm_time->tm_min); Term = tmp; }
  else if (Term == "Sec") {char tmp[3]; sprintf(tmp, "%02d", tm_time->tm_sec); Term = tmp; }
}

CString CDate::Map(const CString& Expression) const {
  CString Tmp;
  Map(Expression, Tmp);
  return Tmp;
}

CString& CDate::Map(const CString& Expression, CString& Target) const {
  MapTerm(Expression, Target);  
  return Target;
}

CString& CDate::MapTerm(const CString& Source, CString& Target) const {
  CString Term;
  int curPos = 0, prevPos = 0;
  CString Prefix;
  CString Postfix;
  CString Elsefix;
  CString CmdFix;
  int Regular, TagChar, IdChar;
  while (curPos < Source.StrLength()){
    if (Source[curPos] == '\\') {
      Target+=Source.Copy(prevPos, curPos-prevPos);
      curPos++; 
      Target+=Source[curPos];
      curPos++;
      prevPos=curPos;
      curPos++;
    } else if ((Source[curPos] == '$')||(Source[curPos]=='£')) {
      IdChar = Source[curPos];
      Prefix.Free();
      Postfix.Free();
      Elsefix.Free();
      Target+=Source.Copy(prevPos, curPos-prevPos);
      curPos++;prevPos = curPos;
      while (isalnum(Source[curPos]) || (Source[curPos] == '_') || (Source[curPos] == '.')) curPos++;
      Term = Source.Copy(prevPos, curPos-prevPos);      
      if (Term.StrLength() && ((Term[Term.StrLength()-1] == '.')||(Term[Term.StrLength()-1] == '_'))) {
	curPos--;
	Term.StrDelete(Term.StrLength()-1, 1);
      }            
      prevPos = curPos;
      while ((Source[curPos] == '~')||(Source[curPos] == '#')||(Source[curPos] == '^')||(Source[curPos] == '|')) {
	TagChar = Source[curPos];
	if (Source[curPos+1] == '[') Regular=1; else Regular=0;
	curPos++;prevPos = curPos;
	if (Regular) {
	  prevPos++;
	  Regular++;
	  while (Source[curPos]) {
	    if ((Source[curPos] == '\\')&&(Source[curPos+1]!='\\')) curPos++;
	    curPos++;
	    if (Source[curPos] == '[') Regular++;
	    else {
	      if (Source[curPos] == ']') Regular--;
	      if (Regular == 1) break;
	    }
	  }
	  if (Source[curPos] == ']') curPos++;
	  if (TagChar == '~') MapTerm(Source.Copy(prevPos,curPos-prevPos-1), Prefix);
	  else if (TagChar == '#') MapTerm(Source.Copy(prevPos,curPos-prevPos-1), Postfix);
	  else if (TagChar == '^') MapTerm(Source.Copy(prevPos,curPos-prevPos-1), Elsefix);
	  else if (TagChar == '|') MapTerm(Source.Copy(prevPos,curPos-prevPos-1), CmdFix);
	} else {
	  while ((Source[curPos]>' ')&&(Source[curPos]!='#')&&(Source[curPos]!='~')&&(Source[curPos]!='^')&&(Source[curPos]!='|')) curPos++;
	  if (TagChar == '~') MapTerm(Source.Copy(prevPos,curPos-prevPos), Prefix);
	  else if (TagChar == '#') MapTerm(Source.Copy(prevPos,curPos-prevPos), Postfix);
	  else if (TagChar == '^') MapTerm(Source.Copy(prevPos,curPos-prevPos), Elsefix);
	  else if (TagChar == '|') MapTerm(Source.Copy(prevPos,curPos-prevPos), CmdFix);
	}
	prevPos = curPos;
      }
      
      /*
	data mapping
	*/

      GetVariable(Term);
      
      /*
	cmd mapping
	*/
      
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
  Target+=Source.Copy(prevPos, Source.StrLength());
  return Target;
}

