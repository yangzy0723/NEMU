#include <common.h>
#include "syscall.h"

#define STRACE 1
void do_syscall(Context *c) {

#ifdef STRACE
	printf("System call trace is triggered:\nmcause\tGPR1\tGPR2\tGPR3\tGPR4\n%p\t%p\t%p\t%p\t%p", c->mcause, c->GPR1, c->GPR2, c->GPR3, c->GPR4);
#endif
	
  uintptr_t a[4];
  a[0] = c->GPR1;
  switch (a[0]) {
		case SYS_exit: halt(0);
		case SYS_yield: yield(); c->GPRx = 0; break; 
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
