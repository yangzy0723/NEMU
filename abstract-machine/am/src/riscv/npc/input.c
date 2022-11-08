#include <am.h>

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  uint32_t keyboard_value = inl(KBD_ADDR);
  kbd->keydown = keyboard_value>>15;
  kbd->keycode = keyboard_value&0x000000ff;
}
