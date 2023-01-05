#include <memory.h>

static void *pf = NULL;

void* new_page(size_t nr_page) {
  pf = pf + nr_page * PGSIZE;
	return pf;
}//每一页分配nr_page*4KB的连续内存区域，得到的是末尾地址，都是对齐了的

#ifdef HAS_VME
static void* pg_alloc(int n) {
	int nr_apge = n / PGSIZE;
	void *ret_point = new_page(nr_page);
	memset(ret_point - nr_page * PGSIZE, 0, nr_page * PGSIZE);
	return ret_point - nr_page * PGSIZE;
}
#endif

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);//赋初值的时候就对齐了，低12位为0
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
