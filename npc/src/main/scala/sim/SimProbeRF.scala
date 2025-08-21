package sim

import chisel3._
import chisel3.util.HasBlackBoxResource
import chisel3.experimental.IntParam

class SimProbeRF(val xlen: Int, val nrReg: Int)
    extends BlackBox(
      Map(
        "XLEN" -> IntParam(xlen),
        "NR_REG" -> IntParam(nrReg)
      )
    )
    with HasBlackBoxResource {
  val io = IO(new Bundle {
    val gprs = Input(UInt((xlen * nrReg).W))
  })
  addResource("/SimProbeRF.sv")
}
