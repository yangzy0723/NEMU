#include <common.h>
#include "syscall.h"

//#define STRACE 1
void do_syscall(Context *c) {

#ifdef STRACE
	printf("\nSystem call trace is triggered:\nmcause\tGPR1\tGPR2\tGPR3\tGPR4\n%p\t%p\t%p\t%p\t%p\n\n", c->mcause, c->GPR1, c->GPR2, c->GPR3, c->GPR4);
#endif
	
  uintptr_t a[4];
  a[0] = c->GPR1;//存的是type
	a[1] = c->GPR2;//_syscall_第二个参数
	a[2] = c->GPR3;//_syscall_第三个参数
	a[3] = c->GPR4;//_syscall_第四个参数
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
		case SYS_brk:
				{
					printf("123\n");
					malloc(a[1]);
					c->GPRx = 0;
				}; break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
