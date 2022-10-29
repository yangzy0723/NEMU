#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int write_int(int x)
{
	int num = 0;
	if(x < 0)
	{
		putch('-');
		num++;
		x = -x;
	}
	if(x >= 10)
		num += write_int(x / 10);
	putch(x % 10 + '0');
	num++;
	return num;
}

int write_int_buffer(int x, char **buffer)
{
	int num = 0;
	if(x < 0)
	{
		*(*buffer) = '-';
		(*buffer)++;
		num++;
		x = -x;
	}
	if(x >= 10)
		num += write_int_buffer(x/10, buffer);
	*(*buffer) = x % 10 + '0';
	(*buffer)++;
	num++;
	return num;
}

void write_int_buffer_limit(int x, int size, int *num, char **buffer)
{
	if(*num == size)
			return;
	if(x < 0)
	{
		*(*buffer) = '-';
		(*buffer)++;
		num++;
		x = -x;
	}
	if(*num == size)
		return;
	if(x >= 10)
		write_int_buffer_limit(x/10, size, num, buffer);
	if(*num == size)
		return;
	*(*buffer) = x % 10 + '0';
	(*buffer)++;
	(*num)++;
	return;
}

int printf(const char *fmt, ...) {
	va_list ap;
	int char_num = 0;
	int int_record;
	char char_record;
	char *string_record;
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
					char_record = (char)va_arg(ap, int);
					//这里va_arg参数为int的原因是，C中调用一个不带原型声明的函数，例如可变长参数函数，传入参数执行“默认实际参数提升”。char将提升为int。
					putch(char_record);
					char_num++;
					break;
				case 's':	
					string_record = va_arg(ap, char*);
					while(*string_record != 0)
					{
						putch(*string_record);
						char_num++;
						string_record++;
					}
					break; 
				case 'd':	
					int_record = va_arg(ap, int);
					char_num += write_int(int_record);
					break;
				default:
					return -1;
					break;
			}
		}
		fmt++;
	}
	va_end(ap);
	return char_num;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
	int char_num = 0;
	int int_record;
	char char_record;
	char *string_record;
	while(*fmt)
	{
		if(*fmt != '%')
		{
			*out = *fmt;
			out++;
			char_num++;
		}
		else
		{
			fmt++;
			switch(*fmt)
			{
				case '%':	
					*out = '%';
					out++;
					char_num++;
					break;
				case 'c':
					char_record = (char)va_arg(ap, int);
					//这里va_arg参数为int的原因是，C中调用一个不带原型声明的函数，例如可变长参数函数，传入参数执行“默认实际参数提升”。char将提升为int。
					*out = char_record;
					out++;
					char_num++;
					break;
				case 's':	
					string_record = va_arg(ap, char*);
					while(*string_record != 0)
					{
						*out = *string_record;
						out++;
						char_num++;
						string_record++;
					}
					break; 
				case 'd':	
					int_record = va_arg(ap, int);
					char_num += write_int_buffer(int_record, &out);
					break;
				default:
					return -1;
					break;
			}
		}
		fmt++;
	}
	*out = 0;
	return char_num;
}

int sprintf(char *out, const char *fmt, ...) {
	va_list ap;
	int char_num;
	va_start(ap, fmt);
	char_num = vsprintf(out, fmt, ap);
	va_end(ap);
	return char_num;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  int char_num = 0;
	int int_record;
	char char_record;
	char *string_record;
	while(*fmt)
	{
		if(*fmt != '%')
		{
			*out = *fmt;
			out++;
			char_num++;
			if(char_num == n)
				return char_num;
		}
		else
		{
			fmt++;
			switch(*fmt)
			{
				case '%':	
					*out = '%';
					out++;
					char_num++;
					if(char_num == n)
							return char_num;
					break;
				case 'c':
					char_record = (char)va_arg(ap, int);
					//这里va_arg参数为int的原因是，C中调用一个不带原型声明的函数，例如可变长参数函数，传入参数执行“默认实际参数提升”。char将提升为int。
					*out = char_record;
					out++;
					char_num++;
					if(char_num == n)
						return char_num;
					break;
				case 's':	
					string_record = va_arg(ap, char*);
					while(*string_record != 0)
					{
						*out = *string_record;
						out++;
						char_num++;
						if(char_num == n)
							return char_num;
						string_record++;
					}
					break; 
				case 'd':	
					int_record = va_arg(ap, int);
					write_int_buffer_limit(int_record, n, &char_num, &out);
					if(char_num == n)
						return char_num;
					break;
				default:
					return -1;
					break;
			}
		}
		fmt++;
	}
	*out = 0;
	return char_num;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  va_list ap;
	int char_num;
	va_start(ap, fmt);
	char_num = vsnprintf(out, n, fmt, ap);
	va_end(ap);
	return char_num;
}
#endif
