package cpu.exu

import chisel3._
import chisel3.util._
import common._
import common.ImmSel

class ImmGen extends Module with Constants {
  val io = IO(new Bundle {
    val inst = Input(UInt(32.W))
    val immSel = Input(ImmSel())
    val imm = Output(UInt(XLEN.W))
  })

  val immI = Cat(Fill(20, io.inst(31)), io.inst(31, 20))
  val immS = Cat(Fill(20, io.inst(31)), io.inst(31, 25), io.inst(11, 7))
  val immB = Cat(
    Fill(20, io.inst(31)),
    io.inst(7),
    io.inst(30, 25),
    io.inst(11, 8),
    0.U(1.W)
  )
  val immU = Cat(io.inst(31, 12), 0.U(12.W))
  val immJ = Cat(
    Fill(12, io.inst(31)),
    io.inst(19, 12),
    io.inst(20),
    io.inst(30, 21),
    0.U(1.W)
  )

  val out = MuxLookup(io.immSel, 0.U)(
    Seq(
      ImmSel.I -> immI,
      ImmSel.S -> immS,
      ImmSel.B -> immB,
      ImmSel.U -> immU,
      ImmSel.J -> immJ
    )
  )
  io.imm := out
}
