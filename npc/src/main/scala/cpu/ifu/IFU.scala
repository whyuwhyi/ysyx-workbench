package cpu.ifu

import chisel3._
import chisel3.util._
import common._
import common.BuildFlags

class IFU extends Module with Constants {
  val io = IO(new Bundle {
    val pcSel = Input(PCSel())
    val branchCond = Input(BranchCond())
    val branchFlags = Input(new BranchFlags)
    val imm = Input(UInt(XLEN.W))
    val rs1 = Input(UInt(XLEN.W))
    val redirValid = Input(Bool())
    val redirPc = Input(UInt(XLEN.W))
    val inst = Output(UInt(32.W))
    val pc = Output(UInt(XLEN.W))
    val pc4 = Output(UInt(XLEN.W))
  })

  val pcInst = Module(new PC)
  pcInst.io.pcSel := io.pcSel
  pcInst.io.branchCond := io.branchCond
  pcInst.io.branchFlags := io.branchFlags
  pcInst.io.imm := io.imm
  pcInst.io.rs1 := io.rs1
  pcInst.io.redirValid := io.redirValid
  pcInst.io.redirPc := io.redirPc

  io.pc := pcInst.io.pc
  io.pc4 := pcInst.io.pc4

  if (BuildFlags.sim) {
    val simIMemInst = Module(new sim.SimIMem(xlen = XLEN))
    simIMemInst.io.raddr := pcInst.io.pc
    io.inst := simIMemInst.io.inst
  } else {
    io.inst := 0.U(32.W)
  }
}
