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
	*dst = *src;//将空字符也copy进来
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
	char *record = dst;
	while(*dst != 0)
	{
		dst++;
	}
	while(*src != 0)
	{
		*dst = *src;
		dst++;
		src++;
	}
	*dst = *src;
	return record;
}

int strcmp(const char *s1, const char *s2) {
  while((*s1 == *s2) && *s1 != 0 && *s2 != 0)
	{
		s1++;
		s2++;
	}
	if(*s1 == 0 && *s2 == 0)
		return 0;
	else if(*s1 == 0)
		return -1;
	else if(*s2 == 0)
		return 1;
	else
		return *s1 < *s2 ? -1 : 1;
}

int strncmp(const char *s1, const char *s2, size_t n) {
	int i;
  for(i = 1; i <= n; i++)
	{
		if((*s1 == *s2) && *s1 != 0 && *s2 != 0)
		{
			s1++;
			s2++;
		}
		else
			break;
	}
	if(i == n+1)
		return 0;
	else if(*s1 == 0 && *s2 == 0)
		return 0;
	else if(*s1 == 0)
		return -1;
	else if(*s2 == 0)
		return 1;
	else
		return *s1 < *s2 ? -1 : 1;
}

void *memset(void *s, int c, size_t n) {
  unsigned char *record = (unsigned char*)s;
	while(n--)
	{
		*(unsigned char*)s = (unsigned char)c;
		s++;
	}
	return record;
}

void *memmove(void *dst, const void *src, size_t n) {
  unsigned char *record = (unsigned char*)dst;
	while(n--)
	{
		*(unsigned char*)dst = *(unsigned char*)src;
		dst++;
		src++;
	}
	return record;
}

void *memcpy(void *out, const void *in, size_t n) {
  return memmove(out, in, n);
}

int memcmp(const void *s1, const void *s2, size_t n) {
	for(int i = 1; i <= n; i++)
	{
		if(*(unsigned char*)s1 == *(unsigned char*)s2)
		{
			s1++;
			s2++;
		}
		else
		{
			if(*(unsigned char*)s1 < *(unsigned char*)s2)
				return -1;
			else
				return 1;
		}
	}
	return 0;
}

#endif
