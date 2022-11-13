#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
	/*int w = 400;
	int h = 300;
	uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
	for(int i = 0; i < w*h; i++)
		fb[i] = i;
	outl(SYNC_ADDR, 1);*/
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
	uint32_t data_code = inl(VGACTL_ADDR);
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = (data_code >> 16) & 0x0000ffff, .height = data_code & 0x0000ffff,
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) 
{
	uint32_t get_information = inl(VGACTL_ADDR);//获取设备长宽信息
	uint32_t Width = (get_information >> 16) & 0x0000ffff;
	/*uint32_t Height = get_information & 0x0000ffff; 用不到*/
	uint32_t dy_x = ctl -> x;
	uint32_t dy_y = ctl -> y;
	int count = 0;
	uint32_t *pixels_live = (uint32_t *)ctl -> pixels;
  if (ctl->sync) 
    outl(SYNC_ADDR, 1);
	for(int i = 0; i < ctl -> h; i++)
	{
		for(int j = 0; j < ctl -> w; j++)
			outl(FB_ADDR + (Width * dy_y + dy_x + j)*4, pixels_live[i * ctl->w + j]);
		dy_y++;
	}
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
