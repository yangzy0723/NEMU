#include <am.h>
#include <nemu.h>
#include <klib.h>

static AddrSpace kas = {};
static void* (*pgalloc_usr)(int) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static Area segments[] = {      // Kernel memory mappings
  NEMU_PADDR_SPACE
};

#define USER_SPACE RANGE(0x40000000, 0x80000000)

static inline void set_satp(void *pdir) {
  uintptr_t mode = 1ul << (__riscv_xlen - 1);
  asm volatile("csrw satp, %0" : : "r"(mode | ((uintptr_t)pdir >> 12)));
}

static inline uintptr_t get_satp() {
  uintptr_t satp;
  asm volatile("csrr %0, satp" : "=r"(satp));
  return satp << 12;
}

bool vme_init(void* (*pgalloc_f)(int), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  kas.ptr = pgalloc_f(PGSIZE);

  int i;
  for (i = 0; i < LENGTH(segments); i ++) {
    void *va = segments[i].start;
    for (; va < segments[i].end; va += PGSIZE) {
      map(&kas, va, va, 0);
    }
  }

  set_satp(kas.ptr);
  vme_enable = 1;

  return true;
}

void protect(AddrSpace *as) {
  PTE *updir = (PTE*)(pgalloc_usr(PGSIZE));
  as->ptr = updir;
  as->area = USER_SPACE;
  as->pgsize = PGSIZE;
  // map kernel space
  memcpy(updir, kas.ptr, PGSIZE);
}

void unprotect(AddrSpace *as) {
}

void __am_get_cur_as(Context *c) {
  c->pdir = (vme_enable ? (void *)get_satp() : NULL);
}

void __am_switch(Context *c) {
  if (vme_enable && c->pdir != NULL) {
    set_satp(c->pdir);
  }
}

static inline uintptr_t GET_DIR(uintptr_t p)//页目录索引
{
	return (p & (uintptr_t)0xffc00000) >> 22;
}

static inline uintptr_t GET_PAGE(uintptr_t p)//页表索引
{
	return (p & (uintptr_t)0x003ff000) >> 12;
}

static inline uintptr_t GET_OFFSET(uintptr_t p)//页内偏移量
{
	return p & 0x00000fff;
}

void map(AddrSpace *as, void *va, void *pa, int prot) {
	//参考ICS课本图6.45
	/*uintptr_t page_directory_entry = get_satp();
	
	PTE *page_directory_item_entry = page_directory_entry + GET_DIR((uintptr_t)va) * 4;
	//此处需要*4得到对应的页目录项，因为每个页目录项都是4个字节的
	
	if((*page_directory_item_entry) & 1 == 0)//研究其有效位是否为0,若为0说明二级表未分配
	{
		(*page_directory_item_entry) = 
	}*/
}

Context *ucontext(AddrSpace *as, Area ustack, void *entry) {
  Context *context = ustack.end - sizeof(Context);
	context->mepc = (uintptr_t)entry;
	context->mstatus = 0x1800;
	return context;
}
