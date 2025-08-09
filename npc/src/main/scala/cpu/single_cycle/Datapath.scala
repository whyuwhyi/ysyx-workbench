package cpu.single_cycle

import chisel3._
import chisel3.util._
import common._
import cpu.component._

class Datapath(implicit override val config: CPUConfig) extends Module with Constants {
  val io = IO(new Bundle {
    val ctrl = Flipped(new ControlIO)
    val imem = new IMemIO
    val dmem = new DMemIO
    val ebreak = Output(Bool())
  })

  val pc = RegInit(PC_START)
  val regFile = Module(new RegisterFile)
  val alu = Module(new ALU)
  val immGen = Module(new ImmGen)
  val csr = Module(new CSR)

  val rs1Addr = io.ctrl.inst(19, 15)
  val rs2Addr = io.ctrl.inst(24, 20)
  val rdAddr  = io.ctrl.inst(11, 7)
  val csrAddr = io.ctrl.inst(31, 20)

  regFile.io.rs1Addr := rs1Addr
  regFile.io.rs2Addr := rs2Addr
  val rs1Data = regFile.io.rs1Data
  val rs2Data = regFile.io.rs2Data

  immGen.io.inst := io.ctrl.inst
  val imm = immGen.io.imm

  val opA = Mux(io.ctrl.opASel === OpASel.PC, pc, rs1Data)
  val opB = Mux(io.ctrl.opBSel === OpBSel.IMM, imm, rs2Data)
  
  alu.io.opA := opA
  alu.io.opB := opB
  alu.io.aluOp := io.ctrl.aluOp
  val aluOut = alu.io.out

  val isBranch = io.ctrl.pcSel === PCSel.PC_BR
  val branchCond = MuxLookup(io.ctrl.inst(14, 12), false.B, Seq(
    Funct3.BEQ  -> alu.io.zero,
    Funct3.BNE  -> !alu.io.zero,
    Funct3.BLT  -> aluOut(0),
    Funct3.BGE  -> !aluOut(0),
    Funct3.BLTU -> aluOut(0),
    Funct3.BGEU -> !aluOut(0)
  ))

  val trapValid = io.ctrl.illegalInst || io.ctrl.isEcall || io.ctrl.isEbreak

  val pcPlus4 = pc + 4.U
  val pcBranchTarget = pc + imm
  val pcJumpTarget = aluOut & (~1.U(XLEN.W))
  
  val nextPc = MuxCase(pcPlus4, Seq(
    trapValid                         -> csr.io.mtvec,
    io.ctrl.isMret                    -> csr.io.mepc,
    (io.ctrl.pcSel === PCSel.PC_JMP)  -> (pc + imm),
    (io.ctrl.pcSel === PCSel.PC_JALR) -> pcJumpTarget,
    (isBranch && branchCond)          -> pcBranchTarget
  ))
  pc := nextPc

  io.imem.addr := pc

  io.dmem.addr := aluOut
  io.dmem.wdata := rs2Data
  io.dmem.wen := io.ctrl.memWen && !trapValid
  io.dmem.memType := io.ctrl.memType
  val memRdata = io.dmem.rdata

  csr.io.raddr := csrAddr
  val csrRdata = csr.io.rdata
  
  val isCsrImm = io.ctrl.inst(14,12)(2)
  val csrOperand = Mux(isCsrImm, rs1Addr.zext, rs1Data)
  
  csr.io.trapValid := trapValid
  csr.io.pc := pc
  csr.io.csrCmd := io.ctrl.csrCmd
  csr.io.inWaddr := Mux(trapValid, MCAUSE_ADDR, csrAddr)
  csr.io.inWdata := Mux(trapValid, Mux(io.ctrl.isEcall, ECALL_M_CAUSE, ILLEGAL_INST_CAUSE), csrOperand)

  when (trapValid) {
    csr.io.csrCmd := CSROp.W
    csr.io.inWaddr := MEPC_ADDR
    csr.io.inWdata := pc
  }
  
  val wbData = MuxCase(aluOut, Seq(
    (io.ctrl.wbSel === WBSel.MEM) -> memRdata,
    (io.ctrl.wbSel === WBSel.PC4) -> pcPlus4,
    (io.ctrl.wbSel === WBSel.CSR) -> csrRdata
  ))

  regFile.io.rdAddr := rdAddr
  regFile.io.wdata := wbData
  regFile.io.wen := io.ctrl.regWen && !trapValid

  io.ebreak := io.ctrl.isEbreak
}
