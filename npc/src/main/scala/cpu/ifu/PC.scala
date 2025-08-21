package cpu.ifu

import chisel3._
import chisel3.util._
import common._
import common.BuildFlags

class PC extends Module with Constants {
  val io = IO(new Bundle {
    val pcSel = Input(PCSel())
    val branchCond = Input(BranchCond())
    val branchFlags = Input(new BranchFlags)
    val imm = Input(UInt(XLEN.W))
    val rs1 = Input(UInt(XLEN.W))
    val redirValid = Input(Bool())
    val redirPc = Input(UInt(XLEN.W))
    val pc = Output(UInt(XLEN.W))
    val pc4 = Output(UInt(XLEN.W))
  })

  val pcReg = RegInit(PC_START.U(XLEN.W))
  io.pc := pcReg
  val pcPlus4 = pcReg + 4.U
  io.pc4 := pcPlus4

  val addA = Mux(io.pcSel === PCSel.PC_JALR, io.rs1, pcReg)
  val addB = io.imm
  val sum = addA + addB
  val targetPre = Mux(io.pcSel === PCSel.PC_JALR, sum & (~1.U(XLEN.W)), sum)

  val take = MuxLookup(io.branchCond, false.B)(
    Seq(
      BranchCond.EQ -> io.branchFlags.isZero,
      BranchCond.NE -> !io.branchFlags.isZero,
      BranchCond.LT -> io.branchFlags.isLessSigned,
      BranchCond.GE -> !io.branchFlags.isLessSigned,
      BranchCond.LTU -> io.branchFlags.isLessUnsigned,
      BranchCond.GEU -> !io.branchFlags.isLessUnsigned
    )
  )

  val coreNext = MuxLookup(io.pcSel, pcPlus4)(
    Seq(
      PCSel.PC_4 -> pcPlus4,
      PCSel.PC_BR -> Mux(take, targetPre, pcPlus4),
      PCSel.PC_JAL -> targetPre,
      PCSel.PC_JALR -> targetPre
    )
  )

  val nextPc = Mux(io.redirValid, io.redirPc, coreNext)
  pcReg := nextPc

  if (BuildFlags.sim) {
    val simProbePCInst = Module(new sim.SimProbePC(xlen = XLEN))
    simProbePCInst.io.pc := pcReg
  }
}
