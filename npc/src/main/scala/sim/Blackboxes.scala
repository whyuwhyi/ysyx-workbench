package sim

import chisel3._
import chisel3.BlackBox
import chisel3.util.HasBlackBoxResource
import common._

class SimMem(implicit override val config: CPUConfig)
    extends BlackBox
    with HasCPUConfig
    with HasBlackBoxResource {
  val io = IO(new Bundle {
    val clk = Input(Clock())
    val raddr = Input(UInt(config.xlen.W))
    val rdata = Output(UInt(config.xlen.W))
    val wen = Input(Bool())
    val waddr = Input(UInt(config.xlen.W))
    val wdata = Input(UInt(config.xlen.W))
    val wmask = Input(UInt((config.xlen / 8).W))
  })
  addResource("/SimBlockboxes.v")
}

class SimCtrl extends BlackBox with HasBlackBoxResource {
  val io = IO(new Bundle {
    val clk = Input(Clock())
    val rst = Input(Reset())
    val ebreak = Input(Bool())
  })
  addResource("/SimBlockboxes.v")
}
