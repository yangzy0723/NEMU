#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

void write_int(int x)
{
	if(x < 0)
	{
		outb('-');
		x = -x;
	}
	if(x>=10)
		write_int(x/10);
	outb(x%10+'0');
	return;
}

int printf(const char *fmt, ...) {
	int int_record;
	va_list ap;
	va_start(ap, fmt);
	while(*fmt)
	{
		if(*fmt != '%')
			outb(*fmt);
		else
		{
			fmt++;
			switch(*fmt)
			{
				case '%':	
					outb('%'); break;
				case 'c':
					char char_record = (char)va_arg(ap, int);
					//这里va_arg参数为int的原因是，C中调用一个不带原型声明的函数，例如可变长参数函数，传入参数执行“默认实际参数提升”。char将提升为int。
					outb(char_record);
					break;
				case 's':	
					char* string_record = va_arg(ap, char*);
					while(*string_record != 0)
					{
						outb(*string_record);
						string_record++;
					}
					break; 
				case 'd':	
					int int_record = va_arg(ap, int);
					write_int(int_record);
					break;
				default:
					break;
			}
		}
	}
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
  panic("Not implemented");
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
