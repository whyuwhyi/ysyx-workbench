package sim

import chisel3._
import chisel3.util.HasBlackBoxResource

class SimEbreak extends BlackBox with HasBlackBoxResource {
  val io = IO(new Bundle {
    val trig = Input(Bool())
  })
  addResource("/SimEbreak.sv")
}
