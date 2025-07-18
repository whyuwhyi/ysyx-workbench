module ysyx_25030081_alu #(DATA_WIDTH=32)(
  input [3:0] alu_op,
  input [DATA_WIDTH-1:0] op1,
  input [DATA_WIDTH-1:0] op2,
  output [DATA_WIDTH-1:0] alu_out,
  output zero,
  output less
);
  
  wire sub_flag;
  wire [DATA_WIDTH-1:0] op2_processed;
  wire [DATA_WIDTH-1:0] addsub_result;
  wire [DATA_WIDTH-1:0] sll_result;
  wire [DATA_WIDTH-1:0] slt_result;
  wire [DATA_WIDTH-1:0] sltu_result;
  wire [DATA_WIDTH-1:0] xor_result;
  wire [DATA_WIDTH-1:0] srl_result;
  wire [DATA_WIDTH-1:0] sra_result;
  wire [DATA_WIDTH-1:0] or_result;
  wire [DATA_WIDTH-1:0] and_result;
  
  assign sub_flag = alu_op[3];
  MuxKey #(2, 1, 32) op2_processed_mux_inst (op2_processed, sub_flag, {
    1'b1, (~op2 + 1),
    1'b0, op2
  });
  assign addsub_result = op1 + op2_processed;
  assign sll_result = op1 << op2[4:0];
  assign slt_result = {31'b0, addsub_result[31]};
  assign sltu_result = {31'b0, ~(~addsub_result[31] & |addsub_result)};
  assign xor_result = op1 ^ op2;
  assign srl_result = op1 >> op2[4:0];
  assign sra_result = $signed(op1) >>> op2[4:0];
  assign or_result = op1 | op2;
  assign and_result = op1 & op2;
  
  MuxKey #(11, 4, 32) alu_result_mux_inst (alu_out, alu_op, {
    4'b0000, addsub_result,
    4'b1000, addsub_result,
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

