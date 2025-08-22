`ifndef PMEM_READ_DEFINED
`define PMEM_READ_DEFINED
import "DPI-C" function int pmem_read(input int addr);
`endif
import "DPI-C" function void pmem_write(input int waddr, input int wdata, input byte wmask);

module SimDMem #(
  parameter XLEN = 32
)(

  input  logic [XLEN-1:0] addr,
  input  logic            wen,
  input  logic [7:0] wmask,
  input  logic [XLEN-1:0] wdata,
  input  logic            ren,
  output logic [XLEN-1:0] rdata
);
  always @(*) begin
    if (ren) rdata = pmem_read(addr);
    else     rdata = '0;
  end

  always @(*) begin
    if (wen) pmem_write(addr, wdata, wmask);
  end
endmodule
