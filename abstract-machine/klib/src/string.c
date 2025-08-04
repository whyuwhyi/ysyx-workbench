#include <klib-macros.h>
#include <klib.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

// String length functions
size_t strlen(const char *s) {
  if (!s)
    return 0;

  const char *start = s;
  while (*s)
    s++;
  return s - start;
}

// String copy functions
char *strcpy(char *dst, const char *src) {
  if (!dst || !src)
    return dst;

  char *ret = dst;
  while ((*dst++ = *src++))
    ;
  return ret;
}

char *strncpy(char *dst, const char *src, size_t n) {
  if (!dst || !src || n == 0)
    return dst;

  char *ret = dst;
  size_t i = 0;

  while (i < n && (*dst = *src)) {
    dst++;
    src++;
    i++;
  }

  while (i < n) {
    *dst++ = '\0';
    i++;
  }

  return ret;
}

char *strcat(char *dst, const char *src) {
  if (!dst || !src)
    return dst;

  char *ret = dst;
  dst += strlen(dst);
  while ((*dst++ = *src++))
    ;
  return ret;
}

// String comparison functions
int strcmp(const char *s1, const char *s2) {
  if (!s1 || !s2)
    return (!s1) - (!s2);

  while (*s1 && *s1 == *s2) {
    s1++;
    s2++;
  }
  return (unsigned char)*s1 - (unsigned char)*s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  if (!s1 || !s2 || n == 0) {
    if (n == 0)
      return 0;
    return (!s1) - (!s2);
  }

  while (n-- && *s1 && *s1 == *s2) {
    s1++;
    s2++;
  }

  return n == SIZE_MAX ? 0 : (unsigned char)*s1 - (unsigned char)*s2;
}

void *memset(void *s, int c, size_t n) {
  if (!s || n == 0)
    return s;

  unsigned char *ptr = (unsigned char *)s;
  unsigned char val = (unsigned char)c;

  if (n >= 4 && ((uintptr_t)ptr & 3) == 0) {
    uint32_t val32 = val | (val << 8) | (val << 16) | (val << 24);
    while (n >= 4) {
      *(uint32_t *)ptr = val32;
      ptr += 4;
      n -= 4;
    }
  }

  while (n--) {
    *ptr++ = val;
  }

  return s;
}

void *memcpy(void *dst, const void *src, size_t n) {
  if (!dst || !src || n == 0)
    return dst;
  if (dst == src)
    return dst;

  unsigned char *d = (unsigned char *)dst;
  const unsigned char *s = (const unsigned char *)src;

  if (n >= 4 && ((uintptr_t)d & 3) == 0 && ((uintptr_t)s & 3) == 0) {
    while (n >= 4) {
      *(uint32_t *)d = *(uint32_t *)s;
      d += 4;
      s += 4;
      n -= 4;
    }
  }

  while (n--) {
    *d++ = *s++;
  }

  return dst;
}

void *memmove(void *dst, const void *src, size_t n) {
  if (!dst || !src || n == 0)
    return dst;
  if (dst == src)
    return dst;

  unsigned char *d = (unsigned char *)dst;
  const unsigned char *s = (const unsigned char *)src;

  if (d < s || d >= s + n) {
    return memcpy(dst, src, n);
  } else {
    d += n - 1;
    s += n - 1;
    while (n--) {
      *d-- = *s--;
    }
  }

  return dst;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  if (!s1 || !s2)
    return (!s1) - (!s2);
  if (n == 0 || s1 == s2)
    return 0;

  const unsigned char *p1 = (const unsigned char *)s1;
  const unsigned char *p2 = (const unsigned char *)s2;

  if (n >= 4 && ((uintptr_t)p1 & 3) == 0 && ((uintptr_t)p2 & 3) == 0) {
    while (n >= 4) {
      uint32_t v1 = *(uint32_t *)p1;
      uint32_t v2 = *(uint32_t *)p2;
      if (v1 != v2)
        break;
      p1 += 4;
      p2 += 4;
      n -= 4;
    }
  }

  while (n--) {
    if (*p1 != *p2) {
      return *p1 - *p2;
    }
    p1++;
    p2++;
  }

  return 0;
}
#endif
