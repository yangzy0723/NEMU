#include <am.h>
#include <klib-macros.h>
#include <klib.h>
int main(const char *args) {
  const char *fmt =
    "Hello, AbstractMachine!\n"
    "mainargs = '%'.\n";
  for (const char *p = fmt; *p; p++) {
    (*p == '%') ? putstr(args) : putch(*p);
  }
  	int d = 99999;
	char s[100] = "$$$$$$$$$";
	char buffer[100] = {0};
  printf("%d",printf("%s%d\n",s,d));
  printf("%d",sprintf(buffer, "%s%d%s\n", s, d, s));
  printf("%s\n",buffer);
  printf("%d",snprintf(buffer, 5, "$$$$$$$$$$$$$$$$$$"));
  printf("%s",buffer);
  return 0;
}
