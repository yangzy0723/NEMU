#include <memory.h>
#include <proc.h>

static void *pf = NULL;

void* new_page(size_t nr_page) {
  pf = pf + nr_page * PGSIZE;
	return pf;
}//每一页分配nr_page*4KB的连续内存区域，得到的是末尾地址，都是对齐了的

#ifdef HAS_VME
static void* pg_alloc(int n) {
	int nr_page = n / PGSIZE;
	void *ret_point = new_page(nr_page);
	memset(ret_point - nr_page * PGSIZE, 0, nr_page * PGSIZE);
	return ret_point - nr_page * PGSIZE;
}
#endif

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
//我的理解，传入的参数直接是增量
extern PCB *current;
int mm_brk(uintptr_t brk) {
	if((int32_t)brk < 0)
		return 0;
	else
	{
		printf("%p\n", brk);
		int pre_page = (current->max_brk)/PGSIZE;
		int now_page = (current->max_brk + brk)/PGSIZE;
		int num_new_page = now_page - pre_page;
		void *alloc_p_start = new_page(num_new_page) - PGSIZE * num_new_page;
		for(int i = 0; i < num_new_page; i++)
			map(&(current->as), (void *)((current->max_brk & 0xfffff000) + PGSIZE + i * PGSIZE), alloc_p_start + i * PGSIZE, 0);
		current->max_brk = current->max_brk + brk;
		printf("123\n");
		return 0;
	}
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);//赋初值的时候就对齐了，低12位为0
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
