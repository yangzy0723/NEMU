#include <common.h>
#include "syscall.h"

int fs_open(const char *pathname, int flags, int mode);
size_t fs_lseek(int fd, size_t offset, int whence);
size_t fs_read(int fd, void *buf, size_t len);

#define STRACE 1
void do_syscall(Context *c) {

#ifdef STRACE
	printf("\nSystem call trace is triggered:\nmcause\tGPR1\tGPR2\tGPR3\tGPR4\n%p\t%p\t%p\t%p\t%p\n\n", c->mcause, c->GPR1, c->GPR2, c->GPR3, c->GPR4);
#endif
	
  uintptr_t a[4];
  a[0] = c->GPR1;//存的是type,a17
	a[1] = c->GPR2;//_syscall_第二个参数,a0
	a[2] = c->GPR3;//_syscall_第三个参数,a1
	a[3] = c->GPR4;//_syscall_第四个参数,a2
  switch (a[0]) {
		case SYS_exit: halt(0);

		case SYS_yield: yield(); c->GPRx = 0; break; 

		case SYS_write:
				{
					if(a[1] == 1 || a[1] == 2)
						for(int i = 0; i < a[3]; i++)
							putch(*((char*)a[2] + i));
					c->GPRx = a[3];
				};break;

		case SYS_brk: c->GPRx = 0; break;

		case SYS_open: 
			{
				c->GPRx = fs_open((const char *)a[1], (int)a[2], (int)a[3]);
				if(c->GPRx == -1)
					panic("opening files fails!");
			}; break;

		case SYS_lseek:
			{
				c->GPRx = fs_lseek(a[1], a[2], a[3]); 
				if(c->GPRx == -1)
					panic("seeking the offset fails!");
			}; break;//在函数实现过程中，未考虑返回-1情况，见fs.c

		case SYS_read:
			{
				c->GPRx = fs_read(a[1], (void *)a[2], a[3]);
				if(c->GPRx == -1)
					panic("reading from file fails!");
			}; break;//在函数实现过程中，未考虑返回-1情况，除非读到STDIN，见fs.c
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
