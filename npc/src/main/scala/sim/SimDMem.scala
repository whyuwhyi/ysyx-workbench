package sim

import chisel3._
import chisel3.util.HasBlackBoxResource
import chisel3.experimental.IntParam
import common.Constants

class SimDMem(val xlen: Int)
    extends BlackBox(Map("XLEN" -> IntParam(xlen)))
    with HasBlackBoxResource {
  val io = IO(new Bundle {
    val addr = Input(UInt(xlen.W))
    val ren = Input(Bool())
    val rdata = Output(UInt(xlen.W))
    val wen = Input(Bool())
    val wmask = Input(UInt(8.W))
    val wdata = Input(UInt(xlen.W))
  })
  addResource("/SimDMem.sv")
}
