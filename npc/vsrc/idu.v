module ysyx_25030081_idu #(RF_ADDR_WIDTH=5,DATA_WIDTH=32)
(
  input [DATA_WIDTH-1:0] inst,
  output [6:0] funct7,
  output [RF_ADDR_WIDTH-1:0] rs2,
  output [RF_ADDR_WIDTH-1:0] rs1,
  output [2:0] funct3,
  output [RF_ADDR_WIDTH-1:0] rd,
  output [6:0] opcode
);

  assign funct7 = inst[31:25];
  assign rs2 = inst[24:20];
  assign rs1 = inst[19:15];
  assign funct3 = inst[14:12];
  assign rd = inst[11:7];
  assign opcode = inst[6:0];

endmodule
