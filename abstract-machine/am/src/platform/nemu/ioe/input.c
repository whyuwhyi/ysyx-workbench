#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {

  int KEY_CODE = (int)inl(KBD_ADDR);
  if (KEY_CODE == 0) {
    kbd->keydown = false;
    kbd->keycode = AM_KEY_NONE;
  } else if (KEY_CODE & KEYDOWN_MASK) {
    kbd->keydown = true;
    kbd->keycode = KEY_CODE & 0x7fff;
  } else {
    kbd->keydown = false;
    kbd->keycode = KEY_CODE;
  }
}
