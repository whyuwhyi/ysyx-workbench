module ysyx_25030081_cu(
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

  wire opcode_check;

  wire addi;
  wire auipc, lui;
  wire jal, jalr;

  wire r_type, i_type, s_type, b_type, u_type, j_type;

  assign opcode_check = opcode[1] & opcode[0];

  // R型

  // I型
  assign addi = ~funct3[2] & ~funct3[1] & ~funct3[0] &
                ~opcode[6] & ~opcode[5] &  opcode[4] &
                ~opcode[3] & ~opcode[2] &  opcode_check;
  assign jalr = ~funct3[2] & ~funct3[1] & ~funct3[0] &
                 opcode[6] &  opcode[5] & ~opcode[4] &
                ~opcode[3] &  opcode[2] &  opcode_check;

  // S型

  // B型

  // U型
  assign auipc = ~opcode[6] & ~opcode[5] &  opcode[4] &
                  opcode[3] & ~opcode[2] &  opcode_check;

  assign lui = ~opcode[6] & opcode[5] &  opcode[4] &
               ~opcode[3] & opcode[2] &  opcode_check;

  // J型
  assign jal = opcode[6] & opcode[5] & ~opcode[4] &
               opcode[3] & opcode[2] &  opcode_check;

  assign r_type = 1'b0;
  assign i_type = addi | jalr;
  // assign s_type;
  // assign b_type;
  assign u_type = auipc | lui;
  assign j_type = jal;

  assign ext_op = (i_type) ? 3'b000 :
                  (u_type) ? 3'b001 :
                  (j_type) ? 3'b100 :
                  3'b000;
  assign reg_wr = r_type | i_type | u_type | j_type;
  assign branch = (jal) ? 3'b001 :
                  (jalr) ? 3'b010 :
                  3'b000;
  assign mem_to_reg = (jal | jalr) ? 1'b1 : 1'b0;
  assign mem_wr = s_type;
  assign mem_op = {s_type, i_type | u_type, j_type};
  assign alu_a_src = (auipc | jal | jalr) ? 1'b1 : 1'b0;
  assign alu_b_src = (jal | jalr) ? 2'b10 :
                     (i_type | u_type) ? 2'b01 : 2'b00;
  assign alu_op = (addi) ? 4'b0000 :
                  (auipc) ? 4'b0000 :
                  (lui) ? 4'b0011 :
                  (jal) ? 4'b0000 :
                  (jalr) ? 4'b0000 :
                  4'b0000;

endmodule
