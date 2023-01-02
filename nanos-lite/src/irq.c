#include <common.h>

void do_syscall(Context *c);
Context* schedule(Context *prev);
Context* do_event(Event e, Context* c) {
  switch (e.event) {
		case EVENT_YIELD: c = schedule(c); break;//返回新的上下文 
		case EVENT_SYSCALL: do_syscall(c); break;
    default: panic("Unhandled event ID = %d", e.event);break;
  }
  return c;
}//删掉了static，方便在cte.c中供__am_irq_handle使用。

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
