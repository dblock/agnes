#ifndef recent_manager_h
#define recent_manager_h

#include <time.h>
#include <cstring/cstring.h>
#include <ext_param/ext_param.h>

class recent_manager{
 private:
	ext_param * template_options;
	struct tm encode_date_article(const CString& date);
	static inline long int day_count (int m, int d, int y) { return 1461 *  funct1(y,m) / 4 + 153 * funct2(m) / 5 + d; }
	static inline long int funct2 (int m);
	static inline long int funct1 (int, int);
 public:
	struct tm encode_date(const CString& date_string);
	int how_old(const CString& article_date, int TimeShift = 0);
	int how_old_hours(const CString& article_date);
	long how_old_minutes(const CString& article_date);
	long how_old_seconds(const CString& article_date);
	void article_recent(cgiOutStream& CGIStream, const CString& article_date);
	recent_manager(ext_param * _template_options);
	~recent_manager(void);
};


inline long int recent_manager::funct1 (int y, int m) {
	long int result;
	if (m<=2) y -= 1;
	result = y;
	return (result);
}

inline long int recent_manager::funct2 (int m) {
	long int result;
	if ( m <= 2 ) result=m+13;
	else result = m + 1;
	return(result);
}


#endif
