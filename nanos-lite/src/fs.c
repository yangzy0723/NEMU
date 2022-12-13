#include <fs.h>

//声明
size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);
size_t invalid_read(void *buf, size_t offset, size_t len);
size_t invalid_write(const void *buf, size_t offset, size_t len);
size_t serial_write(const void *buf, size_t offset, size_t len);

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
	size_t open_offset;
  ReadFn read;		//这应该是一个函数,对应的是fs_read
  WriteFn write;	//这应该也是一个函数,对应的是fs_write
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_READ_FROM_KB};

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, 0, invalid_read, serial_write},
  [FD_STDERR] = {"stderr", 0, 0, 0, invalid_read, serial_write},
  [FD_READ_FROM_KB] = {"/dev/events", 0, 0, 0, events_read, invalid_write},
#include "files.h"
};

void init_fs() {
  // TODO: initialize the size of /dev/fb
}

int fs_open(const char *pathname, int flags, int mode)
{
	int Num_Of_File_Table = sizeof(file_table)/sizeof(Finfo);
	for(int i = 0; i < Num_Of_File_Table; i++)
		if(strcmp(file_table[i].name, pathname) == 0)
		{
#ifdef STRACE
			printf("open the file: %s\n", file_table[i].name);
#endif
			return i;
		}
	return -1;
}

size_t fs_read(int fd, void *buf, size_t len)
{
	printf("123\n");
	if(file_table[fd].read != NULL)
		return file_table[fd].read(buf, 0, len);//从键盘读入不考虑偏移量啦
	else if(fd == FD_STDIN || fd == FD_STDOUT || fd == FD_STDERR)
		return -1;
	if(len > file_table[fd].size - file_table[fd].open_offset)
		len = file_table[fd].size - file_table[fd].open_offset; 
	ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
	file_table[fd].open_offset += len;
	return len;
}

size_t fs_write(int fd, const void *buf, size_t len)
{
	if(file_table[fd].write != NULL)
		return file_table[fd].write(buf, 0, len);//写到串口不考虑偏移量啦
	else if(fd == FD_STDIN)
		return -1;
	else
	{
		if(len > file_table[fd].size - file_table[fd].open_offset)
			len = file_table[fd].size - file_table[fd].open_offset;
		ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len);
		file_table[fd].open_offset += len;
	}
	return len;
}

size_t fs_lseek(int fd, size_t offset, int whence)
{
	switch(whence)
	{
		case SEEK_SET: file_table[fd].open_offset = offset; break;
		case SEEK_CUR: file_table[fd].open_offset += offset; break;
		case SEEK_END: file_table[fd].open_offset = file_table[fd].size + offset; break;
	}
	return file_table[fd].open_offset;
}

int fs_close(int fd)
{
#ifdef STRACE
	printf("close the file: %s\n", file_table[fd].name);
#endif
	file_table[fd].open_offset = 0;
	return 0;
}

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}
