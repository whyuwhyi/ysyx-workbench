`include "marco.v"

module ysyx_25030081_cu(
  input [31:0] inst,
  output [2:0] ext_op,
  output reg_wen,
  output [1:0] reg_wsel,
  output csr_wen,
  output alu_a_src,
  output [1:0] alu_b_src,
  output [3:0] alu_op,
  output alu_unsigned_cmp,
  output [2:0] branch,
  output mem_wen,
  output mem_ren,
  output [2:0] mem_op
);
  wire [6:0] funct7;
  wire [2:0] funct3;
  wire [6:0] opcode;

  assign funct7 = inst[31:25];
  assign funct3 = inst[14:12];
  assign opcode = inst[6:0];

  wire r_add, r_and, r_or, r_sll, r_slt, r_sltu, r_sra, r_srl, r_sub, r_xor;
  wire r_mret;
  wire i_addi, i_andi, i_jalr, i_lb, i_lbu, i_lh, i_lhu, i_lw, i_ori, i_slli,
       i_slti, i_sltiu, i_srai, i_srli, i_xori;
  wire i_csrrc, i_csrrci, i_csrrs, i_csrrsi, i_csrrw, i_csrrwi, i_ecall, i_ebreak;
  wire s_sb, s_sh, s_sw;
  wire b_beq, b_bge, b_bgeu, b_blt, b_bltu, b_bne;
  wire u_auipc, u_lui, j_jal;

  wire r_type, i_type, s_type, b_type, u_type, j_type;

  // R
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

  assign r_mret = inst == 32'h30200073;
  
  assign r_type = r_add | r_and | r_or | r_sll | r_slt | r_sltu | r_sra | r_srl | r_sub | r_xor;

  // I
  assign i_addi   = `ysyx_25030081_I_TYPE(3'b000, 7'b0010011);
  assign i_andi   = `ysyx_25030081_I_TYPE(3'b111, 7'b0010011);
  assign i_jalr   = `ysyx_25030081_I_TYPE(3'b000, 7'b1100111);
  assign i_lb     = `ysyx_25030081_I_TYPE(3'b000, 7'b0000011);
  assign i_lbu    = `ysyx_25030081_I_TYPE(3'b100, 7'b0000011);
  assign i_lh     = `ysyx_25030081_I_TYPE(3'b001, 7'b0000011);
  assign i_lhu    = `ysyx_25030081_I_TYPE(3'b101, 7'b0000011);
  assign i_lw     = `ysyx_25030081_I_TYPE(3'b010, 7'b0000011);
  assign i_ori    = `ysyx_25030081_I_TYPE(3'b110, 7'b0010011);
  assign i_slli   = `ysyx_25030081_I_TYPE(3'b001, 7'b0010011) & (funct7[6:1] == 6'b000000);
  assign i_slti   = `ysyx_25030081_I_TYPE(3'b010, 7'b0010011);
  assign i_sltiu  = `ysyx_25030081_I_TYPE(3'b011, 7'b0010011);
  assign i_srai   = `ysyx_25030081_I_TYPE(3'b101, 7'b0010011) & (funct7[6:1] == 6'b010000);
  assign i_srli   = `ysyx_25030081_I_TYPE(3'b101, 7'b0010011) & (funct7[6:1] == 6'b000000);
  assign i_xori   = `ysyx_25030081_I_TYPE(3'b100, 7'b0010011);

  assign i_csrrc  = `ysyx_25030081_I_TYPE(3'b011, 7'b1110011);
  assign i_csrrci = `ysyx_25030081_I_TYPE(3'b111, 7'b1110011);
  assign i_csrrs  = `ysyx_25030081_I_TYPE(3'b010, 7'b1110011);
  assign i_csrrsi = `ysyx_25030081_I_TYPE(3'b110, 7'b1110011);
  assign i_csrrw  = `ysyx_25030081_I_TYPE(3'b001, 7'b1110011);
  assign i_csrrwi = `ysyx_25030081_I_TYPE(3'b101, 7'b1110011);
  assign i_ecall  = inst == 32'h00000073;
  assign i_ebreak = inst == 32'h00100073;

  assign i_type = i_addi | i_andi | i_jalr | i_lb | i_lbu | i_lh | i_lhu | i_lw |
                  i_ori | i_slli | i_slti | i_sltiu | i_srai | i_srli | i_xori |
                  i_csrrc | i_csrrci | i_csrrs | i_csrrsi | i_csrrw | i_csrrwi;

  // S
  assign s_sb = `ysyx_25030081_S_TYPE(3'b000, 7'b0100011);
  assign s_sh = `ysyx_25030081_S_TYPE(3'b001, 7'b0100011);
  assign s_sw = `ysyx_25030081_S_TYPE(3'b010, 7'b0100011);

  assign s_type = s_sb | s_sh | s_sw;

  // B
  assign b_beq  = `ysyx_25030081_B_TYPE(3'b000, 7'b1100011);
  assign b_bge  = `ysyx_25030081_B_TYPE(3'b101, 7'b1100011);
  assign b_bgeu = `ysyx_25030081_B_TYPE(3'b111, 7'b1100011);
  assign b_blt  = `ysyx_25030081_B_TYPE(3'b100, 7'b1100011);
  assign b_bltu = `ysyx_25030081_B_TYPE(3'b110, 7'b1100011);
  assign b_bne  = `ysyx_25030081_B_TYPE(3'b001, 7'b1100011);

  assign b_type = b_beq | b_bge | b_bgeu | b_blt | b_bltu | b_bne;

  // U
  assign u_auipc = `ysyx_25030081_U_TYPE(7'b0010111);
  assign u_lui    = `ysyx_25030081_U_TYPE(7'b0110111);

  assign u_type = u_auipc | u_lui;

  // J
  assign j_jal = `ysyx_25030081_J_TYPE(7'b1101111);

  assign j_type = j_jal;

  assign ext_op[2] = j_type;
  assign ext_op[1] = b_type | u_type;
  assign ext_op[0] = s_type | u_type;

  assign reg_wen = r_type | i_type | u_type | j_type;
  assign reg_wsel[0] = i_lw | i_lh | i_lhu | i_lb | i_lbu;
  assign reg_wsel[1] = i_csrrc | i_csrrci | i_csrrs | i_csrrsi | i_csrrw | i_csrrwi;

  assign csr_wen = i_csrrc | i_csrrs | i_csrrw | i_csrrci | i_csrrsi | i_csrrwi;


  assign branch[2] = b_type;
  assign branch[1] = i_jalr | b_blt | b_bltu | b_bge | b_bgeu;
  assign branch[0] = b_bne  | b_bge | b_bgeu | j_jal;

  assign alu_a_src = u_auipc | j_jal | i_jalr;

  assign alu_b_src[1] = i_jalr | j_jal;
  assign alu_b_src[0] = i_type | s_type | u_type;
  
  assign alu_op[3] = r_sub  | r_sra   | r_sltu | i_sltiu | i_srai | b_type;
  assign alu_op[2] = r_and  | r_or    | r_xor  | r_srl   | r_sra  | i_andi |
                     i_ori  | i_xori  | i_srli | i_srai;
  assign alu_op[1] = r_and  | r_or    | r_slt  | r_sltu  | i_andi | i_ori  |
                     i_slti | i_sltiu | u_lui;
  assign alu_op[0] = r_and  | r_sll   | r_srl  | r_sra   | i_andi | i_slli |
                    i_srli  | i_srai  | u_lui;

  assign alu_unsigned_cmp = r_sltu | i_sltiu | b_bltu | b_bgeu;

  assign mem_ren = i_lw | i_lh | i_lhu | i_lb | i_lbu;
  assign mem_wen = s_type;

  assign mem_op[2] = i_lbu | i_lhu;
  assign mem_op[1] = i_lw | s_sw;
  assign mem_op[0] = i_lh | i_lhu | s_sh;

  always @(*)
    if (i_ebreak)
      ebreak();

 endmodule
