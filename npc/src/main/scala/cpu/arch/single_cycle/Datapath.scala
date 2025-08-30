package cpu.arch.single_cycle

import chisel3._
import chisel3.util._
import common._
import cpu.ifu._
import cpu.exu._
import cpu.lsu._

class Datapath extends Module with Constants {
  val io = IO(new Bundle {
    val pcSel = Input(PCSel())
    val opASel = Input(OpASel())
    val opBSel = Input(OpBSel())
    val aluOp = Input(AluOp())
    val wbSel = Input(WBSel())
    val regWen = Input(Bool())
    val memWen = Input(Bool())
    val memRen = Input(Bool())
    val memType = Input(MemType())
    val branchCond = Input(BranchCond())
    val immSel = Input(ImmSel())
    val redirValid = Input(Bool())
    val redirPc = Input(UInt(XLEN.W))
    val csrRdata = Input(UInt(XLEN.W))

    val inst = Output(UInt(32.W))
    val pc = Output(UInt(XLEN.W))
    val pc4 = Output(UInt(XLEN.W))
    val rs1Data = Output(UInt(XLEN.W))
    val misalign = Output(Bool())
  })

  val ifuInst = Module(new IFU)
  val immGenInst = Module(new ImmGen)
  val aluInst = Module(new ALU)
  val lsuInst = Module(new LSU)
  val registerFileInst = Module(new RegisterFile)

  io.inst := ifuInst.io.inst
  io.pc := ifuInst.io.pc
  io.pc4 := ifuInst.io.pc4

  val rs1Addr = io.inst(19, 15)
  val rs2Addr = io.inst(24, 20)
  val rdAddr = io.inst(11, 7)

  registerFileInst.io.rs1Addr := rs1Addr
  registerFileInst.io.rs2Addr := rs2Addr
  registerFileInst.io.rdAddr := rdAddr
  io.rs1Data := registerFileInst.io.rs1Data

  immGenInst.io.inst := io.inst
  immGenInst.io.immSel := io.immSel

  ifuInst.io.pcSel := io.pcSel
  ifuInst.io.branchCond := io.branchCond
  ifuInst.io.branchFlags := aluInst.io.branchFlags
  ifuInst.io.imm := immGenInst.io.imm
  ifuInst.io.rs1 := registerFileInst.io.rs1Data
  ifuInst.io.redirValid := io.redirValid
  ifuInst.io.redirPc := io.redirPc

  val opA = Mux(
    io.opASel === OpASel.PC,
    ifuInst.io.pc,
    registerFileInst.io.rs1Data
  )
  val opB = Mux(
    io.opBSel === OpBSel.IMM,
    immGenInst.io.imm,
    registerFileInst.io.rs2Data
  )
  aluInst.io.opA := opA
  aluInst.io.opB := opB
  aluInst.io.aluOp := io.aluOp

  lsuInst.io.addr := aluInst.io.aluOut
  lsuInst.io.wdata := registerFileInst.io.rs2Data
  lsuInst.io.memType := io.memType
  lsuInst.io.ren := io.memRen
  lsuInst.io.wen := io.memWen
  io.misalign := lsuInst.io.misalign

  val wbData = MuxLookup(io.wbSel, aluInst.io.aluOut)(
    Seq(
      WBSel.MEM -> lsuInst.io.rdata,
      WBSel.PC4 -> ifuInst.io.pc4,
      WBSel.CSR -> io.csrRdata
    )
  )
  registerFileInst.io.wdata := wbData
  registerFileInst.io.wen := io.regWen
}
