#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

static Context* (*user_handler)(Event, Context*) = NULL;
void __am_get_cur_as(Context *c);
void __am_switch(Context *c);

Context* __am_irq_handle(Context *c) {
	__am_get_cur_as(c);
  if (user_handler) {
    Event ev = {0};
    switch (c->mcause) {
			case -1: ev.event = EVENT_YIELD; break;
      //case 0x80000007: ev.event = EVENT_IRQ_TIMER; break;
			case 0:case 1:case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9: case 10: 
			case 11: case 12: case 13: case 14: case 15: case 16: case 17: case 18: case 19:
							 ev.event = EVENT_SYSCALL; c->mepc +=4; break;
			default: ev.event = EVENT_ERROR; break;
    }

    c = user_handler(ev, c);
    assert(c != NULL);
  }
	__am_switch(c);
  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
	Context *context = kstack.end - sizeof(Context);
	context->mepc = (uintptr_t)entry;
	context->mstatus = 0x1800 | 0x80;
	context->GPR2 = (uintptr_t)arg;
	context->pdir = NULL;
	return context;
}

void yield() {
  asm volatile("li a7, -1; ecall");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
