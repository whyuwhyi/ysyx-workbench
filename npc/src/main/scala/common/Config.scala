package common

import chisel3._

object CoreType extends ChiselEnum { val SingleCycle = Value }

case class CPUConfig(
    xlen: Int = 32,
    coreType: CoreType.Type = CoreType.SingleCycle,
    zicsr: Boolean = true,
    trapOnMisalign: Boolean = true,
    nrRegs: Int = 16
) {
  val isRV64: Boolean = xlen == 64
}

trait HasCPUConfig { implicit val config: CPUConfig }
