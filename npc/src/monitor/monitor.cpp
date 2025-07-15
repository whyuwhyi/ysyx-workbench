#include <assert.h>
#include <common.h>
#include <cpu/cpu.h>
#include <memory/pmem.h>
#include <stdio.h>

extern void init_sdb();
extern void sdb_mainloop();
extern void sdb_set_batch_mode();

#ifdef CONFIG_ITRACE
extern void init_disasm();
#endif

static bool is_batch_mode = false;

void monitor_init(void) {
  Log("Monitor initializing...");

  // Initialize CPU and subsystems
  init_cpu();

  // Initialize SDB
  init_sdb();

  // Initialize disassembly if itrace is enabled
#ifdef CONFIG_ITRACE
  init_disasm();
#endif

  Log("Monitor initialized");
}

void monitor_exit(void) { Log("Monitor exited"); }

// Set batch mode
void set_batch_mode() {
  is_batch_mode = true;
  sdb_set_batch_mode();
}

// Start the main monitoring loop
void start_monitor() {
  if (!is_batch_mode) {
    Log("Starting interactive debugger...");
    sdb_mainloop();
  }
}

long load_image(char *img_file) {
  Assert(img_file != NULL, "Image file path must not be NULL");

  FILE *fp = fopen(img_file, "rb");
  Assert(fp, "Can not open '%s'", img_file);

  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);

  fseek(fp, 0, SEEK_SET);
  uint8_t *pmem = get_pmem();
  int ret = fread(pmem, size, 1, fp);
  assert(ret == 1);

  fclose(fp);
  Log("Image loaded: %s, size = %ld bytes", img_file, size);

  // Initialize difftest after loading image
#ifdef CONFIG_DIFFTEST
  init_cpu_difftest(const_cast<char *>(CONFIG_DIFFTEST_REF_PATH), size);
#endif

  return size;
}
