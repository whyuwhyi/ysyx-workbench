module ysyx_25030081_cu(
  input [6:0] funct7,
  input [2:0] funct3,
  input [6:0] opcode,
  output [2:0] ext_op,
  output reg_wr,
  output alu_a_src,
  output [1:0] alu_b_src,
  output [3:0] alu_op,
  output [2:0] branch,
  output mem_to_reg,
  output mem_wen,
  output mem_ren,
  output [2:0] mem_op
);

  wire r_add, r_and, r_or, r_sll, r_slt, r_sltu, r_sra, r_srl, r_sub, r_xor;
  wire i_addi, i_andi, i_jalr, i_lb, i_lbu, i_lh, i_lhu, i_lw, 
       i_ori, i_slli, i_slti, i_sltiu, i_srai, i_srli, i_xori;
  wire s_sb, s_sh, s_sw;
  wire b_beq, b_bge, b_bgeu, b_blt, b_bltu, b_bne;
  wire u_auipc, u_lui, j_jal;
  wire j_jal;

  wire r_type, i_type, s_type, b_type, u_type, j_type;

  // R型
  assign r_add  = `ysyx_25030081_R_TYPE(7'b0000000, 3'b000, 7'b0110011);
  assign r_and  = `ysyx_25030081_R_TYPE(7'b0000000, 3'b111, 7'b0110011);
  assign r_or   = `ysyx_25030081_R_TYPE(7'b0000000, 3'b110, 7'b0110011);
  assign r_sll  = `ysyx_25030081_R_TYPE(7'b0000000, 3'b001, 7'b0110011);
  assign r_slt  = `ysyx_25030081_R_TYPE(7'b0000000, 3'b010, 7'b0110011);
  assign r_sltu = `ysyx_25030081_R_TYPE(7'b0000000, 3'b011, 7'b0110011);
  assign r_sra  = `ysyx_25030081_R_TYPE(7'b0100000, 3'b101, 7'b0110011);
  assign r_srl  = `ysyx_25030081_R_TYPE(7'b0000000, 3'b101, 7'b0110011);
  assign r_sub  = `ysyx_25030081_R_TYPE(7'b0100000, 3'b000, 7'b0110011);
  assign r_xor  = `ysyx_25030081_R_TYPE(7'b0000000, 3'b100, 7'b0110011);
  
  assign r_type = r_add | r_and | r_or | r_sll | r_slt | r_sltu | r_sra | r_srl | r_sub | r_xor;

  // I型
  assign i_addi  = `ysyx_25030081_I_TYPE(3'b000, 7'b0010011);
  assign i_andi  = `ysyx_25030081_I_TYPE(3'b111, 7'b0010011);
  assign i_jalr  = `ysyx_25030081_I_TYPE(3'b000, 7'b1100111);
  assign i_lb    = `ysyx_25030081_I_TYPE(3'b000, 7'b0000011);
  assign i_lbu   = `ysyx_25030081_I_TYPE(3'b100, 7'b0000011);
  assign i_lh    = `ysyx_25030081_I_TYPE(3'b001, 7'b0000011);
  assign i_lhu   = `ysyx_25030081_I_TYPE(3'b101, 7'b0000011);
  assign i_lw    = `ysyx_25030081_I_TYPE(3'b010, 7'b0000011);
  assign i_ori   = `ysyx_25030081_I_TYPE(3'b110, 7'b0010011);
  assign i_slli  = `ysyx_25030081_I_TYPE(3'b001, 7'b0010011);
  assign i_slti  = `ysyx_25030081_I_TYPE(3'b010, 7'b0010011);
  assign i_sltiu = `ysyx_25030081_I_TYPE(3'b011, 7'b0010011);
  assign i_srai  = `ysyx_25030081_I_TYPE(3'b101, 7'b0010011);
  assign i_srli  = `ysyx_25030081_I_TYPE(3'b101, 7'b0010011);
  assign i_xori  = `ysyx_25030081_I_TYPE(3'b100, 7'b0010011);

  assign i_type = i_addi | i_andi | i_jalr | i_lb | i_lbu | i_lh | i_lhu | i_lw |
                  i_ori | i_slli | i_slti | i_sltiu | i_srai | i_srli | i_xori;

  // S型
  assign s_sb = `ysyx_25030081_S_TYPE(3'b000, 7'b0100011);
  assign s_sh = `ysyx_25030081_S_TYPE(3'b001, 7'b0100011);
  assign s_sw = `ysyx_25030081_S_TYPE(3'b010, 7'b0100011);

  assign s_type = s_sb | s_sh | s_sw;

  // B型
  assign b_beq  = `ysyx_25030081_B_TYPE(3'b000, 7'b1100011);
  assign b_bge  = `ysyx_25030081_B_TYPE(3'b101, 7'b1100011);
  assign b_bgeu = `ysyx_25030081_B_TYPE(3'b111, 7'b1100011);
  assign b_blt  = `ysyx_25030081_B_TYPE(3'b100, 7'b1100011);
  assign b_bltu = `ysyx_25030081_B_TYPE(3'b110, 7'b1100011);
  assign b_bne  = `ysyx_25030081_B_TYPE(3'b001, 7'b1100011);

  assign b_type = b_beq | b_bge | b_bgeu | b_blt | b_bltu | b_bne;

  // U型
  assign u_auipc = `ysyx_25030081_U_TYPE(7'b0010111);
  assign u_lui    = `ysyx_25030081_U_TYPE(7'b0110111);

  assign u_type = u_auipc | u_lui;

  // J型
  assign j_jal = `ysyx_25030081_J_TYPE(7'b1101111);

  assign j_type = j_jal;


  assign ext_op = {j_type, b_type | u_type, s_type | u_type};
  assign reg_wr = r_type | i_type | u_type | j_type;
  MuxKey #(3, 2, 3) branch_mux_inst (branch, {j_jal, i_jalr}, {
    2'b10, 3'b001,
    2'b01, 3'b010,
    2'b00, 3'b000
  });
  MuxKey #(2, 1, 1) mem_to_reg_mux_inst (mem_to_reg, j_jal | i_jalr, {
    1'b1, 1'b1,
    1'b0, 1'b0
  });
  assign mem_wen = s_type;
  assign mem_ren = i_type;
  assign mem_op = {s_type, i_type | u_type, j_type};
  MuxKey #(2, 1, 1) alu_a_src_mux_inst (alu_a_src, u_auipc | j_jal | i_jalr, {
    1'b1, 1'b1,
    1'b0, 1'b0
  });
  MuxKey #(3, 2, 2) alu_b_src_mux_inst (alu_b_src, {(j_jal | i_jalr), (i_type | u_type)}, {
    2'b10, 2'b10,
    2'b01, 2'b01,
    2'b00, 2'b00
  });
  MuxKey #(2, 1, 4) alu_op_mux_inst (alu_op, u_lui, {
    1'b1, 4'b0011,
    1'b0, 4'b0000
  });

endmodule
