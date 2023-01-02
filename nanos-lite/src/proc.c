#include <proc.h>

#define MAX_NR_PROC 4

void naive_uload(PCB *pcb, const char *filename);
void context_kload(PCB *pcb, void (*entry)(void *), void *arg);
extern Context* (*user_handler)(Event, Context*);
static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
	printf("in hello1: %p\n", user_handler);
  while (1) {
		printf("in hello2: %p\n", user_handler);
    Log("Hello World from Nanos-lite with arg '%p' for the %dth time!", (uintptr_t)arg, j);
    j ++;
		printf("in hello:%p\n", user_handler);
    yield();
		if(j > 3)
			break;
  }
}

void init_proc() {
	context_kload(&pcb[0], hello_fun, "zqy");
	context_kload(&pcb[1], hello_fun, "yzy");
  switch_boot_pcb();

  //Log("Initializing processes...");

  // load program here
	//naive_uload(NULL, "/bin/nterm");
}

Context* schedule(Context *prev) {
  current->cp = prev;
	current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);	
	return current->cp;
}
