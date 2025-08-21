import "DPI-C" function void ebreak();

module SimEbreak (
  input logic trig
);
  always @(*) begin
    if (trig) ebreak();
  end
endmodule
