module ysyx_25020081_cu(
  input [6:0] funct7,
  input [2:0] funct3,
  input [6:0] opcode,
  output [2:0] ext_op,
  output reg_wr,
  output [2:0] branch,
  output mem_to_reg,
  output mem_wr,
  output [2:0] mem_op,
  output alu_a_src,
  output [1:0] alu_b_src,
  output [3:0] alu_op
);

  wire opcode_check, addi;

  assign opcode_check = opcode[1] & opcode[0];
  assign addi = ~funct3[2] & ~funct3[1] & ~funct3[0] & opcode_check &
                ~opcode[6] & ~opcode[5] & opcode[4] & ~opcode[3] & ~opcode[2];

  assign reg_wr = addi;
  assign alu_b_src[0] = addi;








endmodule
