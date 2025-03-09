/**************************************************************************************
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

#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ, TK_NEQ, TK_OR, TK_AND, TK_NOT,
  TK_ADD, TK_SUB, TK_MUL, TK_DIV, TK_LPAREN, TK_RPAREN, 
  TK_DEC, TK_HEX, TK_REG, TK_ID, TK_NUM,
  TK_DEREF,
};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {
  {" +", TK_NOTYPE},                          // spaces
  {"\\+", TK_ADD},                            // plus
  {"-", TK_ADD},                              // subtract
  {"\\*", TK_MUL},                            // multiply
  {"/", TK_DIV},                              // divide
  {"\\(", TK_LPAREN},                         // left parenthesis
  {"\\)", TK_RPAREN},                         // right parenthesis
  {"==", TK_EQ},                              // equal
  {"!=", TK_NEQ},                             // not equal
  {"--+-", TK_OR},                            // or
  {"&&", TK_AND},                             // and
  {"!", TK_NOT},                              // not
  {"0[xX][0-9a-fA-F]+", TK_HEX},              // hex number
  {"\\$[a-zA-Z][a-zA-Z0-9_]*", TK_REG},       // register
  {"\\$[a-zA-Z][a-zA-Z0-9_]*", TK_REG},       // register
  {"0[xX][0-9a-fA-F]+", TK_HEX},              // hex number
  {"[0-9]+", TK_DEC},                         // decimal number
  {"[a-zA-Z_][a-zA-Z0-9_]*", TK_ID},          // identifier
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        if (substr_len >= 32) {
          printf("Token string at position %d is too long: %.*s\n",
                 position, substr_len, substr_start);
          return false;
        }

        if (nr_token >= 32) {
          printf("too many tokens\n");
          return false;
        }

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        switch (rules[i].token_type) {
          case TK_NOTYPE:
            break;
          case TK_DEC:
          case TK_HEX:
          case TK_ID:
          case TK_REG:
            tokens[nr_token].type = rules[i].token_type;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            printf("tokens[%d].str = %s\n", nr_token, tokens[nr_token].str);
            tokens[nr_token].str[substr_len] = '\0';
            nr_token++;
            break;
          default:
            tokens[nr_token++].type = rules[i].token_type;
        }
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

static bool check_parentheses(int s, int e, bool * success) {
  if (tokens[s].type != TK_LPAREN || tokens[e].type != TK_RPAREN) {
    return false;
  }

  int level = 0;
  for (int i = s+1; i < e; i++) {
    if (tokens[i].type == TK_LPAREN) {
      level++;
    }
    else if (tokens[i].type == TK_RPAREN) {
      level--;
    }

    if (level < 0) {
      printf("Unmatched parentheses\n");
      *success = false;
      return false;
    }
  }

  return level == 0;
}

static int get_pos_of_main_op(int s, int e) {
  int pos = -1;
  int level = 0;

  for (int i = e; i >= s; i--) {
    if (tokens[i].type == TK_RPAREN) {
      level++;
    }
    else if (tokens[i].type == TK_LPAREN) {
      level--;
    }
    else if (level == 0) {
      if (tokens[i].type == TK_ADD || tokens[i].type == TK_SUB) {
        pos = i;
        break;
      }
      else if (tokens[i].type == TK_EQ || tokens[i].type == TK_NEQ) {
        pos = i;
      }
      else if (tokens[i].type == TK_OR) {
        pos = i;
      }
      else if (tokens[i].type == TK_MUL || tokens[i].type == TK_DIV) {
        pos = i;
      }
      else if (tokens[i].type == TK_AND) {
        pos = i;
      }
    }
  }

  return pos;
}

static word_t eval(int s, int e, bool *success) {
  if (success == false) {
    return 0;
  } 

  if (s > e) {
    *success = false;
    return 0;
  }
  else if (s == e) {
    switch (tokens[s].type) {
      case TK_DEC:
        word_t val = 0;
        sscanf(tokens[s].str, "%d", &val);
        return val;
      case TK_HEX:
        return (word_t)strtol(tokens[s].str, NULL, 16);
      case TK_REG:
        return isa_reg_str2val(tokens[s].str + 1, NULL);
      default:
        *success = false;
        return 0;
    }
  }
  else if (check_parentheses(s, e, success) == true) {
    return eval(s + 1, e - 1, success);
  }
  else {
    int op_pos = get_pos_of_main_op(s, e);
    
    if (op_pos == -1) {
      *success = false;
      return 0;
    }

    word_t left_val = eval(s, op_pos - 1, success);
    word_t right_val = eval(op_pos + 1, e, success);
    
    switch (tokens[op_pos].type) {
      case TK_ADD:
        return left_val + right_val;
      case TK_SUB:
        return left_val - right_val;
      case TK_MUL:
        return left_val * right_val;
      case TK_DIV:
        return left_val / right_val;
      case TK_EQ:
        return left_val == right_val;
      case TK_NEQ:
        return left_val != right_val;
      case TK_OR:
        return left_val || right_val;
      case TK_AND:
        return left_val && right_val;
      default:
        *success = false;
        return 0;
    }
  }
}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  *success = true;

  return eval(0, nr_token - 1, success);
}
