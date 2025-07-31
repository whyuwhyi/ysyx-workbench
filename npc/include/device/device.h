#ifndef __DEVICE_H__
#define __DEVICE_H__

#define SERIAL_ADDR 0xa00003F8
#define TIMER_ADDR 0xa0000048

void init_device(void);
void serial_init(void);
void timer_init(void);

void serial_putchar(char c);

int timer_get_timer(void);

#endif
