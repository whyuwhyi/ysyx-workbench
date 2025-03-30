#include <am.h>
#include <klib-macros.h>
#include <klib.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int ret = vprintf(fmt, ap);
  va_end(ap);
  return ret;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int ret = vsprintf(out, fmt, ap);
  va_end(ap);
  return ret;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int ret = vsnprintf(out, n, fmt, ap);
  va_end(ap);
  return ret;
}

int vprintf(const char *fmt, va_list ap) {
  char buf[1024];
  int ret = vsnprintf(buf, 1024, fmt, ap);

  for (int i = 0; i < ret; i++) {
    putch(buf[i]);
  }

  return ret;
}

int vsprintf(char *out, const char *fmt, va_list ap) {

  int ret = vsnprintf(out, -1, fmt, ap);

  return ret;
}

static int itoa(int num, char *buf) {
  int i = 0;
  int is_negative = 0;
  if (num < 0) {
    is_negative = 1;
    num = -num;
  }
  if (num == 0) {
    buf[i++] = '0';
  } else {
    while (num > 0 && i < 31) {
      buf[i++] = '0' + (num % 10);
      num /= 10;
    }
  }
  int len = i;
  if (is_negative) {
    buf[i++] = '-';
    len++;
  }
  for (int j = 0; j < i / 2; j++) {
    char tmp = buf[j];
    buf[j] = buf[i - j - 1];
    buf[i - j - 1] = tmp;
  }
  buf[i] = '\0';
  return len;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  size_t total = 0;
  char *ptr = out;
  size_t available = n;

  while (*fmt) {
    if (*fmt != '%') {
      total++;
      if (available > 1) {
        *ptr++ = *fmt;
        available--;
      }
      fmt++;
    } else {
      fmt++;
      if (*fmt == 'd') {
        int num = va_arg(ap, int);
        char numbuf[32];
        int len = itoa(num, numbuf);
        for (int i = 0; i < len; i++) {
          total++;
          if (available > 1) {
            *ptr++ = numbuf[i];
            available--;
          }
        }
        fmt++;
      } else if (*fmt == 's') {
        const char *s = va_arg(ap, const char *);
        while (*s) {
          total++;
          if (available > 1) {
            *ptr++ = *s;
            available--;
          }
          s++;
        }
        fmt++;
      } else if (*fmt == 'c') {
        char c = (char)va_arg(ap, int);
        total++;
        if (available > 1) {
          *ptr++ = c;
          available--;
        }
        fmt++;
      } else {
        total++;
        if (available > 1) {
          *ptr++ = '%';
          available--;
        }
        total++;
        if (available > 1) {
          *ptr++ = *fmt;
          available--;
        }
        fmt++;
      }
    }
  }
  if (n > 0) {
    *ptr = '\0';
  }
  return (int)total;
}

#endif
