module ysyx_25020081_ext #(DATA_WIDTH=32) (
  input [31:7] inst,
  input [2:0]ext_op,
  output [DATA_WIDTH-1:0] imm
);

  wire [DATA_WIDTH-1:0] immI;
  wire [DATA_WIDTH-1:0] immU;
  wire [DATA_WIDTH-1:0] immS;
  wire [DATA_WIDTH-1:0] immB;
  wire [DATA_WIDTH-1:0] immJ;

  assign immI = {{20{inst[31]}}, inst[31:20]};
  assign immU = {inst[31:12], 12'b0};
  assign immS = {{20{inst[31]}}, inst[31:25], inst[11:7]};
  assign immB = {{20{inst[31]}}, inst[7], inst[30:25], inst[11:8], 1'b0};
  assign immJ = {{12{inst[31]}}, inst[19:12], inst[20], inst[30:21], 1'b0};

  MuxKey #(5, 3, 32) mux_inst (imm, ext_op, {
    3'b000, immI,
    3'b001, immU,
    3'b010, immS,
    3'b011, immB,
    3'b100, immJ
  });
 

endmodule
