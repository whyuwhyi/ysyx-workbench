module ysyx_25030081_csr #(DATA_WIDTH=32, ADDR_WIDTH=12)(
  input clk,
  input wen,
  input [ADDR_WIDTH-1:0] addr,
  input [DATA_WIDTH-1:0] wdata,
  output [DATA_WIDTH-1:0] rdata
);

  reg [DATA_WIDTH-1:0] csr_regs [0:4095];

  assign rdata = csr_regs[addr];

  always @(posedge clk) begin
    if (wen) begin
      csr_regs[addr] <= wdata;
    end
  end

endmodule
