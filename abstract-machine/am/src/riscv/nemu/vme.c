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
	assert(0);
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

static inline uintptr_t GET_BASE_ADDR(PTE p)//得到基地址
{
	return (uintptr_t)p & 0xfffffc00;
}

//先只考虑有效位
void map(AddrSpace *as, void *va, void *pa, int prot) {
	printf("map: vaddr: %p paddr: %p\n", va, pa);
	//参考ICS课本图6.45和jianshu.com/6780c4ac272e，但要注意这里是riscv32架构
	
	uintptr_t page_directory_entry = (uintptr_t)as->ptr;//页目录的基地址
	
	PTE *page_directory_item_entry = (PTE *)(page_directory_entry + GET_DIR((uintptr_t)va) * 4);
	//10位*4,正好4096字节，一页
	//此处需要*4得到对应的页目录项，因为每个页目录项都是4个字节的
	
	if(((*page_directory_item_entry) & PTE_V) == 0)//研究其有效位是否为0,若为0说明二级表未分配
	{
		(*page_directory_item_entry) = ((*page_directory_item_entry) & 0x000003ff) + (PTE)pgalloc_usr(PGSIZE);
		//高22位，存页表的地址，低2位一定为0
		(*page_directory_item_entry) = ((*page_directory_item_entry) | PTE_V);
		//有效位为1,说明这个页目录项对应的页表有在工作了
	}

	uintptr_t page_table_entry = GET_BASE_ADDR(*page_directory_item_entry);//得到页表基地址
	
	PTE *page_table_item_entry = (PTE *)(page_table_entry + GET_PAGE((uintptr_t)va) * 4);//得到页表项的地址
	(*page_table_item_entry) = ((*page_table_item_entry) & 0x000003ff) + (PTE)GET_BASE_ADDR((uintptr_t)pa >> 2);//取34位的高22位填充
	(*page_table_item_entry) = (*page_table_item_entry) | PTE_V;
	
}

Context *ucontext(AddrSpace *as, Area ustack, void *entry) {
  Context *context = ustack.end - sizeof(Context);
	context->mepc = (uintptr_t)entry;
	context->mstatus = 0x1800;
	return context;
}
