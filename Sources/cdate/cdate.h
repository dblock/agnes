/* 
   CDate.h
   */

#ifndef C_DATE_H
#define C_DATE_H

#include <time.h>
#include "../cstring/cstring.h"

class CDate {
private:
  time_t time_t_time;
  struct tm * tm_time; 
  CString& MapTerm(const CString& Source, CString& Target) const;
public:
  CDate(struct tm *);  
  const char * AscTime(void) const;
  CDate(void);
  CString& Map(const CString& Expression, CString& Target) const;
  CString& Year(CString& Target) const;
  CString& Month(CString& Target) const;
  CString& Day(CString& Target) const;  
  CString Map(const CString&) const;
  void GetVariable(CString&) const;
  const char * DayEnglish(int) const;
  const char * DayEnglishShort(int) const;
  const char * DayFrenchShort(int) const;
  const char * MonthEnglish(int) const;
  const char * MonthEnglishShort(int) const;
  const char * MonthFrenchShort(int) const;
  const char * DayFrench(int) const;
  const char * DayFrench(void) const;
  const char * MonthFrench(int) const;
  const char * MonthFrench(void) const;
  int DayOfWeek (int year, int month, int day) const;
  const char * DayEnglish(void) const;
  const char * DayEnglishShort(void) const;
  const char * DayFrenchShort(void) const;
  const char * MonthEnglish(void) const;
  const char * MonthEnglishShort(void) const;
  const char * MonthFrenchShort(void) const;
  int DayOfWeek(void) const;
};

#endif
