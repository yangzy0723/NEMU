#include <common.h>

void do_syscall(Context *c);
Context* schedule(Context *prev);
extern Context* (*user_handler)(Event, Context*);
static Context* do_event(Event e, Context* c) {
  switch (e.event) {
		case EVENT_YIELD: c = schedule(c); break;//返回新的上下文 
		case EVENT_SYSCALL: do_syscall(c); break;
    default: panic("Unhandled event ID = %d", e.event);break;
  }
	printf("in do_event user_handler:%p\n", user_handler);
  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
