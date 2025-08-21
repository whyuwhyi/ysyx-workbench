package cpu.idu

import chisel3._
import chisel3.util._
import chisel3.util.experimental.decode._
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

    val csrOp = Output(CSROp())

    val branchCond = Output(BranchCond())
    val immSel = Output(ImmSel())

    val isEcall = Output(Bool())
    val isEbreak = Output(Bool())
    val isMret = Output(Bool())
    val illegalInst = Output(Bool())
  })

  val decoded = RV32I.decodeTable.decode(io.inst)

  io.pcSel := decoded(PCSelField)
  io.opASel := decoded(OpASelField)
  io.opBSel := decoded(OpBSelField)
  io.aluOp := decoded(AluOpField)
  io.wbSel := decoded(WBSelField)
  io.regWen := decoded(RegWenField)
  io.memWen := decoded(MemWenField)
  io.memRen := decoded(MemRenField)
  io.memType := decoded(MemTypeField)
  io.csrOp := decoded(CSROpField)
  io.branchCond := decoded(BranchCondField)
  io.immSel := decoded(ImmSelField)
  io.isEcall := decoded(IsEcallField)
  io.isEbreak := decoded(IsEbreakField)
  io.isMret := decoded(IsMretField)
  io.illegalInst := decoded(IllegalInstField)

  if (BuildFlags.sim) {
    val simEbreakInst = Module(new sim.SimEbreak)
    simEbreakInst.io.trig := io.isEbreak
  }
}
