module SimProbeCSR #(
  parameter XLEN   = 32
) (
  input logic [XLEN-1:0] mstatus,
  input logic [XLEN-1:0] mtvec,
  input logic [XLEN-1:0] mepc,
  input logic [XLEN-1:0] mcause
);

  export "DPI-C" function get_csr_value;
  function int get_csr_value(input int addr);
    case (addr)
      32'h300: get_csr_value = mstatus;
      32'h305: get_csr_value = mtvec;
      32'h341: get_csr_value = mepc;
      32'h342: get_csr_value = mcause;
      default: get_csr_value = 0;
    endcase
  endfunction

endmodule
