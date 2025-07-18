module ysyx_25030081_cpu #(RF_ADDR_WIDTH=5,ADDR_WIDTH=32,DATA_WIDTH=32)(
  input clk,
  input rst
);

wire [ADDR_WIDTH-1:0] pc;
wire [DATA_WIDTH-1:0] inst;

wire [DATA_WIDTH-1:0] alu_out;

wire [RF_ADDR_WIDTH-1:0] rs1;
wire [RF_ADDR_WIDTH-1:0] rs2;
wire [RF_ADDR_WIDTH-1:0] rd;
wire [DATA_WIDTH-1:0] rf_wdata;
wire [DATA_WIDTH-1:0] rf_rdata1;
wire [DATA_WIDTH-1:0] rf_rdata2;
wire [DATA_WIDTH-1:0] alu_op1;
wire [DATA_WIDTH-1:0] alu_op2;
wire [DATA_WIDTH-1:0] imm;
wire zero;
wire less;

wire [6:0] funct7;
wire [2:0] funct3;
wire [6:0] opcode;

wire reg_wr, mem_to_reg, mem_wen, mem_ren, alu_a_src;
wire [1:0] alu_b_src;
wire [3:0] alu_op;
wire [2:0] ext_op;
wire [2:0] branch;
wire [2:0] mem_op;

assign zero = (alu_out == 0);
assign less = alu_out[31];

wire [DATA_WIDTH-1:0] next_pc;
wire [DATA_WIDTH-1:0] pc_plus_4;

wire [DATA_WIDTH-1:0] branch_target;
wire [DATA_WIDTH-1:0] mem_rdata;

MuxKey #(2, 1, 32) branch_target_mux_inst (branch_target, branch[1], {
  1'b0, alu_out,
  1'b1, rf_rdata1 + imm
});

ysyx_25030081_npc npc_inst(
  .pc(pc),
  .branch_jmp_pc(branch_target),
  .branch(branch),
  .zero(zero),
  .less(less),
  .next_pc(next_pc),
  .pc_plus_4(pc_plus_4)
);

ysyx_25030081_pc pc_inst(
  .clk(clk),
  .rst(rst),
  .next_pc(next_pc),
  .pc(pc)
);

ysyx_25030081_ifu ifu_inst(
  .inst_addr(pc),
  .inst(inst)
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
  .alu_a_src(alu_a_src),
  .alu_b_src(alu_b_src),
  .alu_op(alu_op),
  .branch(branch),
  .mem_to_reg(mem_to_reg),
  .mem_wen(mem_wen),
  .mem_ren(mem_ren),
  .mem_op(mem_op)
);

ysyx_25030081_ext ext_inst(
  .inst(inst[31:7]),
  .ext_op(ext_op),
  .imm(imm)
);

ysyx_25030081_rf rf_inst(
  .clk(clk),
  .wen(reg_wr),
  .raddr1(rs1),
  .raddr2(rs2),
  .waddr(rd),
  .wdata(rf_wdata),
  .rdata1(rf_rdata1),
  .rdata2(rf_rdata2)
);

MuxKey #(2 , 1, 32) alu_a_mux_inst (alu_op1, alu_a_src, {
  1'b0, rf_rdata1,
  1'b1, pc
});

MuxKey #(3 , 2, 32) alu_b_mux_inst (alu_op2, alu_b_src, {
  2'b00, rf_rdata2,
  2'b01, imm,
  2'b10, 32'd4
});

ysyx_25030081_alu alu_inst(
  .alu_op(alu_op),
  .op1(alu_op1),
  .op2(alu_op2),
  .alu_out(alu_out),
  .zero(zero),
  .less(less)
);

ysyx_25030081_mem mem_inst(
  .ren(mem_ren),
  .raddr(alu_out),
  .wen(mem_wen),
  .waddr(alu_out),
  .wdata(rf_rdata2),
  .mem_op(mem_op),
  .rdata(mem_rdata)
);

MuxKey #(4, 2, 32) rf_wdata_mux_inst (rf_wdata, {mem_to_reg, mem_ren}, {
  2'b00, alu_out,
  2'b01, mem_rdata,
  2'b10, pc_plus_4,
  2'b11, pc_plus_4
});

always @(inst)
  if (inst == 32'h00100073)
    ebreak();

endmodule
