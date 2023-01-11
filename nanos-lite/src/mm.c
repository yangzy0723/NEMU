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

/*The brk() system call handler. */
//理解为传入brk的变化量
extern PCB *current;
int mm_brk(uintptr_t increment) {
	if((int32_t)increment < 0)//此时没有大于等于，不需要申请新的
		return 0;
	else
	{
		int num_page = increment/PGSIZE + 1;
		void *alloc_p_start = new_page(num_page) - PGSIZE * num_page;
		printf("max_brk:%p\n", current->max_brk);
		//printf("%d\n", num_page);
		for(int i = 0; i < num_page; i++)
			map(&(current->as), (void *)((current->max_brk & 0xfffff000) + i * PGSIZE), alloc_p_start + i * PGSIZE, 0);
		current->max_brk = current->max_brk + increment;
		//printf("finish mm_brk!\n");
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
