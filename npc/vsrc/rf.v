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

  MuxKey #(2, 1, 32) rdata1_mux_inst(rdata1, (raddr1 == 0), {
    1'b1, 32'b0,
    1'b0, rf[raddr1]
  });

  MuxKey #(2, 1, 32) rdata2_mux_inst(rdata2, (raddr2 == 0), {
    1'b1, 32'b0,
    1'b0, rf[raddr2]
  });

  always @(posedge clk) begin
    if (wen && |waddr) rf[waddr] <= wdata;
  end

  // DPI-C function to get register value
  export "DPI-C" function get_reg_value;
  function int get_reg_value(input int reg_idx);
    if (reg_idx == 0) 
      get_reg_value = 0;  // x0 is always 0
    else if (reg_idx >= 1 && reg_idx <= 31)
      get_reg_value = rf[reg_idx];
    else
      get_reg_value = 0;  // Invalid register index
  endfunction

endmodule

