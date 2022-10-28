#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t len = 0;
	while(*s != 0)
	{
		len++;
		s++;
	}
	return len;
}

char *strcpy(char *dst, const char *src) {
	char *record = dst;
  while(*src != 0)
	{
		*dst = *src;
		src++;
		dst++;
	}
	*dst = *src;
	return record;
}

char *strncpy(char *dst, const char *src, size_t n) {
  char *record = dst;
	for(int i = 1; i <= n; i++)
	{
		if(*src != 0)
		{
			*dst = *src;
			dst++;
			src++;
		}
		else
		{
			*dst = 0;
      dst++;
		}
	}
	return record;
}

char *strcat(char *dst, const char *src) {
  panic("Not implemented");
}

int strcmp(const char *s1, const char *s2) {
  panic("Not implemented");
}

int strncmp(const char *s1, const char *s2, size_t n) {
  panic("Not implemented");
}

void *memset(void *s, int c, size_t n) {
  panic("Not implemented");
}

void *memmove(void *dst, const void *src, size_t n) {
  panic("Not implemented");
}

void *memcpy(void *out, const void *in, size_t n) {
  panic("Not implemented");
}

int memcmp(const void *s1, const void *s2, size_t n) {
  panic("Not implemented");
}

#endif
