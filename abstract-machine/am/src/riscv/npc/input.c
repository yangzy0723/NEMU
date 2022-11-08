#include <am.h>

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  kbd->keydown = inl(KBD_ADDR)>>15;
  kbd->keycode = inl(KBD_ADDR)&0x000000ff;
}
