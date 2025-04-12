#include <klib-macros.h>
#include <klib.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  assert(s != NULL);

  size_t len = 0;
  while (s[len] != '\0') {
    len++;
  }
  return len;
}

char *strcpy(char *dst, const char *src) {
  assert(dst != NULL && src != NULL);

  size_t i = 0;

  while ((dst[i] = src[i]) != '\0') {
    i++;
  }

  return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
  assert(dst != NULL && src != NULL);

  size_t i = 0;

  while (i < n && (dst[i] = src[i]) != '\0') {
    i++;
  }

  return dst;
}

char *strcat(char *dst, const char *src) {
  assert(dst != NULL && src != NULL);

  strcpy(dst + strlen(dst), src);
  return dst;
}

int strcmp(const char *s1, const char *s2) {
  assert(s1 != NULL && s2 != NULL);
  size_t i = 0;

  while (s1[i] != '\0' && s1[i] == s2[i]) {
    i++;
  }

  return s1[i] - s2[i];
}

int strncmp(const char *s1, const char *s2, size_t n) {
  assert(s1 != NULL && s2 != NULL);

  size_t i = 0;

  while (i < n && s1[i] != '\0' && s1[i] == s2[i]) {
    i++;
  }

  return i == n ? 0 : (int)s1[i] - (int)s2[i];
}

void *memset(void *s, int c, size_t n) {
  assert(s != NULL);

  size_t i = 0;

  while (i < n) {
    ((uint8_t *)s)[i] = (uint8_t)c;
    i++;
  }

  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  assert(dst != NULL && src != NULL);

  char *d = (char *)dst;
  const char *s = (const char *)src;

  if (d == s) {
    return dst;
  }

  if (d < s || d >= s + n) {
    while (n--) {
      *d++ = *s++;
    }
  } else {
    d += n;
    s += n;
    while (n--) {
      *(--d) = *(--s);
    }
  }

  return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
  assert(out != NULL && in != NULL);

  size_t i = 0;

  while (i < n) {
    ((uint8_t *)out)[i] = ((uint8_t *)in)[i];
    i++;
  }

  return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  assert(s1 != NULL && s2 != NULL);

  size_t i = 0;

  while (i < n && ((uint8_t *)s1)[i] == ((uint8_t *)s2)[i]) {
    i++;
  }

  return i == n ? 0 : ((uint8_t *)s1)[i] - ((uint8_t *)s2)[i];
}

#endif
