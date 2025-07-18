/***************************************************************************************
* Copyright (c) 2014-2024 Zihao Yu, Nanjing University
*
* NPC is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <common.h>
#include <stdarg.h>

FILE* log_fp = NULL;
extern NPCState npc_state;

static bool log_enable_flag = true;

void init_log(const char *log_file) {
  log_fp = stdout;
  if (log_file != NULL) {
    FILE *fp = fopen(log_file, "w");
    Assert(fp, "Can not open '%s'", log_file);
    log_fp = fp;
  }
  Log("Log is written to %s", log_file ? log_file : "stdout");
}

bool log_enable() {
  return log_enable_flag;
}

void log_set_enable(bool enable) {
  log_enable_flag = enable;
}

void log_write(const char *format, ...) {
  if (!log_enable()) return;
  va_list ap;
  va_start(ap, format);
  vfprintf(log_fp, format, ap);
  va_end(ap);
  fflush(log_fp);
}

void assert_fail_msg() {
  Log("Assertion failure at pc = 0x%08x", npc_state.halt_pc);
}