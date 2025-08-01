/***************************************************************************************
 * Copyright (c) 2014-2024 Zihao Yu, Nanjing University
 *
 * NEMU is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan
 *PSL v2. You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 *
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
 *KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
 *NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 *
 * See the Mulan PSL v2 for more details.
 ***************************************************************************************/

#include <SDL2/SDL.h>
#include <common.h>
#include <device/map.h>

enum {
  reg_freq,
  reg_channels,
  reg_samples,
  reg_sbuf_size,
  reg_sbuf_head,
  reg_sbuf_tail,
  reg_init,
  nr_reg
};

static uint8_t *sbuf = NULL;
static uint32_t *audio_base = NULL;

static void audio_fill_cb(void *userdata, uint8_t *stream, int len) {
  SDL_memset(stream, 0, len);

  uint32_t sbuf_size = audio_base[reg_sbuf_size];
  uint32_t head = audio_base[reg_sbuf_head];
  uint32_t tail = audio_base[reg_sbuf_tail];

  uint32_t available = (tail - head + sbuf_size) % sbuf_size;
  if (available == 0)
    return;

  int real_len = (len < available) ? len : available;

  for (int i = 0; i < real_len; i++) {
    stream[i] = sbuf[(head + i) % sbuf_size];
  }

  audio_base[reg_sbuf_head] = (head + real_len) % sbuf_size;
}

static void audio_io_handler(uint32_t offset, int len, bool is_write) {
  if (offset == sizeof(uint32_t) * reg_init) {
    if (audio_base[reg_init]) {
      SDL_AudioSpec spec = {.freq = audio_base[reg_freq],
                            .format = AUDIO_S16SYS,
                            .channels = audio_base[reg_channels],
                            .samples = audio_base[reg_samples],
                            .callback = audio_fill_cb,
                            .userdata = NULL};
      SDL_InitSubSystem(SDL_INIT_AUDIO);
      int ret = SDL_OpenAudio(&spec, NULL);
      Assert(ret == 0, "SDL_OpenAudio failed");
      SDL_PauseAudio(0);
    }
  }
}

void init_audio() {
  uint32_t space_size = sizeof(uint32_t) * nr_reg;
  audio_base = (uint32_t *)new_space(space_size);
  audio_base[reg_sbuf_head] = 0;
  audio_base[reg_sbuf_tail] = 0;
#ifdef CONFIG_HAS_PORT_IO
  add_pio_map("audio", CONFIG_AUDIO_CTL_PORT, audio_base, space_size,
              audio_io_handler);
#else
  add_mmio_map("audio", CONFIG_AUDIO_CTL_MMIO, audio_base, space_size,
               audio_io_handler);
#endif

  sbuf = (uint8_t *)new_space(CONFIG_SB_SIZE);
  add_mmio_map("audio-sbuf", CONFIG_SB_ADDR, sbuf, CONFIG_SB_SIZE, NULL);
}
