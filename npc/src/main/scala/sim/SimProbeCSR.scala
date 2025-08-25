package sim

import chisel3._
import chisel3.util.HasBlackBoxResource
import chisel3.experimental.IntParam

class SimProbeCSR(val xlen: Int) extends BlackBox(Map("XLEN" -> IntParam(xlen))) with HasBlackBoxResource {
  val io = IO(new Bundle {
    val mstatus = Input(UInt(xlen.W))
    val mtvec = Input(UInt(xlen.W))
    val mepc = Input(UInt(xlen.W))
    val mcause = Input(UInt(xlen.W))
  })
  addResource("/SimProbeCSR.sv")
}
