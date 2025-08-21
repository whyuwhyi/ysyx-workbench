
module SimProbeRF #(
  parameter XLEN   = 32,
  parameter NR_REG = 16
) (
  input  logic [XLEN*NR_REG-1:0] gprs
);

  export "DPI-C" function get_reg_value;
  function int get_reg_value(input int reg_idx);
    if (reg_idx >= 1 && reg_idx < NR_REG) begin
      get_reg_value = gprs[(reg_idx+1)*XLEN-1 -: XLEN];
    end else begin
      get_reg_value = 0;
    end
  endfunction

endmodule
