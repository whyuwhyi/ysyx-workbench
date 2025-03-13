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

static void gen_rand_expr();
int pos = 0;

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    pos = 0;
    gen_rand_expr();

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

static void gen_char(char c) {
  buf[pos++] = c;
  buf[pos] = '\0';
}

static void gen_rand_space() {
  int num = rand() % 3;

  for (int i = 0; i < num; i ++) {
    gen_char(' ');
  }
}

static void gen_rand_num() {
  gen_rand_space();
  int num = rand() % 1000;
  sprintf(buf + pos, "%d", num);
  pos += strlen(buf + pos);
  buf[pos] = '\0';
  gen_rand_space();
}

static void gen_rand_none_zero() {
  gen_rand_space();
  int num = rand()%1000 + 1;
  sprintf(buf + pos, "%d", num);
  pos += strlen(buf + pos);
  buf[pos] = '\0';
  gen_rand_space();
}

static void gen_rand_binary_op() {
  switch (rand() % 14) {
    case 0: gen_char('+'); break;
    case 1: gen_char('-'); break;
    case 2: gen_char('*'); break;
    case 3: gen_char('='); gen_char('='); break;
    case 4: gen_char('!'); gen_char('='); break;
    case 5: gen_char('<'); gen_char('='); break;
    case 6: gen_char('>'); gen_char('='); break;
    case 7: gen_char('<'); break;
    case 8: gen_char('>'); break;
    case 9: gen_char('+'); break;
    case 10: gen_char('-'); break;
    case 11: gen_char('*'); break;
    case 12: gen_char('&'); gen_char('&'); break;
    case 13: gen_char('|'); gen_char('|'); break;
    default: assert(0);
  }
}

static void gen_rand_unary_op() {
  switch (rand() % 2) {
    case 0: gen_char('!'); break;
    case 1: gen_char('!'); break;
    case 2: gen_char('-'); break;
    default: assert(0);
  }
}

static void gen_rand_spec_op() {
  switch (rand() % 2) {
    case 0: gen_char('/'); break;
    case 1: gen_char('%'); break;
    default: assert(0);
  }
}

static void gen_rand_expr() {
  if (pos > 60000) {
    gen_rand_num();
    return ;
  }

  switch (rand() % 5) {
    case 0: gen_rand_num(); break;
    case 1: gen_char('('); gen_rand_expr(); gen_char(')'); break;
    case 2: gen_char('('); gen_rand_unary_op(); gen_rand_expr();  gen_char(')'); break;
    case 3: gen_rand_expr(); gen_rand_binary_op(); gen_rand_expr(); break;
    case 4: gen_rand_expr(); gen_rand_spec_op(); gen_rand_none_zero(); break;
    default: assert(0);
  }
}
