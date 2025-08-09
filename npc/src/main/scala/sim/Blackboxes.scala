package sim

import chisel3._
import chisel3.experimental.BlackBox
import common._

class SimMem(implicit override val config: CPUConfig) extends BlackBox with HasCPUConfig with HasBlackBoxResource {
  val io = IO(new Bundle {
    val clk = Input(Clock())
    val raddr = Input(UInt(XLEN.W))
    val rdata = Output(UInt(XLEN.W))
    val wen = Input(Bool())
    val waddr = Input(UInt(XLEN.W))
    val wdata = Input(UInt(XLEN.W))
    val wmask = Input(UInt((XLEN/8).W))
  })
  addResource("/SimBlackboxes.v")
}

class SimCtrl extends BlackBox with HasBlackBoxResource {
  val io = IO(new Bundle {
    val clk = Input(Clock())
    val rst = Input(Reset())
    val ebreak = Input(Bool())
  })
  addResource("/SimBlackboxes.v")
}
