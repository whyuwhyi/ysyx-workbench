#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000
#define KEYCODE_MASK 0x7fff

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {

  int key_code = (int)inl(KBD_ADDR);
  if (key_code == AM_KEY_NONE) {
    kbd->keydown = false;
    kbd->keycode = AM_KEY_NONE;
  } else if (key_code & KEYDOWN_MASK) {
    kbd->keydown = true;
    kbd->keycode = key_code & KEYCODE_MASK;

  } else {
    kbd->keydown = false;
    kbd->keycode = key_code;
  }
}
