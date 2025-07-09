module ysyx_25030081_pc #(ADDR_WIDTH=32,PC_RST=32'h8000_0000)(
    input clk,
    input rst,
    input [ADDR_WIDTH-1:0] next_pc,
    output reg [ADDR_WIDTH-1:0] pc
);

  always @(posedge clk) begin
    if (rst) begin
      pc <= PC_RST;
    end else begin
      pc <= next_pc;
    end
  end

endmodule
