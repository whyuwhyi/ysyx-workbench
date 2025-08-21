package common

trait Constants {
  final val XLEN: Int = 32
  final val NR_REG: Int = 16
  final val PC_START: BigInt = 0x80000000L

  final val MSTATUS_ADDR: Int = 0x300
  final val MEPC_ADDR: Int = 0x341
  final val MCAUSE_ADDR: Int = 0x342
  final val MTVEC_ADDR: Int = 0x305
  final val ECALL_M_CAUSE: Int = 11
  final val ILLEGAL_INST_CAUSE: Int = 2
}
