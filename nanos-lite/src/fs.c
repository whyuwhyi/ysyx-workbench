// #include <cstddef>
#include <fs.h>

typedef size_t (*ReadFn)(void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn)(const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;

enum { FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB };

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
    [FD_STDIN] = {"stdin", 0, 0, invalid_read, invalid_write},
    [FD_STDOUT] = {"stdout", 0, 0, invalid_read, invalid_write},
    [FD_STDERR] = {"stderr", 0, 0, invalid_read, invalid_write},
#include "files.h"
};

#define FD_MAX (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  // TODO: initialize the size of /dev/fb
}

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

int fs_open(const char *pathname, int flags, int mode) {
  for (int i = 0; i < FD_MAX; i++) {
    if (strcmp(pathname, file_table[i].name) == 0) {
      return i;
    }
  }
  assert(0);
}

size_t fs_read(int fd, void *buf, size_t len) {
  if (fd <= 0 || fd >= FD_MAX) {
    return -1;
  }
  if (fd == FD_STDIN || fd == FD_STDOUT || fd == FD_STDERR) {
    return 0;
  }
  Finfo *f = &file_table[fd];

  size_t read_len =
      (len + f->open_offset > f->size) ? (f->size - f->open_offset) : len;
  ramdisk_read(buf, f->disk_offset + f->open_offset, read_len);
  f->open_offset += read_len;
  return read_len;
}

size_t fs_write(int fd, const void *buf, size_t len) {
  if (fd <= 0 || fd >= FD_MAX) {
    return -1;
  }
  if (fd == FD_STDOUT || fd == FD_STDERR) {
    for (size_t i = 0; i < len; i++) {
      putch(((char *)buf)[i]);
    }
    return len;
  }
  Finfo *f = &file_table[fd];

  size_t write_len =
      (len + f->open_offset > f->size) ? (f->size - f->open_offset) : len;
  ramdisk_write(buf, f->disk_offset + f->open_offset, write_len);
  f->open_offset += write_len;
  return write_len;
}

size_t fs_lseek(int fd, size_t offset, int whence) {
  if (fd < 0 || fd >= FD_MAX) {
    return -1;
  }
  Finfo *f = &file_table[fd];
  size_t new_offset;

  switch (whence) {
  case SEEK_SET:
    new_offset = offset > f->size ? f->size : offset;
    break;
  case SEEK_CUR:
    new_offset = f->open_offset + offset;
    if (new_offset > f->size)
      new_offset = f->size;
    if (new_offset < 0)
      new_offset = 0;
    break;
  case SEEK_END:
    new_offset = f->size + offset;
    if (new_offset > f->size)
      new_offset = f->size;
    if (new_offset < 0)
      new_offset = 0;
    break;
  default:
    panic("should not reach here");
  }
  return f->open_offset = new_offset;
}

int fs_close(int fd) { return 0; }
