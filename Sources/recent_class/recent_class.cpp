#include <agnespch.hpp>

recent_manager::recent_manager(ext_param * _template_options){
	template_options = _template_options;
}

recent_manager::~recent_manager(void){
	
}

int recent_manager::how_old(const CString& article_date, int TimeShift){
	if (!(article_date.StrLength())) return(0);

	struct tm ArticleTime = encode_date_article(article_date);
	ArticleTime.tm_mon--;
	time_t tclock = mktime(&ArticleTime);
	if (TimeShift != 0) tclock += (TimeShift * 3600);	
	memcpy(&ArticleTime, localtime(&tclock), sizeof(ArticleTime));

	time_t aclock; time( &aclock ); if (TimeShift != 0) aclock += (TimeShift * 3600);
	struct tm * MachineTime = localtime( &aclock );
	return (abs(- day_count(ArticleTime.tm_mon + 1, ArticleTime.tm_mday, ArticleTime.tm_year) +
		    day_count(MachineTime->tm_mon + 1, MachineTime->tm_mday, MachineTime->tm_year)));
}

int recent_manager::how_old_hours(const CString& article_date){
	if (!(article_date.StrLength())) return(0);
	struct tm *newtime; time_t aclock; time( &aclock ); newtime = localtime( &aclock );
	struct tm thisdate = encode_date_article(article_date);
	int day_diff = abs(- day_count(thisdate.tm_mon, thisdate.tm_mday, thisdate.tm_year) +
			   day_count(newtime->tm_mon + 1, newtime->tm_mday, newtime->tm_year));
	int hour_diff = newtime->tm_hour - thisdate.tm_hour;
	if (hour_diff < 0) {day_diff--; hour_diff+=24;}
	return(day_diff * 24 + hour_diff); 
}

long recent_manager::how_old_minutes(const CString& article_date){
	if (!(article_date.StrLength())) return(0);
	struct tm *newtime; time_t aclock; time( &aclock ); newtime = localtime( &aclock );
	struct tm thisdate = encode_date_article(article_date);
	int day_diff = abs(- day_count(thisdate.tm_mon, thisdate.tm_mday, thisdate.tm_year) +
			   day_count(newtime->tm_mon + 1, newtime->tm_mday, newtime->tm_year));
	int hour_diff = newtime->tm_hour - thisdate.tm_hour;
	if (hour_diff < 0) {day_diff--; hour_diff+=24;}
	hour_diff = day_diff * 24 + hour_diff;
  
	long min_diff = newtime->tm_min - thisdate.tm_min;
	if (min_diff < 0) {hour_diff--; min_diff+=60;}
	min_diff = hour_diff * 60 + min_diff;
  
	return(min_diff);
}

long recent_manager::how_old_seconds(const CString& article_date){
	if (!(article_date.StrLength())) return(0);
	struct tm *newtime; time_t aclock; time( &aclock ); newtime = localtime( &aclock );
	struct tm thisdate = encode_date_article(article_date);
	int day_diff = abs(- day_count(thisdate.tm_mon, thisdate.tm_mday, thisdate.tm_year) +
			   day_count(newtime->tm_mon + 1, newtime->tm_mday, newtime->tm_year));
	int hour_diff = newtime->tm_hour - thisdate.tm_hour;
	if (hour_diff < 0) {day_diff--; hour_diff+=24;}
	hour_diff = day_diff * 24 + hour_diff;

	long min_diff = newtime->tm_min - thisdate.tm_min;
	if (min_diff < 0) {hour_diff--; min_diff+=60;}
	min_diff = hour_diff * 60 + min_diff;
  
	long sec_diff = newtime->tm_sec - thisdate.tm_sec;
	if (sec_diff < 0) {min_diff--; sec_diff+=60;}
	sec_diff = min_diff * 60 + sec_diff;
  
	return(sec_diff);
}


void recent_manager::article_recent(cgiOutStream& CGIStream, const CString& article_date){
	int is_old = how_old(article_date, template_options->get_value("TIMESHIFT").Val());
	if (is_old <= template_options->get_value("DAYS").Val())
		CGIStream << " " << template_options->get_value("RECENT") << " ";	
	CString perday; perday+=is_old;
	if (template_options->get_value("PERDAY",perday).StrLength()) CGIStream << template_options->get_value("PERDAY",perday) << " ";
}

struct tm recent_manager::encode_date_article(const CString& date){
	assert(date.StrLength());
	char sep, weekday[10], month[10], hour[10];	
	char * months[] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
	char * weekdays[] = {"Sun", "Mon","Tue","Wed","Thu","Fri","Sat"};
	int year;
	struct tm thisdate;

	sscanf(date.asString(), "%s%s%d%s%d", weekday, month, &thisdate.tm_mday, hour, &year);
	
	for (int i=0;i<12;i++) if (!strcmpi(month, months[i])) {thisdate.tm_mon = i + 1; break;}
	for (int j=0;j<7;j++) if (!strcmpi(weekday, weekdays[j])) {thisdate.tm_wday = j + 1; break;}
	thisdate.tm_year = year - 1900;
	sscanf(hour, "%d%c%d%c%d", &thisdate.tm_hour, &sep, &thisdate.tm_min, &sep, &thisdate.tm_sec);
	thisdate.tm_isdst = -1;
	return(thisdate);
}

struct tm recent_manager::encode_date(const CString& date_string){
	int day;
	int month;
	int year;
	struct tm when; time_t now;	time( &now ); when = *localtime( &now );	
	
	char sep = 0;
	if (date_string.Pos('.') >=0 ) sep = '.';
	else if (date_string.Pos('/') >= 0) sep = '/';
	else if (date_string.Pos('-') >= 0) sep = '-';  
  
	CVector<CString> Tokens; date_string.Tokenizer(sep, Tokens);
	assert(Tokens.Count()==3);
	year = Tokens[2].Val();
	month = Tokens[1].Val();
	day = Tokens[0].Val();
	
	if ((year>0)&&(year < 100)&&(year >= when.tm_year)) year = year + 1900;
	else if ((year>0)&&(year < 100)&&(year<when.tm_year)) year = year + 2000;	
	else assert((year>=0)&&(year<=100));  

	assert((month<=12)&&(month>=1));  
	day = date_string.Val();
	assert((day <= 31)&&(day >= 1));
  	
	struct tm this_day;
	this_day.tm_mday = day;
	this_day.tm_year = year;
	this_day.tm_mon = month;
	return(this_day);
}

