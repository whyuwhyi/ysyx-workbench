module ysyx_25030081_npc #(DATA_WIDTH=32)(
  input [DATA_WIDTH-1:0] pc,
  input [DATA_WIDTH-1:0] rdata1,
  input [DATA_WIDTH-1:0] imm,
  input [2:0] branch,
  input zero,
  input less,
  output [DATA_WIDTH-1:0] next_pc
);

  wire [DATA_WIDTH-1:0] pc_plus_4;
  wire [DATA_WIDTH-1:0] pc_plus_imm;
  wire [DATA_WIDTH-1:0] rdata1_plus_imm;
  wire [DATA_WIDTH-1:0] pc_branch;
  wire flag;

  assign pc_plus_4 = pc + 4;
  assign pc_plus_imm = pc + imm;
  assign rdata1_plus_imm = rdata1 + imm;

  assign flag = branch[1] ? less : zero;
  assign pc_branch = branch[0] ^ flag ? pc_plus_imm : pc_plus_4;
  
  MuxKey #(7, 3, 32) next_pc_mux_inst (next_pc, branch, {
    3'b000, pc_plus_4,
    3'b001, pc_plus_imm,
    3'b010, rdata1_plus_imm,
    3'b100, pc_branch,
    3'b101, pc_branch,
    3'b110, pc_branch,
    3'b111, pc_branch
  });

 endmodule
