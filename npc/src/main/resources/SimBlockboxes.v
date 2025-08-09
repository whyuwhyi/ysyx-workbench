import "DPI-C" function int pmem_read(input int addr);
import "DPI-C" function void pmem_write(input int waddr, input int wdata, input byte wmask);
import "DPI-C" function void ebreak();

module SimMem(
  input        clk,
  input [31:0] raddr,
  output [31:0] rdata,
  input        wen,
  input [31:0] waddr,
  input [31:0] wdata,
  input [3:0]  wmask
);

  assign rdata = pmem_read(raddr);

  always @(posedge clk) begin
    if (wen) begin
      pmem_write(waddr, wdata, wmask);
    end
  end
endmodule

module SimCtrl(
  input clk,
  input rst,
  input ebreak
);
  always @(posedge clk) begin
    if (ebreak && !rst) begin
      ebreak();
    end
  end
endmodule
