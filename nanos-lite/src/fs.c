#include <fs.h>
#include <stdint.h>

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

size_t serial_write(const void *buf, size_t offset, size_t len);

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
    [FD_STDIN] = {"stdin", 0, 0, invalid_read, invalid_write},
    [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
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
      file_table[i].open_offset = 0;
      return i;
    }
  }
  assert(0);
}

static inline size_t min_sz(size_t a, size_t b) { return a < b ? a : b; }

size_t fs_read(int fd, void *buf, size_t len) {
  if (fd < 0 || fd >= FD_MAX) {
    return (size_t)-1;
  }
  if (len == 0)
    return 0;
  if (buf == NULL) {
    return (size_t)-1;
  }

  Finfo *f = &file_table[fd];
  if (f->read) {
    return f->read(buf, f->open_offset, len);
  }

  if (f->open_offset >= f->size)
    return 0;

  size_t avail = f->size - f->open_offset;
  size_t n = min_sz(len, avail);
  if (n == 0)
    return 0;

  ramdisk_read(buf, f->disk_offset + f->open_offset, n);
  f->open_offset += n;
  return n;
}

size_t fs_write(int fd, const void *buf, size_t len) {
  if (fd < 0 || fd >= FD_MAX) {
    return (size_t)-1;
  }
  if (len == 0)
    return 0;
  if (buf == NULL) {
    return (size_t)-1;
  }

  Finfo *f = &file_table[fd];
  if (f->write) {
    return f->write(buf, f->open_offset, len);
  }

  if (f->open_offset > f->size) {
    return (size_t)-1;
  }

  size_t avail = f->size - f->open_offset;
  size_t n = min_sz(len, avail);
  if (n == 0) {
    return 0;
  }

  ramdisk_write(buf, f->disk_offset + f->open_offset, n);
  f->open_offset += n;
  return n;
}

size_t fs_lseek(int fd, size_t offset, int whence) {
  if (fd < 0 || fd >= FD_MAX) {
    return (size_t)-1;
  }

  if (fd == FD_STDIN || fd == FD_STDOUT || fd == FD_STDERR) {
    return (size_t)-1;
  }

  Finfo *f = &file_table[fd];

  size_t base = 0;
  switch (whence) {
  case SEEK_SET:
    base = 0;
    break;
  case SEEK_CUR:
    base = (size_t)f->open_offset;
    break;
  case SEEK_END:
    base = (size_t)f->size;
    break;
  default:
    assert(0);
  }

  int64_t new_off = (int64_t)base + (int64_t)offset;
  if (new_off < 0) {
    new_off = 0;
  }
  if (new_off > (int64_t)f->size) {
    new_off = f->size;
  }
  f->open_offset = new_off;
  return (size_t)new_off;
}

int fs_close(int fd) { return 0; }
