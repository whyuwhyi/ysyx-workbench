package common

import chisel3._

trait HasCPUConfig {
  implicit val config: CPUConfig
}

abstract class BaseCoreIO(implicit override val config: CPUConfig) extends Bundle with HasCPUConfig with Constants

class IMemIO(implicit override val config: CPUConfig) extends BaseCoreIO {
  val addr = Output(UInt(XLEN.W))
  val inst = Input(UInt(XLEN.W))
}

class DMemIO(implicit override val config: CPUConfig) extends BaseCoreIO {
  val addr = Output(UInt(XLEN.W))
  val rdata = Input(UInt(XLEN.W))
  val wen = Output(Bool())
  val wdata = Output(UInt(XLEN.W))
  val memType = Output(MemType())
}

class CoreIO(implicit override val config: CPUConfig) extends BaseCoreIO {
    val imem = new IMemIO
    val dmem = new DMemIO
    val ebreak = Output(Bool())
}

abstract class Core(implicit val config: CPUConfig) extends Module with Constants {
  val io = IO(new CoreIO)
}
