#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {

  int key_code = (int)inl(KBD_ADDR);
  if (key_code == 0) {
    kbd->keydown = false;
    kbd->keycode = AM_KEY_NONE;
  } else if (key_code & KEYDOWN_MASK) {
    kbd->keydown = true;
    kbd->keycode = key_code & 0x7fff;
  } else {
    kbd->keydown = false;
    kbd->keycode = key_code;
  }
}
