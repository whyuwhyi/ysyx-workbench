module ysyx_25030081_cpu #(RF_ADDR_WIDTH=5,ADDR_WIDTH=32,DATA_WIDTH=32)(
  input clk,
  input rst
);

wire [ADDR_WIDTH-1:0] pc;
wire [DATA_WIDTH-1:0] next_pc;
wire [DATA_WIDTH-1:0] inst;

wire [RF_ADDR_WIDTH-1:0] rs1;
wire [RF_ADDR_WIDTH-1:0] rs2;
wire [RF_ADDR_WIDTH-1:0] rd;
wire [DATA_WIDTH-1:0] rf_wdata;
wire [DATA_WIDTH-1:0] rf_rdata1;
wire [DATA_WIDTH-1:0] rf_rdata2;
wire [DATA_WIDTH-1:0] alu_op1;
wire [DATA_WIDTH-1:0] alu_op2;
wire [DATA_WIDTH-1:0] imm;

wire reg_wen, csr_ren, csr_wen, mem_wen, mem_ren, alu_a_src, alu_unsigned_cmp;
wire [1:0] reg_wsel, alu_b_src;
wire [3:0] alu_op;
wire [2:0] ext_op;
wire [2:0] branch;
wire [2:0] mem_op;

wire [DATA_WIDTH-1:0] alu_out;
wire zero;
wire less;

wire [DATA_WIDTH-1:0] mem_rdata;

assign alu_op1 = alu_a_src ? pc : rf_rdata1;
assign alu_op2 = alu_b_src[1] ? 32'd4 :
                                (alu_b_src[0] ? imm : rf_rdata2);

assign rf_wdata = mem_to_reg ? mem_rdata : alu_out;

ysyx_25030081_npc npc_inst(
  .pc(pc),
  .rdata1(rf_rdata1),
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
  .inst(inst),
  .ext_op(ext_op),
  .reg_wen(reg_wen),
  .alu_a_src(alu_a_src),
  .alu_b_src(alu_b_src),
  .alu_op(alu_op),
  .alu_unsigned_cmp(alu_unsigned_cmp),
  .branch(branch),
  .mem_to_reg(mem_to_reg),
  .mem_wen(mem_wen),
  .mem_ren(mem_ren),
  .mem_op(mem_op)
);

ysyx_25030081_ext ext_inst(
  .inst(inst[31:7]),
  .op(ext_op),
  .imm(imm)
);

ysyx_25030081_rf rf_inst(
  .clk(clk),
  .wen(reg_wen),
  .raddr1(rs1),
  .raddr2(rs2),
  .waddr(rd),
  .wdata(rf_wdata),
  .rdata1(rf_rdata1),
  .rdata2(rf_rdata2)
);


ysyx_25030081_alu alu_inst(
  .op(alu_op),
  .op1(alu_op1),
  .op2(alu_op2),
  .unsigned_cmp(alu_unsigned_cmp),
  .out(alu_out),
  .zero(zero),
  .less(less)
);

ysyx_25030081_mem mem_inst(
  .ren(mem_ren),
  .raddr(alu_out),
  .wen(mem_wen),
  .waddr(alu_out),
  .wdata(rf_rdata2),
  .op(mem_op),
  .rdata(mem_rdata)
);

ysyx_25030081_csr csr_inst(
  .clk(clk),
  .wen(csr_wen),
  .addr(inst[31:20]),
  .wdata(rf_rdata2),
  .rdata(rf_wdata)
);

endmodule
