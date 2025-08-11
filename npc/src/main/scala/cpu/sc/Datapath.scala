package cpu.sc

import chisel3._
import chisel3.util._
import common._
import cpu.component._
import cpu.csr._
import cpu.lsu._
import common.Arch

class Datapath(implicit override val config: CPUConfig)
    extends chisel3.Module
    with Arch
    with Constants {
  val io = IO(new chisel3.Bundle {
    val ctrl = Flipped(new ControlIO)
    val imem = new IMemIO
    val dmem = new DMemIO
    val ebreak = Output(Bool())
  })
  val pc = RegInit(PC_START)
  io.imem.addr := pc
  val inst = io.imem.inst
  io.ctrl.inst := inst
  val regFile = Module(new RegisterFile)
  val alu = Module(new ALU)
  val immGen = Module(new ImmGen)
  val csr = Module(new CSRFile)
  val lsu = Module(new LSU)
  val rs1Addr = inst(19, 15)
  val rs2Addr = inst(24, 20)
  val rdAddr = inst(11, 7)
  val csrAddr = inst(31, 20)
  regFile.io.rs1Addr := rs1Addr
  regFile.io.rs2Addr := rs2Addr
  immGen.io.inst := inst
  val opA = Mux(io.ctrl.opASel === OpASel.PC, pc, regFile.io.rs1Data)
  val opBimm = MuxLookup(inst(6, 0), 0.U(config.xlen.W))(
    Seq(
      "b0010011".U -> immGen.io.immI,
      "b0000011".U -> immGen.io.immI,
      "b0100011".U -> immGen.io.immS,
      "b1100011".U -> immGen.io.immB,
      "b0110111".U -> immGen.io.immU,
      "b0010111".U -> immGen.io.immU,
      "b1101111".U -> immGen.io.immJ,
      "b1100111".U -> immGen.io.immI,
      "b1110011".U -> immGen.io.immI
    )
  )
  val opB = Mux(io.ctrl.opBSel === OpBSel.IMM, opBimm, regFile.io.rs2Data)
  alu.io.opA := opA
  alu.io.opB := opB
  alu.io.aluOp := io.ctrl.aluOp
  lsu.io.addrIn := alu.io.out
  lsu.io.wdataIn := regFile.io.rs2Data
  lsu.io.memTypeIn := io.ctrl.memType
  lsu.io.ren := io.ctrl.memRen
  lsu.io.wen := io.ctrl.memWen
  lsu.io.rdataMem := io.dmem.rdata
  io.dmem.addr := lsu.io.addrOut
  io.dmem.wdata := lsu.io.wdataOut
  io.dmem.wen := lsu.io.wenOut
  io.dmem.memType := lsu.io.memTypeOut
  val csrImm = Cat(0.U((config.xlen - 5).W), rs1Addr)
  val csrOperand = Mux(csrIsImm(inst), csrImm, regFile.io.rs1Data)
  val memFault = lsu.io.misalign && (io.ctrl.memRen || io.ctrl.memWen)
  val trapEn = if (config.trapOnMisalign) memFault else false.B
  csr.io.trapValid := io.ctrl.illegalInst || trapEn
  csr.io.isEcall := io.ctrl.isEcall
  csr.io.isEbreak := io.ctrl.isEbreak
  csr.io.isMret := io.ctrl.isMret
  csr.io.csrCmd := io.ctrl.csrCmd
  csr.io.raddr := csrAddr
  csr.io.inWaddr := csrAddr
  csr.io.inWdata := csrOperand
  csr.io.pc := pc
  val pc4 = pc + 4.U
  val brTaken = MuxLookup(inst(14, 12), false.B)(
    Seq(
      "b000".U -> alu.io.zero,
      "b001".U -> !alu.io.zero,
      "b100".U -> (alu.io.out(0) === 1.U),
      "b101".U -> (alu.io.out(0) === 0.U),
      "b110".U -> (alu.io.out(0) === 1.U),
      "b111".U -> (alu.io.out(0) === 0.U)
    )
  )
  val pcBr = Mux(brTaken, pc + immGen.io.immB, pc4)
  val pcJ = pc + immGen.io.immJ
  val pcJalr = (regFile.io.rs1Data + immGen.io.immI) & Cat(
    Fill(config.xlen - 1, 1.U(1.W)),
    0.U(1.W)
  )
  val pcNextCore = MuxLookup(io.ctrl.pcSel.asUInt, pc4)(
    Seq(
      PCSel.PC_4.asUInt -> pc4,
      PCSel.PC_BR.asUInt -> pcBr,
      PCSel.PC_JMP.asUInt -> pcJ,
      PCSel.PC_JALR.asUInt -> pcJalr
    )
  )
  val pcNext = Mux(csr.io.takeTrap, csr.io.nextPc, pcNextCore)
  pc := pcNext
  val memRdata = lsu.io.rdataOut
  val csrRdata = csr.io.rdata
  val wbData = MuxLookup(io.ctrl.wbSel.asUInt, alu.io.out)(
    Seq(
      WBSel.MEM.asUInt -> memRdata,
      WBSel.PC4.asUInt -> pc4,
      WBSel.CSR.asUInt -> csrRdata
    )
  )
  regFile.io.rdAddr := rdAddr
  regFile.io.wdata := wbData
  regFile.io.wen := io.ctrl.regWen && !csr.io.takeTrap
  io.ebreak := io.ctrl.isEbreak
}
