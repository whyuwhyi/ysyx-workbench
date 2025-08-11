package common

import chisel3._

abstract class BaseCoreIO(implicit override val config: CPUConfig)
    extends Bundle
    with HasCPUConfig
class IMemIO(implicit override val config: CPUConfig) extends BaseCoreIO {
  val addr = Output(UInt(config.xlen.W)); val inst = Input(UInt(config.xlen.W))
}
class DMemIO(implicit override val config: CPUConfig) extends BaseCoreIO {
  val addr = Output(UInt(config.xlen.W));
  val rdata = Input(UInt(config.xlen.W)); val wen = Output(Bool());
  val wdata = Output(UInt(config.xlen.W)); val memType = Output(MemType())
}
class CoreIO(implicit override val config: CPUConfig) extends BaseCoreIO {
  val imem = new IMemIO; val dmem = new DMemIO; val ebreak = Output(Bool())
}
abstract class CoreBase(implicit val config: CPUConfig)
    extends chisel3.Module
    with Constants {
  val io = chisel3.IO(new CoreIO)
}
