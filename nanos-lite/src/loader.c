#include <proc.h>
#include <elf.h>
#include <fs.h>

//声明一下
size_t ramdisk_read(void *buf, size_t offset, size_t len);//声明一下
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
	if(fd == -1)//此处解决批处理模式中，输入无效字符的问题
	{
		printf("opening file fails!\n");
		return loader(NULL, "/bin/nterm");
	}

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

void context_kload(PCB *pcb, void (*entry)(void *), void *arg)
{
	Area kstack;
	kstack.start = &(pcb->cp);
	kstack.end = kstack.start + sizeof(PCB);
	pcb->cp = kcontext(kstack, entry, arg);
}

void context_uload(PCB *pcb, char* filename)
{
	Area kstack;
	kstack.start = &(pcb->cp);
	kstack.end = kstack.start + sizeof(PCB);
	pcb->cp = ucontext(NULL, kstack, (void *)loader(pcb, filename));
}
