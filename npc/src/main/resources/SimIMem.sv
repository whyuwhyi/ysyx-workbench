`ifndef PMEM_READ_DEFINED
`define PMEM_READ_DEFINED
import "DPI-C" function int pmem_read(input int addr);
`endif

module SimIMem #(
  parameter XLEN = 32
) (
  input  logic [XLEN-1:0] raddr,
  output logic [31:0] inst
);
  assign inst = pmem_read(raddr);

  function int get_pc_value();
    get_pc_value = raddr;
  endfunction

  function int get_inst_value();
    get_inst_value = inst;
  endfunction

  export "DPI-C" function get_pc_value;
  export "DPI-C" function get_inst_value;
endmodule
