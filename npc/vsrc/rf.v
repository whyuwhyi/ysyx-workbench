module ysyx_25030081_rf #(RF_ADDR_WIDTH = 5, DATA_WIDTH = 32) (
  input clk,
  input wen,
  input [RF_ADDR_WIDTH-1:0] raddr1,
  input [RF_ADDR_WIDTH-1:0] raddr2,
  input [RF_ADDR_WIDTH-1:0] waddr,
  input [DATA_WIDTH-1:0] wdata,
  output [DATA_WIDTH-1:0] rdata1,
  output [DATA_WIDTH-1:0] rdata2
);
  reg [DATA_WIDTH-1:0] rf [2**RF_ADDR_WIDTH-1:0];
  
  assign rdata1 = (|raddr1) ? rf[raddr1] : 32'b0;
  assign rdata2 = (|raddr2) ? rf[raddr2] : 32'b0;

  always @(posedge clk) begin
    if (wen && |waddr) rf[waddr] <= wdata;
  end
endmodule

