/***************************************************************************************
 * Copyright (c) 2014-2024 Zihao Yu, Nanjing University
 *
 * NEMU is licensed under Mulan PSL v2.
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

#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  char exp[100];
  word_t value;

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

void new_wp(char *exp, word_t value) {
  if (free_ == NULL) {
    printf("No enough watchpoints.\n");
    assert(0);
  }
  assert(strlen(exp) < 100);

  WP *new = free_;
  free_ = free_->next;
  new->next = head;
  head = new;
  strcpy(new->exp, exp);
  new->value = value;
}

void free_wp(int NO) {
  WP *p = head;
  if (p == NULL) {
    printf("No watchpoint with NO %d.\n", NO);
    return;
  }
  if (p->NO == NO) {
    head = p->next;
    p->next = free_;
    free_ = p;
    return;
  }
  while (p->next != NULL) {
    if (p->next->NO == NO) {
      WP *q = p->next;
      p->next = q->next;
      q->next = free_;
      free_ = q;
      return;
    }
    p = p->next;
  }
  printf("No watchpoint with NO %d.\n", NO);
}

void display_wp() {
  WP *p = head;
  if (p == NULL) {
    printf("No watchpoints.\n");
    return;
  }
  printf("Num\t Value\t What\n");
  while (p != NULL) {
    printf("%d\t %d\t %s\n", p->NO, p->value, p->exp);
    p = p->next;
  }
}

void watchpoint_check() {
  WP *p = head;
  while (p != NULL) {
    printf("Checking watchpoint %d: %s\n", p->NO, p->exp);
    bool success = true;
    word_t value = expr(p->exp, &success);
    if (value != p->value) {
      printf("Watchpoint %d: %s\n", p->NO, p->exp);
      printf("Old value = %d\n", p->value);
      printf("New value = %d\n", value);
      printf("The value of watchpoint %d has changed.\n", p->NO);
      printf("The program will stop at the next instruction.\n");
      p->value = value;
      nemu_state.state = NEMU_STOP;
    }
    p = p->next;
  }
}
