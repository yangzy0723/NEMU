#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;

static int fd_event = 0;
static int fd_dispinfo = 0;
static int fd_fb = 0;


uint32_t NDL_GetTicks() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_usec / 1000;
    //return 0;
}

int NDL_PollEvent(char *buf, int len) {
    return read(fd_event, buf, len);
    return 0;
}

void NDL_OpenCanvas(int *w, int *h) {
    char temp[64];
    read(fd_dispinfo, temp, sizeof(temp));
    screen_w = 0, screen_h = 0;
    int i = 0;
    for (; temp[i] != ' '; ++i) {
        screen_w *= 10;
        screen_w += temp[i] - '0';
    }
    i++;
    for (; temp[i] != '\0'; ++i) {
        screen_h *= 10;
        screen_h += temp[i] - '0';
    }
    if (*w == 0 && *h == 0)
        *w = screen_w, *h = screen_h;
    if (getenv("NWM_APP")) {
        int fbctl = 4;
        fbdev = 5;
        screen_w = *w;
        screen_h = *h;
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
    //lseek(fd_fb, x + y * screen_w, 0);
    for (int i = 0; i < h; ++i) {
        lseek(fd_fb, x + (y + i) * screen_w, 0);
        //for (int j = 0; j < w; ++j) {
        write(fd_fb, &pixels[i * w], sizeof(uint32_t) * w);
        //}
    }
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


    fd_event = open("/dev/events", 0, 0);
    fd_dispinfo = open("/proc/dispinfo", 0, 0);
    fd_fb = open("/dev/fb", 0, 0);
    return 0;
}

void NDL_Quit() {
}
