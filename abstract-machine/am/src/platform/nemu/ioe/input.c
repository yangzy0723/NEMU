#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
	uint32_t am_scan_code = inl(KBD_ADDR);
  kbd->keydown = am_scan_code >> 15;//第十六位是特征码，确定是按下按键还是抬起按键。
  kbd->keycode = am_scan_code & 0x000000ff;//0-7位是按键的am对应码
}
