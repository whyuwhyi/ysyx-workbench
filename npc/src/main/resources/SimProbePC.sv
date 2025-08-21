module SimProbePC #(
  parameter XLEN = 32
) (
  input  logic [XLEN-1:0] pc
);

  function int get_pc_value();
    get_pc_value = pc;
  endfunction
  export "DPI-C" function get_pc_value;

endmodule
