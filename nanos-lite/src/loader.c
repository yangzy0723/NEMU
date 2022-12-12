#include <proc.h>
#include <elf.h>

//声明一下
size_t ramdisk_read(void *buf, size_t offset, size_t len);//声明一下
int fs_open(const char *pathname, int flags, int mode);
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
	fs_read(fd, &elf, sizeof(elf));
	fs_close(fd);
	assert(*(uint32_t*)elf.e_ident == 0x464c457f);//小端方式
	for(int i = 0; i < elf.e_phnum; i++)
	{
		Elf_Phdr segment;
		ramdisk_read(&segment, i * elf.e_phentsize + elf.e_phoff, sizeof(segment));
		if(segment.p_type == PT_LOAD)
		{
			void *VAddr = (void *)segment.p_vaddr;
			ramdisk_read(VAddr, segment.p_offset, segment.p_filesz);
			memset(VAddr + segment.p_filesz, 0, segment.p_memsz - segment.p_filesz);
		}
	}
	printf("123\n");
  return elf.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

/*
ELF Header:
  Magic:   7f 45 4c 46 01 01 01 00 00 00 00 00 00 00 00 00 
  Class:                             ELF32
  Data:                              2's complement, little endian
  Version:                           1 (current)
  OS/ABI:                            UNIX - System V
  ABI Version:                       0
  Type:                              EXEC (Executable file)
  Machine:                           RISC-V
  Version:                           0x1
  Entry point address:               0x80000000
  Start of program headers:          52 (bytes into file)
  Start of section headers:          42336 (bytes into file)
  Flags:                             0x0
  Size of this header:               52 (bytes)
  Size of program headers:           32 (bytes)
  Number of program headers:         3
  Size of section headers:           40 (bytes)
  Number of section headers:         14
  Section header string table index: 13
  */

