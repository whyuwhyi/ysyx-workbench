#include <klib-macros.h>
#include <klib.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

// String length functions
size_t strlen(const char *s) {
  if (!s) return 0;
  
  const char *start = s;
  while (*s) s++;
  return s - start;
}

size_t strnlen(const char *s, size_t maxlen) {
  if (!s) return 0;
  
  size_t len = 0;
  while (len < maxlen && s[len]) len++;
  return len;
}

// String copy functions
char *strcpy(char *dst, const char *src) {
  if (!dst || !src) return dst;
  
  char *ret = dst;
  while ((*dst++ = *src++));
  return ret;
}

char *strncpy(char *dst, const char *src, size_t n) {
  if (!dst || !src || n == 0) return dst;
  
  char *ret = dst;
  size_t i = 0;
  
  // Copy characters until null terminator or n reached
  while (i < n && (*dst = *src)) {
    dst++;
    src++;
    i++;
  }
  
  // Null-pad remaining bytes (POSIX requirement)
  while (i < n) {
    *dst++ = '\0';
    i++;
  }
  
  return ret;
}

// String concatenation functions
char *strcat(char *dst, const char *src) {
  if (!dst || !src) return dst;
  
  char *ret = dst;
  dst += strlen(dst);  // Find end of dst
  while ((*dst++ = *src++));
  return ret;
}

char *strncat(char *dst, const char *src, size_t n) {
  if (!dst || !src || n == 0) return dst;
  
  char *ret = dst;
  dst += strlen(dst);  // Find end of dst
  
  while (n-- && (*dst = *src)) {
    dst++;
    src++;
  }
  *dst = '\0';  // Ensure null termination
  
  return ret;
}

// String comparison functions
int strcmp(const char *s1, const char *s2) {
  if (!s1 || !s2) return (!s1) - (!s2);
  
  while (*s1 && *s1 == *s2) {
    s1++;
    s2++;
  }
  return (unsigned char)*s1 - (unsigned char)*s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  if (!s1 || !s2 || n == 0) {
    if (n == 0) return 0;
    return (!s1) - (!s2);
  }
  
  while (n-- && *s1 && *s1 == *s2) {
    s1++;
    s2++;
  }
  
  return n == SIZE_MAX ? 0 : (unsigned char)*s1 - (unsigned char)*s2;
}

// String search functions
char *strchr(const char *s, int c) {
  if (!s) return NULL;
  
  char ch = (char)c;
  while (*s) {
    if (*s == ch) return (char *)s;
    s++;
  }
  return (ch == '\0') ? (char *)s : NULL;
}

char *strrchr(const char *s, int c) {
  if (!s) return NULL;
  
  char *last = NULL;
  char ch = (char)c;
  
  while (*s) {
    if (*s == ch) last = (char *)s;
    s++;
  }
  
  return (ch == '\0') ? (char *)s : last;
}

char *strstr(const char *haystack, const char *needle) {
  if (!haystack || !needle) return NULL;
  if (!*needle) return (char *)haystack;
  
  size_t needle_len = strlen(needle);
  
  while (*haystack) {
    if (*haystack == *needle && strncmp(haystack, needle, needle_len) == 0) {
      return (char *)haystack;
    }
    haystack++;
  }
  
  return NULL;
}

// Memory manipulation functions  
void *memset(void *s, int c, size_t n) {
  if (!s || n == 0) return s;
  
  unsigned char *ptr = (unsigned char *)s;
  unsigned char val = (unsigned char)c;
  
  // Optimized: set 4 bytes at a time when aligned
  if (n >= 4 && ((uintptr_t)ptr & 3) == 0) {
    uint32_t val32 = val | (val << 8) | (val << 16) | (val << 24);
    while (n >= 4) {
      *(uint32_t *)ptr = val32;
      ptr += 4;
      n -= 4;
    }
  }
  
  // Handle remaining bytes
  while (n--) {
    *ptr++ = val;
  }
  
  return s;
}

void *memcpy(void *dst, const void *src, size_t n) {
  if (!dst || !src || n == 0) return dst;
  if (dst == src) return dst;
  
  unsigned char *d = (unsigned char *)dst;
  const unsigned char *s = (const unsigned char *)src;
  
  // Optimized: copy 4 bytes at a time when aligned
  if (n >= 4 && ((uintptr_t)d & 3) == 0 && ((uintptr_t)s & 3) == 0) {
    while (n >= 4) {
      *(uint32_t *)d = *(uint32_t *)s;
      d += 4;
      s += 4;
      n -= 4;
    }
  }
  
  // Handle remaining bytes
  while (n--) {
    *d++ = *s++;
  }
  
  return dst;
}

void *memmove(void *dst, const void *src, size_t n) {
  if (!dst || !src || n == 0) return dst;
  if (dst == src) return dst;
  
  unsigned char *d = (unsigned char *)dst;
  const unsigned char *s = (const unsigned char *)src;
  
  // Check for overlap and copy direction
  if (d < s || d >= s + n) {
    // No overlap or dst before src - copy forward
    return memcpy(dst, src, n);
  } else {
    // Overlap with dst after src - copy backward
    d += n - 1;
    s += n - 1;
    while (n--) {
      *d-- = *s--;
    }
  }
  
  return dst;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  if (!s1 || !s2) return (!s1) - (!s2);
  if (n == 0 || s1 == s2) return 0;
  
  const unsigned char *p1 = (const unsigned char *)s1;
  const unsigned char *p2 = (const unsigned char *)s2;
  
  // Optimized: compare 4 bytes at a time when aligned
  if (n >= 4 && ((uintptr_t)p1 & 3) == 0 && ((uintptr_t)p2 & 3) == 0) {
    while (n >= 4) {
      uint32_t v1 = *(uint32_t *)p1;
      uint32_t v2 = *(uint32_t *)p2;
      if (v1 != v2) break;
      p1 += 4;
      p2 += 4;
      n -= 4;
    }
  }
  
  // Handle remaining bytes
  while (n--) {
    if (*p1 != *p2) {
      return *p1 - *p2;
    }
    p1++;
    p2++;
  }
  
  return 0;
}

void *memchr(const void *s, int c, size_t n) {
  if (!s || n == 0) return NULL;
  
  const unsigned char *ptr = (const unsigned char *)s;
  unsigned char ch = (unsigned char)c;
  
  while (n--) {
    if (*ptr == ch) return (void *)ptr;
    ptr++;
  }
  
  return NULL;
}

// Additional utility functions
char *strdup(const char *s) {
  if (!s) return NULL;
  
  size_t len = strlen(s) + 1;
  char *copy = (char *)malloc(len);
  if (copy) {
    memcpy(copy, s, len);
  }
  
  return copy;
}

char *strtok(char *str, const char *delim) {
  static char *last = NULL;
  
  if (str) last = str;
  if (!last || !delim) return NULL;
  
  // Skip leading delimiters
  while (*last && strchr(delim, *last)) last++;
  if (!*last) return NULL;
  
  char *start = last;
  
  // Find end of token
  while (*last && !strchr(delim, *last)) last++;
  
  if (*last) {
    *last++ = '\0';
  }
  
  return start;
}

// Case conversion helpers
#ifndef _CTYPE_H
#ifndef __CTYPE_H
#ifndef _CTYPE_H_
int tolower(int c) {
  return (c >= 'A' && c <= 'Z') ? c + 32 : c;
}

int toupper(int c) {
  return (c >= 'a' && c <= 'z') ? c - 32 : c;
}
#endif
#endif
#endif

// Case-insensitive comparison
int strcasecmp(const char *s1, const char *s2) {
  if (!s1 || !s2) return (!s1) - (!s2);
  
  while (*s1 && *s2 && tolower(*s1) == tolower(*s2)) {
    s1++;
    s2++;
  }
  
  return tolower(*s1) - tolower(*s2);
}

int strncasecmp(const char *s1, const char *s2, size_t n) {
  if (!s1 || !s2 || n == 0) {
    if (n == 0) return 0;
    return (!s1) - (!s2);
  }
  
  while (n-- && *s1 && *s2 && tolower(*s1) == tolower(*s2)) {
    s1++;
    s2++;
  }
  
  return n == SIZE_MAX ? 0 : tolower(*s1) - tolower(*s2);
}

#endif