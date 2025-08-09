package sim

import chisel3._
import chisel3.util._
import common._
import cpu.Core

class SimTop extends Module {
  implicit val config = CPUConfig(XLEN = 32, extensions = Seq('I', 'E'))

  val core = Module(new Core)
  val mem = Module(new SimMem)
  val ctrl = Module(new SimCtrl)

  mem.io.clk := clock
  mem.io.raddr := core.io.imem.addr
  core.io.imem.inst := mem.io.rdata

  mem.io.wen := core.io.dmem.wen
  mem.io.waddr := core.io.dmem.addr
  mem.io.wdata := core.io.dmem.wdata

  val wmask = Wire(UInt((config.XLEN / 8).W))
  wmask := 0.U
  switch(core.io.dmem.memType) {
    is(MemType.B, MemType.BU) { wmask := "b0001".U << core.io.dmem.addr(1, 0) }
    is(MemType.H, MemType.HU) { wmask := "b0011".U << core.io.dmem.addr(1, 0) }
    is(MemType.W) { wmask := "b1111".U }
  }
  mem.io.wmask := wmask

  val rdataB = mem.io.rdata(7, 0).sextTo(config.XLEN)
  val rdataBU = mem.io.rdata(7, 0).zextTo(config.XLEN)
  val rdataH = mem.io.rdata(15, 0).sextTo(config.XLEN)
  val rdataHU = mem.io.rdata(15, 0).zextTo(config.XLEN)

  core.io.dmem.rdata := MuxLookup(
    core.io.dmem.memType.asUInt,
    mem.io.rdata,
    Seq(
      MemType.B.asUInt -> rdataB,
      MemType.BU.asUInt -> rdataBU,
      MemType.H.asUInt -> rdataH,
      MemType.HU.asUInt -> rdataHU
    )
  )

  ctrl.io.clk := clock
  ctrl.io.rst := reset
  ctrl.io.ebreak := core.io.ebreak
}

object Generator extends App {
  def main(args: Array[String]): Unit = {
    emitVerilog(new SimTop(), Array("--target-dir", "build"))
  }
}
