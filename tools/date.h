#ifndef _DATE_HEADER
#define _DATE_HEADER
#include <iostream>

using namespace std;

namespace tools {
class Date {
public:
  Date(){}
  Date(char* date);
  //operator ==(
private:
  int m_year;
  int m_month;
  int m_day;
  int m_hour;
  int m_minute;
  int m_second;
};
}
#endif //_DATE_HEADER