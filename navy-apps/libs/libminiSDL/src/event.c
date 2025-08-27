#include <NDL.h>
#include <SDL.h>

#define keyname(k) #k,

static const char *keyname[] = {"NONE", _KEYS(keyname)};

int SDL_PushEvent(SDL_Event *ev) { return 0; }

int SDL_PollEvent(SDL_Event *ev) { return 0; }

int SDL_WaitEvent(SDL_Event *event) {
  char buf[64];
  char type[8], key_name[8];
  int keycode;

  while (1) {
    if (NDL_PollEvent(buf, sizeof(buf)) == 0) {
      continue;
    }
    sscanf(buf, "%s %s %d\n", type, key_name, &keycode);
    event->type = (buf[1] == 'u') ? SDL_KEYUP : SDL_KEYDOWN;
    event->key.keysym.sym = keycode;
    return 1;
  }
  return 0;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

uint8_t *SDL_GetKeyState(int *numkeys) { return NULL; }
