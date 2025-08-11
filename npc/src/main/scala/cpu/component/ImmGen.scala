package cpu.component

import chisel3._
import common._
import common.Arch

class ImmGen(implicit override val config: CPUConfig)
    extends chisel3.Module
    with Constants
    with Arch {
  val io = IO(new chisel3.Bundle {
    val inst = Input(UInt(config.xlen.W));
    val immI = Output(UInt(config.xlen.W));
    val immS = Output(UInt(config.xlen.W));
    val immB = Output(UInt(config.xlen.W));
    val immU = Output(UInt(config.xlen.W));
    val immJ = Output(UInt(config.xlen.W))
  })
  io.immI := immI(io.inst)
  io.immS := immS(io.inst)
  io.immB := immB(io.inst)
  io.immU := immU(io.inst)
  io.immJ := immJ(io.inst)
}
