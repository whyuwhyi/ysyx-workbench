module ysyx_25030081_npc #(DATA_WIDTH=32)(
  input [DATA_WIDTH-1:0] pc,
  input [DATA_WIDTH-1:0] branch_jmp_pc,
  input [2:0] branch,
  input zero,
  input less,
  output [DATA_WIDTH-1:0] next_pc,
  output [DATA_WIDTH-1:0] pc_plus_4
);

  wire pc_sel;

  MuxKey #(3, 3, 1) pc_sel_mux_inst (pc_sel, branch, {
    3'b000, 1'b0,
    3'b001, 1'b1,
    3'b010, 1'b1
  });

  assign pc_plus_4 = pc + 4;
  MuxKey #(2, 1, 32) next_pc_mux_inst(next_pc, pc_sel, {
    1'b0, pc_plus_4,
    1'b1, branch_jmp_pc
  });


endmodule
