package cpu.component

import chisel3._
import chisel3.util._
import common._

class ImmGen(implicit override val config: CPUConfig) extends Module with Constants {
  val io = IO(new Bundle {
    val inst = Input(UInt(XLEN.W))
    val imm = Output(UInt(XLEN.W))
  })
  
  val inst = io.inst
  val immI = inst(31, 20).sextTo(XLEN)
  val immS = Cat(inst(31, 25), inst(11, 7)).sextTo(XLEN)
  val immB = Cat(inst(31), inst(7), inst(30, 25), inst(11, 8), 0.U(1.W)).sextTo(XLEN)
  val immU = Cat(inst(31, 12), 0.U(12.W))
  val immJ = Cat(inst(31), inst(19, 12), inst(20), inst(30, 21), 0.U(1.W)).sextTo(XLEN)
  
  io.imm := MuxLookup(inst(6, 2), immI, Seq(
    "b11011".U -> immJ,
    "b01101".U -> immU,
    "b00101".U -> immU,
    "b11000".U -> immB,
    "b01000".U -> immS,
    "b11100".U -> immI
  ))
}
