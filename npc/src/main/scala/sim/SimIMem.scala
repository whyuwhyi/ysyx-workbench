package sim

import chisel3._
import chisel3.util.HasBlackBoxResource
import chisel3.experimental.IntParam
import common.Constants

class SimIMem(val xlen: Int)
    extends BlackBox(
      Map(
        "XLEN" -> IntParam(xlen)
      )
    )
    with HasBlackBoxResource {
  val io = IO(new Bundle {
    val raddr = Input(UInt(xlen.W))
    val inst = Output(UInt(32.W))
  })
  addResource("/SimIMem.sv")
}
