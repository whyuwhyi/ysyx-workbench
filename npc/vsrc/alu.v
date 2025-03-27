module ysyx_25020081_alu #(DATA_WIDTH=32)(
  input [3:0] alu_op,
  input [DATA_WIDTH-1:0] op1,
  input [DATA_WIDTH-1:0] op2,
  output [DATA_WIDTH-1:0] alu_out
);
  
  MuxKey #(1 , 4, 32) mux_inst (alu_out, alu_op, {
    4'b0000, op1 + op2
  });

endmodule

