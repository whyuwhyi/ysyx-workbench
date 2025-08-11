package sim

import chisel3._
import chisel3.util._
import common._
import cpu._
import common.CoreType

class SimTop extends Module {
  implicit val config: CPUConfig = CPUConfig(
    xlen = 32,
    coreType = CoreType.SingleCycle,
    zicsr = true,
    trapOnMisalign = true,
    nrRegs = 16
  )
  val io = IO(new Bundle {})
  val core = Module(new Core)
  val mem = Module(new SimMem)
  val ctrl = Module(new SimCtrl)
  mem.io.clk := clock
  ctrl.io.clk := clock
  ctrl.io.rst := reset
  ctrl.io.ebreak := core.io.ebreak
  mem.io.raddr := core.io.imem.addr
  core.io.imem.inst := mem.io.rdata
  core.io.dmem.rdata := mem.io.rdata
  mem.io.wen := core.io.dmem.wen
  mem.io.waddr := core.io.dmem.addr
  mem.io.wdata := core.io.dmem.wdata
  val lanes = config.xlen / 8
  val fullMask = Fill(lanes, 1.U(1.W))
  val byteMask = (1.U(lanes.W) << core.io.dmem.addr(1, 0))
  val halfMask =
    Mux(core.io.dmem.addr(1), "b1100".U(lanes.W), "b0011".U(lanes.W))
  val mt = core.io.dmem.memType.asUInt
  val selMask = MuxLookup(mt, 0.U(lanes.W))(
    Seq(
      MemType.B.asUInt -> byteMask,
      MemType.H.asUInt -> halfMask,
      MemType.W.asUInt -> fullMask
    )
  )
  mem.io.wmask := Mux(core.io.dmem.wen, selMask, 0.U(lanes.W))
}

object Generator extends App {
  emitVerilog(new SimTop, Array("--target-dir", "build"))
}
