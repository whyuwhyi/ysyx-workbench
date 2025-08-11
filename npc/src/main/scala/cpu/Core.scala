package cpu

import chisel3._
import common._
import cpu.sc._

class Core(implicit override val config: CPUConfig) extends CoreBase {
  val datapath = Module(new Datapath)
  val control = Module(new ControlUnit)
  datapath.io.imem <> io.imem
  datapath.io.dmem <> io.dmem
  control.io.inst := datapath.io.imem.inst
  datapath.io.ctrl <> control.io
  io.ebreak := datapath.io.ebreak
}
