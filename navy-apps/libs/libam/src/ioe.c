#include <NDL.h>
#include <am.h>
#include <amdev.h>
#include <klib-macros.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static void __am_timer_config(AM_TIMER_CONFIG_T *cfg) {
  cfg->present = true;
  cfg->has_rtc = true;
}
static void __am_input_config(AM_INPUT_CONFIG_T *cfg) { cfg->present = true; }
static void __am_uart_config(AM_INPUT_CONFIG_T *cfg) { cfg->present = false; }
static void __am_timer_init();

static void __am_timer_rtc(AM_TIMER_RTC_T *);
static void __am_timer_uptime(AM_TIMER_UPTIME_T *);
static void __am_input_keybrd(AM_INPUT_KEYBRD_T *);

typedef void (*handler_t)(void *buf);
static void *lut[128] = {
    [AM_TIMER_CONFIG] = __am_timer_config,
    [AM_TIMER_RTC] = __am_timer_rtc,
    [AM_TIMER_UPTIME] = __am_timer_uptime,
    [AM_INPUT_CONFIG] = __am_input_config,
    [AM_INPUT_KEYBRD] = __am_input_keybrd,
    [AM_UART_CONFIG] = __am_uart_config,
};

static void fail(void *buf) { panic("access nonexist register"); }

bool ioe_init() {
  for (int i = 0; i < LENGTH(lut); i++)
    if (!lut[i])
      lut[i] = fail;
  __am_timer_init();

  // NDL_Init(0);
  return true;
}

void ioe_read(int reg, void *buf) { ((handler_t)lut[reg])(buf); }
void ioe_write(int reg, void *buf) { ((handler_t)lut[reg])(buf); }

static void __am_timer_init() {}
static void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour = 0;
  rtc->day = 0;
  rtc->month = 0;
  rtc->year = 1900;
}
static void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  uptime->us = (uint64_t)NDL_GetTicks() / 1000;
}
static void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  kbd->keydown = 0;
  kbd->keycode = AM_KEY_NONE;
}
