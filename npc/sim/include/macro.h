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

#ifndef __MACRO_H__
#define __MACRO_H__

#include <string.h>

#define concat_temp(x, y) x##y
#define concat(x, y) concat_temp(x, y)
#define concat3(x, y, z) concat(concat(x, y), z)
#define concat4(w, x, y, z) concat3(concat(w, x), y, z)
#define concat5(v, w, x, y, z) concat4(concat(v, w), x, y, z)

#define str_temp(x) #x
#define str(x) str_temp(x)

#define STRLEN(CONST_STR) (sizeof(CONST_STR) - 1)

#define ARRLEN(arr) (int)(sizeof(arr) / sizeof(arr[0]))

#define CHOOSE2nd(a, b, ...) b
#define MUX_WITH_COMMA(contain_comma, a, b) CHOOSE2nd(contain_comma a, b)
#define MUX_MACRO_PROPERTY(p, macro, a, b)                                     \
  MUX_WITH_COMMA(concat(p, macro), a, b)

#define __P_DEF_0 X,
#define __P_DEF_1 X,
#define __P_ONE_1 X,
#define __P_ZERO_0 X,

#define MUXDEF(macro, X, Y) MUX_MACRO_PROPERTY(__P_DEF_, macro, X, Y)
#define MUXNDEF(macro, X, Y) MUX_MACRO_PROPERTY(__P_DEF_, macro, Y, X)
#define MUXONE(macro, X, Y) MUX_MACRO_PROPERTY(__P_ONE_, macro, X, Y)
#define MUXZERO(macro, X, Y) MUX_MACRO_PROPERTY(__P_ZERO_, macro, X, Y)

#define ISDEF(macro) MUXDEF(macro, 1, 0)
#define ISNDEF(macro) MUXNDEF(macro, 1, 0)
#define ISONE(macro) MUXONE(macro, 1, 0)
#define ISZERO(macro) MUXZERO(macro, 1, 0)

#define MAP(c, f) c(f)

#define ROUNDUP(a, sz) ((((uintptr_t)a) + (sz) - 1) & ~((sz) - 1))
#define ROUNDDOWN(a, sz) ((((uintptr_t)a)) & ~((sz) - 1))

#define PG_ALIGN __attribute((aligned(4096)))

#define __IGNORE(...)
#define __KEEP(...) __VA_ARGS__

#define IFDEF(cond, ...) MUXDEF(cond, __KEEP, __IGNORE)(__VA_ARGS__)
#define IFNDEF(cond, ...) MUXNDEF(cond, __KEEP, __IGNORE)(__VA_ARGS__)

#define __ANSI(fmt, str) "\033[" str "m" fmt "\033[0m"
#define ANSI_FMT(fmt, str) __ANSI(fmt, str)

#define ANSI_FG_BLACK "30"
#define ANSI_FG_RED "31"
#define ANSI_FG_GREEN "32"
#define ANSI_FG_YELLOW "33"
#define ANSI_FG_BLUE "34"
#define ANSI_FG_MAGENTA "35"
#define ANSI_FG_CYAN "36"
#define ANSI_FG_WHITE "37"

#define ANSI_BG_BLACK "40"
#define ANSI_BG_RED "41"
#define ANSI_BG_GREEN "42"
#define ANSI_BG_YELLOW "43"
#define ANSI_BG_BLUE "44"
#define ANSI_BG_MAGENTA "45"
#define ANSI_BG_CYAN "46"
#define ANSI_BG_WHITE "47"

#endif
