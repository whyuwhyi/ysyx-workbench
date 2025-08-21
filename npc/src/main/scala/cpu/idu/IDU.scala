package cpu.idu

import chisel3._
import chisel3.util._
import common._
import common.BuildFlags

class IDU extends Module with Constants {
  val io = IO(new Bundle {
    val inst = Input(UInt(32.W))

    val pcSel = Output(PCSel())
    val opASel = Output(OpASel())
    val opBSel = Output(OpBSel())
    val aluOp = Output(AluOp())
    val wbSel = Output(WBSel())
    val regWen = Output(Bool())

    val memWen = Output(Bool())
    val memRen = Output(Bool())
    val memType = Output(MemType())

    val csrCmd = Output(CSROp())

    val branchCond = Output(BranchCond())
    val immSel = Output(ImmSel())

    val isEcall = Output(Bool())
    val isEbreak = Output(Bool())
    val isMret = Output(Bool())
    val illegalInst = Output(Bool())
  })

  val defaultSignalsData = ControlSignalsData()
  val lookupTable = RV32I.instructions
  
  val controlSignalsList = ListLookup(io.inst, defaultSignalsData.toList, lookupTable)
  val controlSignals = ControlSignalsBundle.fromList(controlSignalsList)

  io.pcSel := controlSignals.pcSel
  io.opASel := controlSignals.opASel
  io.opBSel := controlSignals.opBSel
  io.aluOp := controlSignals.aluOp
  io.wbSel := controlSignals.wbSel
  io.regWen := controlSignals.regWen
  io.memWen := controlSignals.memWen
  io.memRen := controlSignals.memRen
  io.memType := controlSignals.memType
  io.csrCmd := controlSignals.csrCmd
  io.branchCond := controlSignals.branchCond
  io.immSel := controlSignals.immSel
  io.isEcall := controlSignals.isEcall
  io.isEbreak := controlSignals.isEbreak
  io.isMret := controlSignals.isMret
  io.illegalInst := controlSignals.illegalInst

  if (BuildFlags.sim) {
    val simEbreakInst = Module(new sim.SimEbreak)
    simEbreakInst.io.trig := io.isEbreak
  }
}
