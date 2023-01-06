#include <proc.h>
#include <elf.h>
#include <fs.h>

//声明一下
size_t ramdisk_read(void *buf, size_t offset, size_t len);
int fs_open(const char *pathname, int flags, int mode);
size_t fs_lseek(int fd, size_t offset, int whence);
size_t fs_read(int fd, void *buf, size_t len);
int fs_close(int fd);

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

static uintptr_t loader(PCB *pcb, const char *filename) {

	int fd = fs_open(filename, 0, 0);

	Elf_Ehdr elf;
	
	fs_read(fd, &elf, sizeof(elf));//由于elf头一定是某一个ELF文件的头部文件，直接读取对应大小即可,不需要fs_lseek
	
	assert(*(uint32_t*)elf.e_ident == 0x464c457f);//小端方式
	for(int i = 0; i < elf.e_phnum; i++)
	{
		Elf_Phdr segment;
		
		fs_lseek(fd, i * elf.e_phentsize + elf.e_phoff, SEEK_SET);
		fs_read(fd, &segment, sizeof(segment));
		
		if(segment.p_type == PT_LOAD)
		{
			void *VAddr = (void *)segment.p_vaddr;
			
			fs_lseek(fd, segment.p_offset, SEEK_SET);
			fs_read(fd, VAddr, segment.p_filesz);
			
			memset(VAddr + segment.p_filesz, 0, segment.p_memsz - segment.p_filesz);
		}
	}

	fs_close(fd);
  
	return elf.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void(*entry)(void *), void *arg)
{
	Area kstack;
	kstack.start = &(pcb->cp);
	kstack.end = kstack.start + sizeof(PCB);
	pcb->cp = kcontext(kstack, entry, arg);
}

void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[])
{
	protect(&(pcb->as));//可以理解为创建了一个进程的页目录，同时腾出了(0x40000000, 0x80000000)的用户进程空间
	void *alloc_p_end = new_page(8);//32KB
	for(int i = 1; i <= 8; i++)
		map(&(pcb->as), (pcb->as).area.end - i * PGSIZE, alloc_p_end - i * PGSIZE, 1);//都是以页为单位	
	//此处需要先进行栈的维护，进行argv和envp的处理，否则envp信息会丢失
	//count
	printf("123\n");
	int num_argv = 0;
	int num_envp = 0;
	while(argv != NULL && argv[num_argv] != NULL)
	{
		num_argv++;
	}
	while(envp != NULL && envp[num_envp] != NULL)
	{
		num_envp++;
	}

	//deal with string-area
	char *record_position_argv[num_argv];
	char *record_position_envp[num_envp];
	char *p = (pcb->as).area.end;
	for(int i = 0; i < num_argv; i++)
	{
		p = p - strlen(argv[i]) - 1;
		strcpy(p, argv[i]);
		record_position_argv[i] = p;
	}
	for(int i = 0; i < num_envp; i++)
	{
		p = p - strlen(envp[i]) - 1;
		strcpy(p, envp[i]);
		record_position_envp[i] = p;
	}

	//deal with envp array and argv array
	p = (char *)((uint32_t)p - (uint32_t)p%4);//对齐
	uintptr_t *point = (uintptr_t *)p;
	point = point - num_envp - 1;
	for(int i = 0; i < num_envp; i++)
		point[i] = (uintptr_t)record_position_envp[i];
	point[num_envp] = 0x0;
	point = point - num_argv - 1;
	for(int i = 0; i < num_argv; i++)
		point[i] = (uintptr_t)record_position_argv[i];
	point[num_argv] = 0x0;
	point = point - 1;
	*point = (uintptr_t)num_argv;

	Area ustack;
	ustack.start = &(pcb->cp);
	ustack.end = ustack.start + sizeof(PCB);
	pcb->cp = ucontext(NULL, ustack, (void *)loader(pcb, filename));
	pcb->cp->GPRx = (uintptr_t)point;
}
