module ysyx_25030081_alu #(DATA_WIDTH=32)(
  input [3:0] alu_op,
  input [DATA_WIDTH-1:0] op1,
  input [DATA_WIDTH-1:0] op2,
  output [DATA_WIDTH-1:0] alu_out
);
  
  wire [DATA_WIDTH-1:0] add_result;
  wire [DATA_WIDTH-1:0] sub_result;
  wire [DATA_WIDTH-1:0] sll_result;
  wire [DATA_WIDTH-1:0] slt_result;
  wire [DATA_WIDTH-1:0] sltu_result;
  wire [DATA_WIDTH-1:0] xor_result;
  wire [DATA_WIDTH-1:0] srl_result;
  wire [DATA_WIDTH-1:0] sra_result;
  wire [DATA_WIDTH-1:0] or_result;
  wire [DATA_WIDTH-1:0] and_result;
  
  assign add_result = op1 + op2;
  assign sub_result = op1 - op2;
  assign sll_result = op1 << op2[4:0];
  assign slt_result = ($signed(op1) < $signed(op2)) ? 32'h1 : 32'h0;
  assign sltu_result = (op1 < op2) ? 32'h1 : 32'h0;
  assign xor_result = op1 ^ op2;
  assign srl_result = op1 >> op2[4:0];
  assign sra_result = $signed(op1) >>> op2[4:0];
  assign or_result = op1 | op2;
  assign and_result = op1 & op2;
  
  MuxKey #(11, 4, 32) alu_result_mux_inst (alu_out, alu_op, {
    4'b0000, add_result,
    4'b1000, sub_result,
    4'b0001, sll_result,
    4'b0010, slt_result,
    4'b1010, sltu_result,
    4'b0011, op2,
    4'b0100, xor_result,
    4'b0101, srl_result,
    4'b1101, sra_result,
    4'b0110, or_result,
    4'b0111, and_result
  });

endmodule

