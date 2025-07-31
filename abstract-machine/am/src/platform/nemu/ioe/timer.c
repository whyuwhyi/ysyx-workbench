#include <am.h>
#include <nemu.h>
#include <stdint.h>

#define RTC_ADDR_HIGH (RTC_ADDR + 4)
#define RTC_ADDR_LOW RTC_ADDR

void __am_timer_init() {}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  uint64_t high_us = inl(RTC_ADDR_HIGH);
  uint64_t low_us = inl(RTC_ADDR_LOW);
  uptime->us = (high_us << 32) | low_us;
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour = 0;
  rtc->day = 0;
  rtc->month = 0;
  rtc->year = 1900;
}
