#ifndef __ITRACE_H__
#define __ITRACE_H__

strcut RingBuffer {
  uint8_t *buf;
  uint32_t size;
  uint32_t head;
  uint32_t tail;
};

#endif
