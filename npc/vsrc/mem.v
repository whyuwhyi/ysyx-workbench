module ysyx_25030081_mem #(ADDR_WIDTH=32, DATA_WIDTH=32)
(
  input ren,
  input [ADDR_WIDTH-1:0] raddr,
  input wen,
  input [ADDR_WIDTH-1:0] waddr,
  input [DATA_WIDTH-1:0] wdata,
  input [2:0] mem_op,
  output reg [DATA_WIDTH-1:0] rdata
);

  wire [7:0] wmask;

  MuxKey #(4, 2, 8) wmask_mux_inst (wmask, mem_op[1:0], {
    2'b00, 8'b00000001,
    2'b01, 8'b00000011,
    2'b10, 8'b00001111,
    2'b11, 8'b00000000
  });

  always @(*) begin
    if (ren)
      rdata = pmem_read(raddr);
    else
      rdata = 32'b0;
  end

  always @(*) begin
    if (wen)
      pmem_write(waddr, wdata, wmask);
  end

endmodule
