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
wire [DATA_WIDTH-1:0] rs1_plus_imm;

assign pc_plus_4 = pc + 4;
assign pc_plus_imm = pc + imm;
assign rs1_plus_imm = rdata1 + imm;

wire pc_sel;

MuxKey #(7, 3, 1) branch_mux (pc_sel, branch, {
  3'b000, 1'b0,
  3'b001, 1'b1,
  3'b010, 1'b1,
  3'b100, zero,
  3'b101, ~zero,
  3'b110, less,
  3'b111, ~less
});

wire pc_src;
assign pc_src = (branch == 3'b010) ? 1'b1 : 1'b0;

wire [DATA_WIDTH-1:0] pc_target;
MuxKey #(2, 1, 32) pc_target_mux (pc_target, pc_src, {
  1'b0, pc_plus_imm,
  1'b1, rs1_plus_imm
});

assign next_pc = pc_sel ? pc_target : pc_plus_4;

endmodule
