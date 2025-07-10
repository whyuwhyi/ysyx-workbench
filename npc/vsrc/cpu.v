import "DPI-C" function void ebreak();

module ysyx_25030081_cpu #(RF_ADDR_WIDTH=5,ADDR_WIDTH=32,DATA_WIDTH=32)(
  input clk,
  input rst,
  input [DATA_WIDTH-1:0] inst,
  output [ADDR_WIDTH-1:0] pc,
  output [DATA_WIDTH-1:0] alu_out,
  output [DATA_WIDTH-1:0] a0
);

wire [RF_ADDR_WIDTH-1:0] rs1;
wire [RF_ADDR_WIDTH-1:0] rs2;
wire [RF_ADDR_WIDTH-1:0] rd;
wire [DATA_WIDTH-1:0] wdata;
wire [DATA_WIDTH-1:0] rdata1;
wire [DATA_WIDTH-1:0] rdata2;
wire [DATA_WIDTH-1:0] op1;
wire [DATA_WIDTH-1:0] op2;
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

wire [DATA_WIDTH-1:0] next_pc;
wire [DATA_WIDTH-1:0] pc_plus_4;

assign pc_plus_4 = pc + 4;

ysyx_25030081_npc npc_inst(
  .pc(pc),
  .rdata1(rdata1),
  .imm(imm),
  .branch(branch),
  .zero(zero),
  .less(less),
  .next_pc(next_pc)
);

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
  .rdata2(rdata2),
  .a0(a0)
);

MuxKey #(2 , 1, 32) alu_a_mux_inst (op1, alu_a_src, {
  1'b0, rdata1,
  1'b1, pc
});

MuxKey #(3 , 2, 32) alu_b_mux_inst (op2, alu_b_src, {
  2'b00, rdata2,
  2'b01, imm,
  2'b10, 32'd4
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
