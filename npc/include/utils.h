/***************************************************************************************
 * Copyright (c) 2014-2024 Zihao Yu, Nanjing University
 *
 * NPC is licensed under Mulan PSL v2.
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

#ifndef __UTILS_H__
#define __UTILS_H__

#include <common.h>

// ----------- log -----------

bool log_enable();
void init_log(const char *log_file);
void log_set_enable(bool enable);

extern FILE *log_fp;

#define log_write(...)                                                         \
  do {                                                                         \
    extern bool log_enable();                                                  \
    extern FILE *log_fp;                                                       \
    if (log_enable() && log_fp != NULL) {                                      \
      fprintf(log_fp, __VA_ARGS__);                                            \
      fflush(log_fp);                                                          \
    }                                                                          \
  } while (0)

#define _Log(...)                                                              \
  do {                                                                         \
    printf(__VA_ARGS__);                                                       \
    log_write(__VA_ARGS__);                                                    \
  } while (0)

#endif
