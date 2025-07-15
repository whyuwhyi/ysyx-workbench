module ysyx_25030081_pc #(ADDR_WIDTH=32,PC_RST=32'h8000_0000)(
    input clk,
    input rst,
    input [ADDR_WIDTH-1:0] next_pc,
    output reg [ADDR_WIDTH-1:0] pc
);

  Reg #(32, 32'h8000_0000) pc_reg (
    .clk(clk),
    .rst(rst),
    .din(next_pc),
    .dout(pc),
    .wen(1'b1)
  );

endmodule
