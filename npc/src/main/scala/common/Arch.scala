package common

import chisel3._
import chisel3.util._

trait Arch extends HasCPUConfig {
  object Opcodes {
    val LUI = "b0110111".U
    val AUIPC = "b0010111".U
    val JAL = "b1101111".U
    val JALR = "b1100111".U
    val BRANCH = "b1100011".U
    val LOAD = "b0000011".U
    val STORE = "b0100011".U
    val OP_IMM = "b0010011".U
    val OP = "b0110011".U
    val SYSTEM = "b1110011".U
  }
  object Funct3 {
    val ADD_SUB = "b000".U
    val SLL = "b001".U
    val SLT = "b010".U
    val SLTU = "b011".U
    val XOR = "b100".U
    val SR = "b101".U
    val OR = "b110".U
    val AND = "b111".U
    val BEQ = "b000".U
    val BNE = "b001".U
    val BLT = "b100".U
    val BGE = "b101".U
    val BLTU = "b110".U
    val BGEU = "b111".U
    val LB = "b000".U
    val LH = "b001".U
    val LW = "b010".U
    val LBU = "b100".U
    val LHU = "b101".U
    val SB = "b000".U
    val SH = "b001".U
    val SW = "b010".U
    val PRIV = "b000".U
    val CSRRW = "b001".U
    val CSRRS = "b010".U
    val CSRRC = "b011".U
    val CSRRWI = "b101".U
    val CSRRSI = "b110".U
    val CSRRCI = "b111".U
  }
  object Funct7 {
    val ADD = "b0000000".U
    val SUB = "b0100000".U
    val SRL = "b0000000".U
    val SRA = "b0100000".U
  }
  def immI(inst: UInt): UInt =
    Cat(Fill(config.xlen - 12, inst(31)), inst(31, 20))
  def immS(inst: UInt): UInt =
    Cat(Fill(config.xlen - 12, inst(31)), inst(31, 25), inst(11, 7))
  def immB(inst: UInt): UInt = Cat(
    Fill(config.xlen - 13, inst(31)),
    inst(31),
    inst(7),
    inst(30, 25),
    inst(11, 8),
    0.U(1.W)
  )
  def immU(inst: UInt): UInt = Cat(inst(31, 12), 0.U(12.W)).asUInt
  def immJ(inst: UInt): UInt = Cat(
    Fill(config.xlen - 21, inst(31)),
    inst(31),
    inst(19, 12),
    inst(20),
    inst(30, 21),
    0.U(1.W)
  )
  def csrIsImm(inst: UInt): Bool = inst(14, 12)(2)
}
