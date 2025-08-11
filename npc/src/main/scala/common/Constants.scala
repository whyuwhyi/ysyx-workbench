package common

import chisel3._
import chisel3.util._

trait Constants extends HasCPUConfig {
  def PC_START: UInt = "h80000000".U(config.xlen.W)
  def MSTATUS_ADDR: UInt = "h300".U(12.W)
  def MTVEC_ADDR: UInt = "h305".U(12.W)
  def MEPC_ADDR: UInt = "h341".U(12.W)
  def MCAUSE_ADDR: UInt = "h342".U(12.W)
  def ILLEGAL_INST_CAUSE: UInt = 2.U(config.xlen.W)
  def ECALL_M_CAUSE: UInt = 11.U(config.xlen.W)
}
