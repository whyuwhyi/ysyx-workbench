module ysyx_25030081_ifu #(ADDR_WIDTH=32, INST_WIDTH=32)
(
  input [ADDR_WIDTH-1:0] inst_addr,
  output reg [INST_WIDTH-1:0] inst
);

  assign inst = pmem_read(inst_addr);

endmodule
