module ysyx_25030081_alu #(DATA_WIDTH=32)(
  input [3:0] op,
  input [DATA_WIDTH-1:0] op1,
  input [DATA_WIDTH-1:0] op2,
  input unsigned_cmp,
  output [DATA_WIDTH-1:0] out,
  output zero,
  output less
);
  
  wire sub_flag;
  wire [DATA_WIDTH-1:0] op2_processed;
  wire [DATA_WIDTH-1:0] add_result;
  wire [DATA_WIDTH-1:0] sll_result;
  wire [DATA_WIDTH-1:0] slt_result;
  wire [DATA_WIDTH-1:0] sltu_result;
  wire [DATA_WIDTH-1:0] xor_result;
  wire [DATA_WIDTH-1:0] srl_result;
  wire [DATA_WIDTH-1:0] sra_result;
  wire [DATA_WIDTH-1:0] or_result;
  wire [DATA_WIDTH-1:0] and_result;
  
  assign op2_processed = (op == 4'b0000) ? op2 : ~op2+1; 
  assign add_result = op1 + op2_processed;
  assign sll_result = op1 << op2[4:0];
  
  // 有符号比较：考虑溢出情况
  wire signed_less = (op1[31] == op2[31]) ? add_result[31] : op1[31];
  assign slt_result = {31'b0, signed_less};
  
  // 无符号比较：直接比较
  assign sltu_result = {31'b0, (op1 < op2)};
  
  assign xor_result = op1 ^ op2;
  assign srl_result = op1 >> op2[4:0];
  assign sra_result = $signed(op1) >>> op2[4:0];
  assign or_result = op1 | op2;
  assign and_result = op1 & op2;
  
  MuxKey #(11, 4, 32) alu_result_mux_inst (out, op, {
    4'b0000, add_result,
    4'b1000, add_result,
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

  assign zero = (out == 32'b0);
  assign less = unsigned_cmp ? sltu_result[0] : slt_result[0];

endmodule

