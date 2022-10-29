#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

void write_int(int x, int *num)
{
	if(x < 0)
	{
		putch('-');
		(*num)++;
		x = -x;
	}
	if(x>=10)
		write_int(x/10, num);
	putch(x%10+'0');
	(*num)++;
	return;
}

int printf(const char *fmt, ...) {
	va_list ap;
	int char_num = 0;
	va_start(ap, fmt);
	while(*fmt)
	{
		if(*fmt != '%')
		{
			putch(*fmt);
			char_num++;
		}
		else
		{
			fmt++;
			switch(*fmt)
			{
				case '%':	
					putch('%');
					char_num++;
					break;
				case 'c':
					char char_record = (char)va_arg(ap, int);
					//这里va_arg参数为int的原因是，C中调用一个不带原型声明的函数，例如可变长参数函数，传入参数执行“默认实际参数提升”。char将提升为int。
					putch(char_record);
					char_num++;
					break;
				case 's':	
					char* string_record = va_arg(ap, char*);
					while(*string_record != 0)
					{
						putch(*string_record);
						char_num++;
						string_record++;
					}
					break; 
				case 'd':	
					int int_record = va_arg(ap, int);
					write_int(int_record, &char_num);
					break;
				default:
					return -1;
					break;
			}
		}
		fmt++;
	}
	return char_num;
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
