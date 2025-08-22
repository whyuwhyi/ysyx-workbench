#include <defs.h>
FILE *log_fp = NULL;
static bool log_enable_flag = true;

void init_log(const char *log_file) {
  log_fp = NULL;
  if (log_file != NULL) {
    FILE *fp = fopen(log_file, "w");
    Assert(fp, "Can not open '%s'", log_file);
    log_fp = fp;
    Log("Log is written to %s", log_file);
  } else {
    Log("Log is written to stdout");
  }
}

bool log_enable() { return log_enable_flag; }

void log_set_enable(bool enable) { log_enable_flag = enable; }

void assert_fail_msg() {
  Log("Assertion failure at pc = 0x%08x", npc_state.halt_pc);
}
