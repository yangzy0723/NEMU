#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;
static struct timeval now_time;

uint32_t NDL_GetTicks() {
  gettimeofday(&now_time, NULL);
	return now_time.tv_sec * 1000 + now_time.tv_usec / 1000;
}

int NDL_PollEvent(char *buf, int len) {
  int fd = open("/dev/events", O_RDONLY);//以只读方式打开
	return read(fd, (void *)buf, len);
}

void NDL_OpenCanvas(int *w, int *h) {
	if(*w  == 0 && *h == 0)
	{
		*w = screen_w;
		*h = screen_h;
	}
  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
	int fd = open("/dev/fb", O_RDWR);
	for(int i = 0; i < h; i++)//一次写一行的像素颜色信息进去
	{
		lseek(fd, ((y + i) * screen_w + x) * 4, SEEK_SET);
		write(fd, pixels + w * i, w * 4);
	}
	close(fd);
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }

	//timer initialized
	now_time.tv_usec = 0;
	now_time.tv_sec = 0;

  //display monitor initialized
	int fd = open("/proc/dispinfo", 0);
	char buf[4096];
	read(fd, buf, sizeof(buf));
	close(fd);
	int my_weight = 0;
	int weight_constant = 8;
	int count = 0;
	while('0' <= buf[weight_constant] && buf[weight_constant] <= '9')
	{
		my_weight = my_weight * 10 + *(buf + weight_constant) - '0';
		weight_constant++;
	}
	int my_height = 0;
	int height_constant = 19;
	while('0' <= buf[height_constant] && buf[height_constant] <= '9')
	{
		my_height = my_height * 10 + *(buf + height_constant) - '0';
		height_constant++;
	}
	screen_w = my_weight;
	screen_h = my_height;
	return 0;
}

void NDL_Quit() {
	//timer quit
	now_time.tv_usec = 0;
	now_time.tv_sec = 0;
}
