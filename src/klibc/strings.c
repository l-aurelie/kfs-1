#include "libc.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// memory

inline void *memcpy(void *dest, const void *src, size_t n) {
  const char *s = src;
  char *d = dest;

  // slow!
  for (; n > 0; --n, ++s, ++d)
    *d = *s;
  return dest;
}

inline void *memset(void *s, int c, size_t n) {
  for (size_t i = 0; i < n; ++i)
    ((char *)s)[i] = c;
  return s;
}

inline void bzero(void *s, size_t n) { memset(s, 0, n); }

// strings

inline size_t strlen(const char *s) {
  const char *t;

  for (t = s; *t; ++t)
    ;
  return t - s;
}

size_t strnlen(const char *s, size_t maxlen) {
  const char *t;

  for (t = s; maxlen > 0 && *t; ++t, maxlen--)
    ;
  return t - s;
}

int strcmp(const char *s1, const char *s2) {
  // TODO better optimized
  const unsigned char *tmp1 = (const unsigned char *)s1;
  const unsigned char *tmp2 = (const unsigned char *)s2;
  size_t i = 0;

  while ((tmp1[i] && tmp2[i]) && tmp1[i] == tmp2[i])
    ++i;
  return (tmp1[i] - tmp2[i]);
}

int strncmp(const char *s1, const char *s2, size_t n) {
  // TODO better optimized
  const unsigned char *tmp1 = (const unsigned char *)s1;
  const unsigned char *tmp2 = (const unsigned char *)s2;
  size_t i = 0;

  if (!n)
    return (0);

  while ((i + 1) < n && (tmp1[i] && tmp2[i]) && tmp1[i] == tmp2[i])
    i++;
  return (tmp1[i] - tmp2[i]);
}

char *strchr(const char *s, int c) {
  while (*s) {
    if (*s == c)
      return (char *)s;
    s++;
  }
  return NULL;
}

char *strtok_r(char *str, const char *delim, char **saveptr) {
  char *token;

  if (str != NULL)
    *saveptr = str;
  if (*saveptr == NULL)
    return NULL;
  token = *saveptr;
  while (*token && strchr(delim, *token))
    token++;
  if (!*token) {
    *saveptr = NULL;
    return NULL;
  }
  str = token;
  while (*str && !strchr(delim, *str))
    str++;
  if (*str) {
    *str = '\0';
    *saveptr = str + 1;
  } else
    *saveptr = NULL;
  return token;
}

char *strtok(char *str, const char *delim) {
  static char *saveptr = NULL;
  return strtok_r((char *)str, delim, &saveptr);
}

int atoi(const char *str) {
  long int r = 0;
  bool neg = false;

  for (; isspace(*str); ++str)
    ;
  neg = (*str == '-');
  if (neg || *str == '+')
    str++;
  for (; isdigit(*str); ++str)
    r = r * 10 + (*str - '0');
  return (neg) ? -r : r;
}

// non-standard

uint32_t atou_base(const char *str, unsigned short base) {
  uint32_t r = 0;

  for (; isspace(*str) || *str == '0' || *str == 'x'; ++str)
    ;
  for (; isalnum(*str); ++str)
    if (base <= 10 && (*str >= '0' && *str <= '0' + base - 1)) {
      r = r * base + (*str - '0');
    } else if (base <= 16) {
      if (isdigit(*str)) {
        r = r * base + (*str - '0');
      } else if (*str >= 'a' && *str <= 'a' + base - 11) {
        r = r * base + (10 + *str - 'a');
      } else if (*str >= 'A' && *str <= 'A' + base - 11) {
        r = r * base + (10 + *str - 'A');
      } else {
        break;
      }
    } else
      break;

  return r;
}

void strrev(char *s) {
  char c;

  for (int i = 0, j = strlen(s) - 1; i < j; ++i, --j) {
    c = s[i];
    s[i] = s[j];
    s[j] = c;
  }
}

char *utoa(char *str, unsigned int value, unsigned char base) {
  const char base_digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
  unsigned int i = 0;

  str[0] = '\0';
  if (base < 2 || base > 36)
    return NULL;
  while (value != 0) {
    str[i++] = base_digits[value % base];
    value /= base;
  }
  if (str[0]) {
    str[i] = '\0';
  } else {
    str[0] = '0';
    str[1] = '\0';
  }
  strrev(str);
  return str;
}

char *itoa(char *str, int value, unsigned char base) {
  unsigned int i = 0;
  unsigned int uval;

  str[0] = '\0';
  if (base < 2 || base > 36)
    return NULL;
  if (base == 10 && value < 0) {
    str[i++] = '-';
    uval = (unsigned)-value;
  } else {
    uval = (unsigned)value;
  }
  utoa(str + i, uval, base);
  return str;
}
