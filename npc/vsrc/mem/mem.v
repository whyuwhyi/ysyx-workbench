module ysyx_25030081_mem #(ADDR_WIDTH=32, DATA_WIDTH=32)
(
  input ren,
  input [ADDR_WIDTH-1:0] raddr,
  input wen,
  input [ADDR_WIDTH-1:0] waddr,
  input [DATA_WIDTH-1:0] wdata,
  input [2:0] op,
  output reg [DATA_WIDTH-1:0] rdata
);

  wire [7:0] wmask;

  MuxKey #(4, 2, 8) wmask_mux_inst (wmask, op[1:0], {
    2'b00, 8'b00000001,
    2'b01, 8'b00000011,
    2'b10, 8'b00001111,
    2'b11, 8'b00000000
  });

  reg [DATA_WIDTH-1:0] raw_rdata;
  // assign raw_rdata = ren ? pmem_read(raddr) : 32'b0;
  always @(*) begin
    if (ren)
      raw_rdata = pmem_read(raddr);
    else
      raw_rdata = 32'b0;
  end
  
  MuxKey #(5, 3, 32) rdata_mux_inst (rdata, op, {
    3'b000, {{24{raw_rdata[7]}}, raw_rdata[7:0]},
    3'b001, {{16{raw_rdata[15]}}, raw_rdata[15:0]},
    3'b010, raw_rdata,
    3'b100, {24'b0, raw_rdata[7:0]},
    3'b101, {16'b0, raw_rdata[15:0]}
  });

  always @(*) begin
    if (wen)
      pmem_write(waddr, wdata, wmask);
  end

endmodule
