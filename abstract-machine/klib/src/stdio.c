#include <am.h>
#include <klib-macros.h>
#include <klib.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  if (!fmt) return -1;
  va_list ap;
  va_start(ap, fmt);
  int ret = vprintf(fmt, ap);
  va_end(ap);
  return ret;
}

int sprintf(char *out, const char *fmt, ...) {
  if (!out || !fmt) return -1;
  va_list ap;
  va_start(ap, fmt);
  int ret = vsprintf(out, fmt, ap);
  va_end(ap);
  return ret;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  if (!out || !fmt) return -1;
  va_list ap;
  va_start(ap, fmt);
  int ret = vsnprintf(out, n, fmt, ap);
  va_end(ap);
  return ret;
}

int vprintf(const char *fmt, va_list ap) {
  if (!fmt) return -1;
  char buf[1024];
  int ret = vsnprintf(buf, sizeof(buf), fmt, ap);

  for (int i = 0; buf[i] != '\0'; i++) {
    putch(buf[i]);
  }

  return ret;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  if (!out || !fmt) return -1;
  return vsnprintf(out, SIZE_MAX, fmt, ap);
}

static int itoa_base(long long num, char *buf, int base, int uppercase) {
  if (base < 2 || base > 36) return 0;
  
  int i = 0;
  int is_negative = 0;
  const char *digits = uppercase ? "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ" 
                                 : "0123456789abcdefghijklmnopqrstuvwxyz";
  
  if (num < 0 && base == 10) {
    is_negative = 1;
    num = -num;
  }
  
  unsigned long long unum = (unsigned long long)num;
  
  if (unum == 0) {
    buf[i++] = '0';
  } else {
    while (unum > 0 && i < 31) {
      buf[i++] = digits[unum % base];
      unum /= base;
    }
  }
  
  if (is_negative) {
    buf[i++] = '-';
  }
  
  for (int j = 0; j < i / 2; j++) {
    char tmp = buf[j];
    buf[j] = buf[i - j - 1];
    buf[i - j - 1] = tmp;
  }
  
  buf[i] = '\0';
  return i;
}

static int format_number(char *out, size_t available, long long num, int base, 
                        int uppercase, int width, int zero_pad) {
  char numbuf[64];
  int len = itoa_base(num, numbuf, base, uppercase);
  int written = 0;
  
  int pad_len = (width > len) ? width - len : 0;
  char pad_char = zero_pad ? '0' : ' ';
  
  if (pad_len > 0 && pad_char == ' ') {
    for (int i = 0; i < pad_len && available > 1; i++) {
      *out++ = ' ';
      available--;
      written++;
    }
  }
  
  if (numbuf[0] == '-' && zero_pad && pad_len > 0) {
    if (available > 1) {
      *out++ = '-';
      available--;
      written++;
    }
    for (int i = 0; i < pad_len && available > 1; i++) {
      *out++ = '0';
      available--;
      written++;
    }
    for (int i = 1; i < len && available > 1; i++) {
      *out++ = numbuf[i];
      available--;
      written++;
    }
  } else {
    if (pad_len > 0 && zero_pad) {
      for (int i = 0; i < pad_len && available > 1; i++) {
        *out++ = '0';
        available--;
        written++;
      }
    }
    for (int i = 0; i < len && available > 1; i++) {
      *out++ = numbuf[i];
      available--;
      written++;
    }
  }
  
  return width > len ? width : len;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  if (!out || !fmt) return -1;
  
  size_t total = 0;
  char *ptr = out;
  size_t available = n;

  while (*fmt) {
    if (*fmt != '%') {
      if (available > 1) {
        *ptr++ = *fmt;
        available--;
      }
      fmt++;
      total++;
    } else {
      fmt++; // skip '%'
      
      // Parse flags
      int zero_pad = 0;
      int left_align = 0;
      while (*fmt == '0' || *fmt == '-') {
        if (*fmt == '0') zero_pad = 1;
        if (*fmt == '-') left_align = 1;
        fmt++;
      }
      
      // Parse width
      int width = 0;
      while (*fmt >= '0' && *fmt <= '9') {
        width = width * 10 + (*fmt - '0');
        fmt++;
      }
      
      // Parse format specifier
      switch (*fmt) {
        case 'd':
        case 'i': {
          int num = va_arg(ap, int);
          int len = format_number(ptr, available, num, 10, 0, width, zero_pad && !left_align);
          if (available > 1) {
            int written = (len < available - 1) ? len : available - 1;
            ptr += written;
            available -= written;
          }
          total += len;
          break;
        }
        
        case 'u': {
          unsigned int num = va_arg(ap, unsigned int);
          int len = format_number(ptr, available, (long long)num, 10, 0, width, zero_pad && !left_align);
          if (available > 1) {
            int written = (len < available - 1) ? len : available - 1;
            ptr += written;
            available -= written;
          }
          total += len;
          break;
        }
        
        case 'x': {
          unsigned int num = va_arg(ap, unsigned int);
          int len = format_number(ptr, available, (long long)num, 16, 0, width, zero_pad && !left_align);
          if (available > 1) {
            int written = (len < available - 1) ? len : available - 1;
            ptr += written;
            available -= written;
          }
          total += len;
          break;
        }
        
        case 'X': {
          unsigned int num = va_arg(ap, unsigned int);
          int len = format_number(ptr, available, (long long)num, 16, 1, width, zero_pad && !left_align);
          if (available > 1) {
            int written = (len < available - 1) ? len : available - 1;
            ptr += written;
            available -= written;
          }
          total += len;
          break;
        }
        
        case 'o': {
          unsigned int num = va_arg(ap, unsigned int);
          int len = format_number(ptr, available, (long long)num, 8, 0, width, zero_pad && !left_align);
          if (available > 1) {
            int written = (len < available - 1) ? len : available - 1;
            ptr += written;
            available -= written;
          }
          total += len;
          break;
        }
        
        case 'p': {
          void *p = va_arg(ap, void *);
          if (available > 2) {
            *ptr++ = '0';
            *ptr++ = 'x';
            available -= 2;
          }
          total += 2;
          unsigned long addr = (unsigned long)p;
          int len = format_number(ptr, available, (long long)addr, 16, 0, 0, 0);
          if (available > 1) {
            int written = (len < available - 1) ? len : available - 1;
            ptr += written;
            available -= written;
          }
          total += len;
          break;
        }
        
        case 's': {
          const char *s = va_arg(ap, const char *);
          if (!s) s = "(null)";
          
          // Calculate actual string length (up to width limit if specified)
          int str_len = 0;
          while (s[str_len] && (width == 0 || str_len < width)) {
            str_len++;
          }
          
          // Add padding if width is specified and string is shorter
          int pad_len = (width > str_len) ? width - str_len : 0;
          
          // Add left padding (spaces)
          for (int i = 0; i < pad_len && available > 1; i++) {
            *ptr++ = ' ';
            available--;
          }
          
          // Copy the string
          for (int i = 0; i < str_len && available > 1; i++) {
            *ptr++ = s[i];
            available--;
          }
          
          total += str_len + pad_len;
          break;
        }
        
        case 'c': {
          char c = (char)va_arg(ap, int);
          if (available > 1) {
            *ptr++ = c;
            available--;
          }
          total++;
          break;
        }
        
        case '%': {
          if (available > 1) {
            *ptr++ = '%';
            available--;
          }
          total++;
          break;
        }
        
        default: {
          if (available > 2) {
            *ptr++ = '%';
            *ptr++ = *fmt;
            available -= 2;
          }
          total += 2;
          break;
        }
      }
      fmt++;
    }
  }
  
  if (n > 0) {
    *ptr = '\0';
  }
  return (int)total;
}

#endif
