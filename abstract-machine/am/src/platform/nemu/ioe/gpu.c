#include <am.h>
#include <nemu.h>
#include <stdio.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
  // int i;
  // int w = io_read(AM_GPU_CONFIG).width;  // TODO: get the correct width
  // int h = io_read(AM_GPU_CONFIG).height;  // TODO: get the correct height
  // uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  // for (i = 0; i < w * h; i ++) fb[i] = i;
  // outl(SYNC_ADDR, 1);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  uint32_t info = inl(VGACTL_ADDR);
  uint16_t height = (uint16_t)(info & 0xFFFF);
  uint16_t width = (uint16_t)(info >> 16);
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = width, .height = height,
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  int wth = io_read(AM_GPU_CONFIG).width;
  uint32_t *fb_addr = (uint32_t *)FB_ADDR;
  uint32_t *p_addr = (uint32_t *)ctl->pixels;
  for (int i = 0; i < ctl->h; ++i) {
    for (int j = 0; j < ctl->w; ++j) {
      fb_addr[(ctl->y) * wth + i * wth + ctl->x + j] = p_addr[i * (ctl->w) + j];
    }
  }
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
