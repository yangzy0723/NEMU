#include <common.h>
#include "syscall.h"
#include <sys/time.h>
#include <proc.h>

int fs_open(const char *pathname, int flags, int mode);
size_t fs_lseek(int fd, size_t offset, int whence);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
int fs_close(int fd);
void naive_uload(PCB *pcb, const char *filename);
int execve(char *filename, char *const argv[], char *const envp[]);

void do_syscall(Context *c) {

#ifdef STRACE
	printf("\nSystem call trace is triggered:\nmcause\tGPR1\tGPR2\tGPR3\tGPR4\n%p\t%p\t%p\t%p\t%p\n\n", c->mcause, c->GPR1, c->GPR2, c->GPR3, c->GPR4);
#endif
	
  uintptr_t a[4];
  a[0] = c->GPR1;//存的是type,a17
	a[1] = c->GPR2;//_syscall_第二个参数,a0
	a[2] = c->GPR3;//_syscall_第三个参数,a1
	a[3] = c->GPR4;//_syscall_第四个参数,a2
  switch (a[0]) {
		case SYS_exit: execve("/bin/nterm", NULL, NULL); c->GPRx = 0; break;

		case SYS_yield: yield(); c->GPRx = 0; break; 

		case SYS_brk: c->GPRx = 0; break;

		case SYS_open: 
			{
				c->GPRx = fs_open((const char *)a[1], (int)a[2], (int)a[3]);
				if(c->GPRx == -1)
					panic("opening files fails!");
			}; break;

		case SYS_lseek:
			{
				c->GPRx = fs_lseek(a[1], a[2], a[3]); 
				if(c->GPRx == -1)
					panic("seeking the offset fails!");
			}; break;//在函数实现过程中，未考虑返回-1情况，见fs.c

		case SYS_read:
			{
				c->GPRx = fs_read(a[1], (void *)a[2], a[3]);
				if(c->GPRx == -1)
					panic("reading from file fails!");
			}; break;//在函数实现过程中,从STDOUT，STDERR和STDIN中读才会触发，见fs.c

		case SYS_write:	
				{
					c->GPRx = fs_write(a[1], (const void *)a[2], a[3]);
					if(c->GPRx == -1)
						panic("writing files fails!");//在我的实现里，只有写到STDIN才会触发，见fs.c
				};break;

		case SYS_close:
			{
				c->GPRx = fs_close(a[1]);
				if(c->GPRx == -1)
					panic("closing files fails!");
			}; break;//在函数实现过程中，未考虑返回-1情况，见fs.c
		
		case SYS_time:
			{
				struct timeval *my_time = (struct timeval*)(a[1]);
				my_time->tv_sec = io_read(AM_TIMER_UPTIME).us / 1000000;
				my_time->tv_usec = io_read(AM_TIMER_UPTIME).us % 1000000;//在自己的机器上跑，发现usec总不会超过1,000,000，故应该模一下
				c->GPRx = 0;//想不出来出错的情况，总是返回0	
			};break;
		
		case SYS_execve:
			{
				printf("%s\n", (char *)a[1]);
				c->GPRx = execve((char *)a[1], (char **)a[2], (char **)a[3]);
			};break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
