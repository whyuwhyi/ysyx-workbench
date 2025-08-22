package sim

import chisel3._
import chisel3.util.HasBlackBoxResource
import chisel3.experimental.IntParam
import common.Constants

class SimProbePC(val xlen: Int)
    extends BlackBox(
      Map(
        "XLEN" -> IntParam(xlen)
      )
    )
    with HasBlackBoxResource {
  val io = IO(new Bundle {
    val pc = Input(UInt(xlen.W))
  })
  addResource("/SimProbePC.sv")
}
