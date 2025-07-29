`define ysyx_25030081_R_TYPE(f7, f3, op) \
  (funct7 == f7 & funct3 == f3 & opcode == op)

`define ysyx_25030081_I_TYPE(f3, op) \
  (funct3 == f3 & opcode == op)

`define ysyx_25030081_I_TYPE_WITH_F7(f7, f3, op) \
  (funct7 == f7 & funct3 == f3 & opcode == op)

`define ysyx_25030081_S_TYPE(f3, op) \
  (funct3 == f3 & opcode == op)

`define ysyx_25030081_B_TYPE(f3, op) \
  (funct3 == f3 & opcode == op)

`define ysyx_25030081_U_TYPE(op) \
  (opcode == op)

`define ysyx_25030081_J_TYPE(op) \
  (opcode == op)
