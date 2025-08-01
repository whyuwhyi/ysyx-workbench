#include <am.h>
#include <nemu.h>
#include <stdint.h>

#define AUDIO_FREQ_ADDR (AUDIO_ADDR + 0x00)
#define AUDIO_CHANNELS_ADDR (AUDIO_ADDR + 0x04)
#define AUDIO_SAMPLES_ADDR (AUDIO_ADDR + 0x08)
#define AUDIO_SBUF_SIZE_ADDR (AUDIO_ADDR + 0x0c)
#define AUDIO_SBUF_HEAD_ADDR (AUDIO_ADDR + 0x10)
#define AUDIO_SBUF_TAIL_ADDR (AUDIO_ADDR + 0x14)
#define AUDIO_INIT_ADDR (AUDIO_ADDR + 0x18)

void __am_audio_init() {}

void __am_audio_config(AM_AUDIO_CONFIG_T *cfg) {
  *cfg = (AM_AUDIO_CONFIG_T){.present = true,
                             .bufsize = (int)inl(AUDIO_SBUF_SIZE_ADDR)};
}

void __am_audio_ctrl(AM_AUDIO_CTRL_T *ctrl) {
  outl(AUDIO_FREQ_ADDR, ctrl->freq);
  outl(AUDIO_CHANNELS_ADDR, ctrl->channels);
  outl(AUDIO_SAMPLES_ADDR, ctrl->samples);
  outl(AUDIO_INIT_ADDR, 1);
}

void __am_audio_status(AM_AUDIO_STATUS_T *stat) {
  uint32_t head = inl(AUDIO_SBUF_HEAD_ADDR);
  uint32_t tail = inl(AUDIO_SBUF_TAIL_ADDR);
  uint32_t sbuf_size = inl(AUDIO_SBUF_SIZE_ADDR);

  stat->count = (tail - head + sbuf_size) % sbuf_size;
}

void __am_audio_play(AM_AUDIO_PLAY_T *ctl) {
  uint8_t *buf = ctl->buf.start;
  int len = ctl->buf.end - ctl->buf.start;

  uint32_t sbuf_size = inl(AUDIO_SBUF_SIZE_ADDR);
  uint8_t *sbuf = (uint8_t *)(uintptr_t)AUDIO_SBUF_ADDR;
  uint32_t head, tail, free_space;

  do {
    head = inl(AUDIO_SBUF_HEAD_ADDR);
    tail = inl(AUDIO_SBUF_TAIL_ADDR);
    free_space = (head - tail - 1 + sbuf_size) % sbuf_size;

  } while (free_space < len);

  for (int i = 0; i < len; i++) {
    sbuf[(tail + i) % sbuf_size] = buf[i];
  }

  tail = (tail + len) % sbuf_size;

  outl(AUDIO_SBUF_TAIL_ADDR, tail);
}
