#include <am.h>
#include <nemu.h>
#include <stdint.h>

static long boot_time = 0;

void __am_timer_init() {}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  uint64_t current_time = inl(RTC_ADDR) | (uint64_t)inl(RTC_ADDR + 4) << 32;
  uptime->us = current_time - boot_time;
}

static inline bool is_leap_year(int year) {
  return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  uint64_t current_time_us =
      inl(RTC_ADDR) | ((uint64_t)inl(RTC_ADDR + 4) << 32);

  uint64_t total_seconds = current_time_us / 1000000;
  rtc->second = total_seconds % 60;

  uint64_t total_minutes = total_seconds / 60;
  rtc->minute = total_minutes % 60;

  uint64_t total_hours = total_minutes / 60;
  rtc->hour = total_hours % 24;

  uint64_t total_days = total_hours / 24;
  int year = 1970;

  while (total_days >= (is_leap_year(year) ? 366 : 365)) {
    total_days -= (is_leap_year(year) ? 366 : 365);
    year++;
  }
  rtc->year = year;

  int day_of_year = total_days;

  int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  if (is_leap_year(year)) {
    days_in_month[1] = 29;
  }

  rtc->month = 1;
  for (int i = 0; i < 12; i++) {
    if (day_of_year < days_in_month[i]) {
      rtc->day = day_of_year + 1;
      break;
    }
    day_of_year -= days_in_month[i];
    rtc->month++;
  }
}

// void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
//   uint64_t current_time = inl(RTC_ADDR) | (uint64_t)inl(RTC_ADDR + 4) << 32;
//   rtc->second = current_time % 60 / 60;
//   rtc->minute = (current_time / 60) % 60 / 60;
//   rtc->hour = (current_time / 3600) % 24;
//
//   rtc->second = 0;
//   rtc->minute = 0;
//   rtc->hour = 0;
//   rtc->day = 0;
//   rtc->month = 0;
//   rtc->year = 1900;
// }
