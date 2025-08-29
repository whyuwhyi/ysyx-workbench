/* 32-bit only division routines for RISC-V32E (no M extension)
 * Software implementation for 32-bit mul/div operations only
 * 64-bit operations are handled by compiler-rt
 */

#include "riscv-asm.h"

  .text
  .align 2

/* 32-bit unsigned division */
FUNC_BEGIN (__udivsi3)
  mv    a2, a1
  mv    a1, a0
  li    a0, -1
  beqz  a2, .L5
  li    a3, 1
  bgeu  a2, a1, .L2
.L1:
  blez  a2, .L2
  slli  a2, a2, 1
  slli  a3, a3, 1
  bgtu  a1, a2, .L1
.L2:
  li    a0, 0
.L3:
  bltu  a1, a2, .L4
  sub   a1, a1, a2
  or    a0, a0, a3
.L4:
  srli  a3, a3, 1
  srli  a2, a2, 1
  bnez  a3, .L3
.L5:
  ret
FUNC_END (__udivsi3)

/* 32-bit unsigned modulo */
FUNC_BEGIN (__umodsi3)
  move  t0, ra
  call  __udivsi3
  move  a0, a1
  jr    t0
FUNC_END (__umodsi3)

/* 32-bit signed division */
FUNC_BEGIN (__divsi3)
  bltz  a0, .L10
  bltz  a1, .L11
  /* Both positive, fall into __udivsi3 */
  j     __udivsi3

.L10:
  neg   a0, a0
  bgtz  a1, .L12     /* Compute __udivsi3(-a0, a1), then negate */
  neg   a1, a1
  j     __udivsi3    /* Both negative, result positive */
  
.L11:                /* Compute __udivsi3(a0, -a1), then negate */
  neg   a1, a1
.L12:
  move  t0, ra
  call  __udivsi3
  neg   a0, a0
  jr    t0
FUNC_END (__divsi3)

/* 32-bit signed modulo */
FUNC_BEGIN (__modsi3)
  move   t0, ra
  bltz   a1, .L31
  bltz   a0, .L32
.L30:
  call   __udivsi3
  move   a0, a1
  jr     t0
.L31:
  neg    a1, a1
  bgez   a0, .L30
.L32:
  neg    a0, a0
  call   __udivsi3
  neg    a0, a1
  jr     t0
FUNC_END (__modsi3)

/* 32-bit unsigned multiplication */
FUNC_BEGIN (__mulsi3)
  mv    a2, a0
  li    a0, 0
.Lmul_loop:
  andi  a3, a1, 1
  beqz  a3, .Lmul_skip
  add   a0, a0, a2
.Lmul_skip:
  srli  a1, a1, 1
  slli  a2, a2, 1
  bnez  a1, .Lmul_loop
  ret
FUNC_END (__mulsi3)

/* 64-bit unsigned division - software implementation */
FUNC_BEGIN (__udivdi3)
  mv    a2, a1
  mv    a1, a0
  li    a0, -1
  beqz  a2, .L64_5
  li    a3, 1
  bgeu  a2, a1, .L64_2
.L64_1:
  blez  a2, .L64_2
  slli  a2, a2, 1
  slli  a3, a3, 1
  bgtu  a1, a2, .L64_1
.L64_2:
  li    a0, 0
.L64_3:
  bltu  a1, a2, .L64_4
  sub   a1, a1, a2
  or    a0, a0, a3
.L64_4:
  srli  a3, a3, 1
  srli  a2, a2, 1
  bnez  a3, .L64_3
.L64_5:
  ret
FUNC_END (__udivdi3)

/* 64-bit unsigned modulo */
FUNC_BEGIN (__umoddi3)
  move  t0, ra
  call  __udivdi3
  move  a0, a1
  jr    t0
FUNC_END (__umoddi3)

/* 64-bit signed division */
FUNC_BEGIN (__divdi3)
  bltz  a0, .L64_10
  bltz  a1, .L64_11
  j     __udivdi3

.L64_10:
  neg   a0, a0
  bgtz  a1, .L64_12
  neg   a1, a1
  j     __udivdi3
  
.L64_11:
  neg   a1, a1
.L64_12:
  move  t0, ra
  call  __udivdi3
  neg   a0, a0
  jr    t0
FUNC_END (__divdi3)

/* 64-bit signed modulo */
FUNC_BEGIN (__moddi3)
  move   t0, ra
  bltz   a1, .L64_31
  bltz   a0, .L64_32
.L64_30:
  call   __udivdi3
  move   a0, a1
  jr     t0
.L64_31:
  neg    a1, a1
  bgez   a0, .L64_30
.L64_32:
  neg    a0, a0
  call   __udivdi3
  neg    a0, a1
  jr     t0
FUNC_END (__moddi3)
