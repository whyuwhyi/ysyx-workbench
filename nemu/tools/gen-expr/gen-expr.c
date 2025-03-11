/***************************************************************************************
* Copyright (c) 2014-2024 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

static void gen_rand_expr(int *pos);

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    int pos = 0;
    gen_rand_expr(&pos);

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    ret = fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}

static void gen_num(int *pos) {
  int num = rand()%1000 - 500;
  sprintf(buf + *pos, "%d", num);
  *pos += strlen(buf + *pos);
}

static void gen_no_zero_num(int *pos) {
  int num = rand() % 1000 + 1;
  sprintf(buf + *pos, "%d", num);
  *pos += strlen(buf + *pos);
}

static void gen_ch(int *pos, char ch) {
  buf[*pos] = ch;
  *pos += 1;
}

static int gen_rand_op(int *pos) {
  int op = rand() % 14;
  switch (op) {
    case 0: gen_ch(pos, '/'); break;
    case 1: gen_ch(pos, '%'); break;
    case 2: gen_ch(pos, '+'); break;
    case 3: gen_ch(pos, '-'); break;
    case 4: gen_ch(pos, '*'); break;
    case 5: gen_ch(pos, '&'); break;
    case 6: gen_ch(pos, '|'); break;
    case 7: gen_ch(pos, '^'); break;
    case 8: gen_ch(pos, '<'); break;
    case 9: gen_ch(pos, '>'); break;
    case 10: gen_ch(pos, '='); gen_ch(pos, '='); break;
    case 11: gen_ch(pos, '!'); gen_ch(pos, '='); break;
    case 12: gen_ch(pos, '&'); gen_ch(pos, '&'); break;
    case 13: gen_ch(pos, '|'); gen_ch(pos, '|'); break;
    default: ;
  }
  return op;
}

static void gen_rand_expr(int *pos) {
  if (*pos >= 60000) {
    gen_num(pos);
  }

  switch (rand()%3) {
    case 0: gen_num(pos); break;
    case 1: gen_ch(pos, '('); gen_rand_expr(pos); gen_ch(pos, ')'); break;
    case 2: gen_rand_expr(pos);
            if (gen_rand_op(pos) <= 1 ) gen_no_zero_num(pos);
            else gen_rand_expr(); 
            break;
    default: ;
  }
}

