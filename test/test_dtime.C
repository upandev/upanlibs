#include <stdio.h>
#include <time.h>

#define SECONDS_IN_NON_LEAP_YEAR 31536000L
#define SECONDS_IN_LEAP_YEAR 31622400L
#define SECONDS_IN_DAY 86400L

static const int DAYS_IN_A_MONTH[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

struct tm* custom_gmtime_r(const time_t *timep, struct tm *result) {
  if (result == NULL || timep == NULL) {
    return NULL;
  }

  time_t t = *timep;
  int year = 1970;
  int totalDays = 0;
  const bool negativeTime = t < 0;

  if (negativeTime) {
    t = -t;
    year = 1969;
  }

  while (true) {
    bool isLeapYear = year % 4 == 0 ? true : false;
    if ((isLeapYear && t < SECONDS_IN_LEAP_YEAR) || (!isLeapYear && t < SECONDS_IN_NON_LEAP_YEAR)) {
      break;
    }
    t -= isLeapYear ? SECONDS_IN_LEAP_YEAR : SECONDS_IN_NON_LEAP_YEAR;
    if (negativeTime) {
      --year;
    } else {
      ++year;
    }
    totalDays += isLeapYear ? 366 : 365;
  }

  const bool isLeapYear = year % 4 == 0 ? true : false;
  if (negativeTime) {
    t = (isLeapYear ? SECONDS_IN_LEAP_YEAR : SECONDS_IN_NON_LEAP_YEAR) - t;
  }

  int month = 0;
  int dayOfTheYear = 0;
  for(; month < 12; ++month) {
    int days = DAYS_IN_A_MONTH[month];
    if (isLeapYear && month == 1) {
      days++;
    }

    const long secondsInMonth = days * SECONDS_IN_DAY;
    if (t < secondsInMonth) {
      break;
    }
    t -= secondsInMonth;
    dayOfTheYear += days;
  }

  int dayOfTheMonth = 1;
  while (t >= SECONDS_IN_DAY) {
    t -= SECONDS_IN_DAY;
    dayOfTheMonth++;
    dayOfTheYear++;
  }
  totalDays += (negativeTime ? (isLeapYear ? 366 : 365) - dayOfTheYear : dayOfTheYear);

  int hour = t / 3600;
  int minute = (t % 3600) / 60;
  int second = t % 60;
  result->tm_sec = second;
  result->tm_min = minute;
  result->tm_hour = hour;
  result->tm_mday = dayOfTheMonth;
  result->tm_mon = month;
  result->tm_year = year - 1900;
  if (negativeTime) {
    int w = (4 - totalDays) % 7;
    result->tm_wday = (w < 0 ? w + 7 : w);
  } else {
    result->tm_wday = (totalDays + 4) % 7;
  }
  result->tm_yday = dayOfTheYear;
  return result;
}

static const char DAY_OF_WEEK[][30] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

void print(struct tm r) {
  printf("\n Actual: %02d/%02d/%d %02d:%02d:%02d (%s)", r.tm_mday, r.tm_mon + 1, r.tm_year + 1900, r.tm_hour, r.tm_min, r.tm_sec, DAY_OF_WEEK[r.tm_wday]);
}

int main() {
  struct tm r;
  time_t t;

  printf("\n");
  t = -1;
  custom_gmtime_r(&t, &r);
  printf("\n Expected: 31/12/1969 23:59:59");
  print(r);

  printf("\n");
  t = 0;
  custom_gmtime_r(&t, &r);
  printf("\n Expected: 01/01/1970 00:00:00");
  print(r);

  printf("\n");
  t = 86399;
  custom_gmtime_r(&t, &r);
  printf("\n Expected: 01/01/1970 23:59:59");
  print(r);

  printf("\n");
  t = 86400;
  custom_gmtime_r(&t, &r);
  printf("\n Expected: 02/01/1970 00:00:00");
  print(r);

  printf("\n");
  t = -86400;
  custom_gmtime_r(&t, &r);
  printf("\n Expected: 31/12/1969 00:00:00");
  print(r);

  printf("\n");
  t = 31535999;
  custom_gmtime_r(&t, &r);
  printf("\n Expected: 31/12/1970 23:59:59");
  print(r);

  printf("\n");
  t = 68169599;
  custom_gmtime_r(&t, &r);
  printf("\n Expected: 28/02/1972 23:59:59");
  print(r);

  printf("\n");
  t = 68169600;
  custom_gmtime_r(&t, &r);
  printf("\n Expected: 29/02/1972 00:00:00");
  print(r);


  printf("\n");
  t = 94694399;
  custom_gmtime_r(&t, &r);
  printf("\n Expected: 31/12/1972 23:59:59");
  print(r);

  printf("\n");
  t = -2203891200;
  custom_gmtime_r(&t, &r);
  printf("\n Expected: 01/03/1900 00:00:00");
  print(r);

  printf("\n");
  t = 951782400;
  custom_gmtime_r(&t, &r);
  printf("\n Expected: 29/02/2000 00:00:00");
  print(r);


  printf("\n");
  t = 2147483647;
  custom_gmtime_r(&t, &r);
  printf("\n Expected: 19/01/2038 03:14:07");
  print(r);

  printf("\n");
  t = 2147483648;
  custom_gmtime_r(&t, &r);
  printf("\n Expected: 19/01/2038 03:14:08");
  print(r);

  printf("\n");

  return 0;
}
