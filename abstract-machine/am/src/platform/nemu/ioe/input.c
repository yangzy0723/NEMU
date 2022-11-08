#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
	uint32_t am_scan_code = inl(KBD_ADDR);
  kbd->keydown = am_scan_code >> 15;
  kbd->keycode = am_scan_code & 0x000000ff;
}
