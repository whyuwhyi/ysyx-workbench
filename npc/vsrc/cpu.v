import "DPI-C" function void ebreak();

module ysyx_25030081_cpu #(RF_ADDR_WIDTH=5,ADDR_WIDTH=32,DATA_WIDTH=32)(
  input clk,
  input rst,
  input [DATA_WIDTH-1:0] inst,
  output [ADDR_WIDTH-1:0] pc,
  output [DATA_WIDTH-1:0] alu_out
);

wire [RF_ADDR_WIDTH-1:0] rs1;
wire [RF_ADDR_WIDTH-1:0] rs2;
wire [RF_ADDR_WIDTH-1:0] rd;
wire [DATA_WIDTH-1:0] wdata;
wire [DATA_WIDTH-1:0] rdata1;
wire [DATA_WIDTH-1:0] rdata2;
wire [DATA_WIDTH-1:0] op1;
wire [DATA_WIDTH-1:0] op2;
// wire [DATA_WIDTH-1:0] alu_out;
wire [DATA_WIDTH-1:0] imm;
wire zero;
wire less;

wire [6:0] funct7;
wire [2:0] funct3;
wire [6:0] opcode;

wire reg_wr, mem_to_reg, mem_wr, alu_a_src;
wire [1:0] alu_b_src;
wire [3:0] alu_op;
wire [2:0] ext_op;
wire [2:0] branch;
wire [2:0] mem_op;

assign zero = (alu_out == 0);
assign less = alu_out[31];

// PC选择逻辑
wire [DATA_WIDTH-1:0] pc_plus_4;
wire [DATA_WIDTH-1:0] pc_plus_imm;
wire [DATA_WIDTH-1:0] rs1_plus_imm;
wire [DATA_WIDTH-1:0] next_pc;

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

ysyx_25030081_pc pc_inst(
  .clk(clk),
  .rst(rst),
  .next_pc(next_pc),
  .pc(pc)
);

ysyx_25030081_idu idu_inst(
  .inst(inst),
  .funct7(funct7),
  .rs2(rs2),
  .rs1(rs1),
  .funct3(funct3),
  .rd(rd),
  .opcode(opcode)
);

ysyx_25030081_cu cu_inst(
  .funct7(funct7),
  .funct3(funct3),
  .opcode(opcode),
  .ext_op(ext_op),
  .reg_wr(reg_wr),
  .branch(branch),
  .mem_to_reg(mem_to_reg),
  .mem_wr(mem_wr),
  .mem_op(mem_op),
  .alu_a_src(alu_a_src),
  .alu_b_src(alu_b_src),
  .alu_op(alu_op)
);

ysyx_25030081_ext ext_inst(
  .inst(inst[31:7]),
  .ext_op(ext_op),
  .imm(imm)
);

ysyx_25030081_rf regfile_inst(
  .clk(clk),
  .wen(reg_wr),
  .raddr1(rs1),
  .raddr2(rs2),
  .waddr(rd),
  .wdata(wdata),
  .rdata1(rdata1),
  .rdata2(rdata2)
);

MuxKey #(2 , 1, 32) alu_a_mux_inst (op1, alu_a_src, {
  1'b0, rdata1,
  1'b1, pc
});

MuxKey #(2 , 2, 32) alu_b_mux_inst (op2, alu_b_src, {
  2'b00, rdata2,
  2'b01, imm
});

ysyx_25030081_alu alu_inst(
  .alu_op(alu_op),
  .op1(op1),
  .op2(op2),
  .alu_out(alu_out)
);

MuxKey #(2, 1, 32) rf_wdata_mux_inst (wdata, mem_to_reg, {
  1'b0, alu_out,
  1'b1, pc_plus_4
});

always @(inst)
  if (inst == 32'h00100073)
    ebreak();

endmodule
