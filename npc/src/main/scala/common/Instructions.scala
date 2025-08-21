package common

import chisel3._
import chisel3.util._
import chisel3.util.experimental.decode._

// RV32I instruction patterns using DecodePattern
case class RV32IInstruction(
    name: String,
    bitPat: BitPat
) extends DecodePattern

// Decode fields for each control signal
object PCSelField extends DecodeField[RV32IInstruction, PCSel.Type] {
  def name = "pcSel"
  def chiselType = PCSel()
  def genTable(inst: RV32IInstruction): BitPat = inst.name match {
    case "BEQ" | "BNE" | "BLT" | "BGE" | "BLTU" | "BGEU" =>
      BitPat(PCSel.PC_BR.litValue.U(PCSel.getWidth.W))
    case "JAL"  => BitPat(PCSel.PC_JAL.litValue.U(PCSel.getWidth.W))
    case "JALR" => BitPat(PCSel.PC_JALR.litValue.U(PCSel.getWidth.W))
    case _      => BitPat(PCSel.PC_4.litValue.U(PCSel.getWidth.W))
  }
}

object OpASelField extends DecodeField[RV32IInstruction, OpASel.Type] {
  def name = "opASel"
  def chiselType = OpASel()
  def genTable(inst: RV32IInstruction): BitPat = inst.name match {
    case "JAL" | "JALR" | "AUIPC" =>
      BitPat(OpASel.PC.litValue.U(OpASel.getWidth.W))
    case _ => BitPat(OpASel.RS1.litValue.U(OpASel.getWidth.W))
  }
}

object OpBSelField extends DecodeField[RV32IInstruction, OpBSel.Type] {
  def name = "opBSel"
  def chiselType = OpBSel()
  def genTable(inst: RV32IInstruction): BitPat = inst.name match {
    case "ADD" | "SUB" | "AND" | "OR" | "XOR" | "SLL" | "SRL" | "SRA" | "SLT" |
        "SLTU" =>
      BitPat(OpBSel.RS2.litValue.U(OpBSel.getWidth.W))
    case _ => BitPat(OpBSel.IMM.litValue.U(OpBSel.getWidth.W))
  }
}

object AluOpField extends DecodeField[RV32IInstruction, AluOp.Type] {
  def name = "aluOp"
  def chiselType = AluOp()
  def genTable(inst: RV32IInstruction): BitPat = inst.name match {
    case "ADD" | "ADDI" | "LB" | "LH" | "LW" | "LBU" | "LHU" | "SB" | "SH" |
        "SW" | "JAL" | "JALR" | "AUIPC" =>
      BitPat(AluOp.ADD.litValue.U(AluOp.getWidth.W))
    case "SUB" | "BEQ" | "BNE" | "BLT" | "BGE" | "BLTU" | "BGEU" =>
      BitPat(AluOp.SUB.litValue.U(AluOp.getWidth.W))
    case "AND" | "ANDI"   => BitPat(AluOp.AND.litValue.U(AluOp.getWidth.W))
    case "OR" | "ORI"     => BitPat(AluOp.OR.litValue.U(AluOp.getWidth.W))
    case "XOR" | "XORI"   => BitPat(AluOp.XOR.litValue.U(AluOp.getWidth.W))
    case "SLL" | "SLLI"   => BitPat(AluOp.SLL.litValue.U(AluOp.getWidth.W))
    case "SRL" | "SRLI"   => BitPat(AluOp.SRL.litValue.U(AluOp.getWidth.W))
    case "SRA" | "SRAI"   => BitPat(AluOp.SRA.litValue.U(AluOp.getWidth.W))
    case "SLT" | "SLTI"   => BitPat(AluOp.SLT.litValue.U(AluOp.getWidth.W))
    case "SLTU" | "SLTIU" => BitPat(AluOp.SLTU.litValue.U(AluOp.getWidth.W))
    case "LUI"            => BitPat(AluOp.COPY_B.litValue.U(AluOp.getWidth.W))
    case _                => BitPat(AluOp.ADD.litValue.U(AluOp.getWidth.W))
  }
}

object WBSelField extends DecodeField[RV32IInstruction, WBSel.Type] {
  def name = "wbSel"
  def chiselType = WBSel()
  def genTable(inst: RV32IInstruction): BitPat = inst.name match {
    case "LB" | "LH" | "LW" | "LBU" | "LHU" =>
      BitPat(WBSel.MEM.litValue.U(WBSel.getWidth.W))
    case "JAL" | "JALR" => BitPat(WBSel.PC4.litValue.U(WBSel.getWidth.W))
    case "CSRRW" | "CSRRS" | "CSRRC" | "CSRRWI" | "CSRRSI" | "CSRRCI" =>
      BitPat(WBSel.CSR.litValue.U(WBSel.getWidth.W))
    case _ => BitPat(WBSel.ALU.litValue.U(WBSel.getWidth.W))
  }
}

object RegWenField extends BoolDecodeField[RV32IInstruction] {
  def name = "regWen"
  def genTable(inst: RV32IInstruction): BitPat = inst.name match {
    case "SB" | "SH" | "SW" | "BEQ" | "BNE" | "BLT" | "BGE" | "BLTU" | "BGEU" |
        "ECALL" | "EBREAK" | "MRET" =>
      n
    case _ => y
  }
}

object MemWenField extends BoolDecodeField[RV32IInstruction] {
  def name = "memWen"
  def genTable(inst: RV32IInstruction): BitPat = inst.name match {
    case "SB" | "SH" | "SW" => y
    case _                  => n
  }
}

object MemRenField extends BoolDecodeField[RV32IInstruction] {
  def name = "memRen"
  def genTable(inst: RV32IInstruction): BitPat = inst.name match {
    case "LB" | "LH" | "LW" | "LBU" | "LHU" => y
    case _                                  => n
  }
}

object MemTypeField extends DecodeField[RV32IInstruction, MemType.Type] {
  def name = "memType"
  def chiselType = MemType()
  def genTable(inst: RV32IInstruction): BitPat = inst.name match {
    case "LB" | "SB" => BitPat(MemType.B.litValue.U(MemType.getWidth.W))
    case "LH" | "SH" => BitPat(MemType.H.litValue.U(MemType.getWidth.W))
    case "LW" | "SW" => BitPat(MemType.W.litValue.U(MemType.getWidth.W))
    case "LBU"       => BitPat(MemType.BU.litValue.U(MemType.getWidth.W))
    case "LHU"       => BitPat(MemType.HU.litValue.U(MemType.getWidth.W))
    case _           => BitPat(MemType.NONE.litValue.U(MemType.getWidth.W))
  }
}

object CSROpField extends DecodeField[RV32IInstruction, CSROp.Type] {
  def name = "csrCmd"
  def chiselType = CSROp()
  def genTable(inst: RV32IInstruction): BitPat = inst.name match {
    case "CSRRW" | "CSRRWI" => BitPat(CSROp.RW.litValue.U(CSROp.getWidth.W))
    case "CSRRS" | "CSRRSI" => BitPat(CSROp.RS.litValue.U(CSROp.getWidth.W))
    case "CSRRC" | "CSRRCI" => BitPat(CSROp.RC.litValue.U(CSROp.getWidth.W))
    case _                  => BitPat(CSROp.NONE.litValue.U(CSROp.getWidth.W))
  }
}

object BranchCondField extends DecodeField[RV32IInstruction, BranchCond.Type] {
  def name = "branchCond"
  def chiselType = BranchCond()
  def genTable(inst: RV32IInstruction): BitPat = inst.name match {
    case "BEQ"  => BitPat(BranchCond.EQ.litValue.U(BranchCond.getWidth.W))
    case "BNE"  => BitPat(BranchCond.NE.litValue.U(BranchCond.getWidth.W))
    case "BLT"  => BitPat(BranchCond.LT.litValue.U(BranchCond.getWidth.W))
    case "BGE"  => BitPat(BranchCond.GE.litValue.U(BranchCond.getWidth.W))
    case "BLTU" => BitPat(BranchCond.LTU.litValue.U(BranchCond.getWidth.W))
    case "BGEU" => BitPat(BranchCond.GEU.litValue.U(BranchCond.getWidth.W))
    case _      => BitPat(BranchCond.NONE.litValue.U(BranchCond.getWidth.W))
  }
}

object ImmSelField extends DecodeField[RV32IInstruction, ImmSel.Type] {
  def name = "immSel"
  def chiselType = ImmSel()
  def genTable(inst: RV32IInstruction): BitPat = inst.name match {
    case "ADDI" | "ANDI" | "ORI" | "XORI" | "SLTI" | "SLTIU" | "SLLI" | "SRLI" |
        "SRAI" | "LB" | "LH" | "LW" | "LBU" | "LHU" | "JALR" | "CSRRW" |
        "CSRRS" | "CSRRC" | "CSRRWI" | "CSRRSI" | "CSRRCI" =>
      BitPat(ImmSel.I.litValue.U(ImmSel.getWidth.W))
    case "SB" | "SH" | "SW" => BitPat(ImmSel.S.litValue.U(ImmSel.getWidth.W))
    case "BEQ" | "BNE" | "BLT" | "BGE" | "BLTU" | "BGEU" =>
      BitPat(ImmSel.B.litValue.U(ImmSel.getWidth.W))
    case "LUI" | "AUIPC" => BitPat(ImmSel.U.litValue.U(ImmSel.getWidth.W))
    case "JAL"           => BitPat(ImmSel.J.litValue.U(ImmSel.getWidth.W))
    case _               => BitPat(ImmSel.NONE.litValue.U(ImmSel.getWidth.W))
  }
}

object IsEcallField extends BoolDecodeField[RV32IInstruction] {
  def name = "isEcall"
  def genTable(inst: RV32IInstruction): BitPat =
    if (inst.name == "ECALL") y else n
}

object IsEbreakField extends BoolDecodeField[RV32IInstruction] {
  def name = "isEbreak"
  def genTable(inst: RV32IInstruction): BitPat =
    if (inst.name == "EBREAK") y else n
}

object IsMretField extends BoolDecodeField[RV32IInstruction] {
  def name = "isMret"
  def genTable(inst: RV32IInstruction): BitPat =
    if (inst.name == "MRET") y else n
}

object IllegalInstField extends BoolDecodeField[RV32IInstruction] {
  def name = "illegalInst"
  def genTable(inst: RV32IInstruction): BitPat =
    n // All defined instructions are legal
}

// RV32I instruction set definition
object RV32I {
  // Bit pattern macros for cleaner definition
  def toBinary(value: Int, width: Int): String = {
    val binary = value.toBinaryString
    if (binary.length > width) binary.takeRight(width)
    else "0" * (width - binary.length) + binary
  }

  def R_TYPE(funct7: Int, funct3: Int): String = {
    val f7 = toBinary(funct7, 7)
    val f3 = toBinary(funct3, 3)
    s"b${f7}_?????_?????_${f3}_?????_0110011"
  }

  def I_TYPE(funct3: Int, opcode: Int): String = {
    val f3 = toBinary(funct3, 3)
    val op = toBinary(opcode, 7)
    s"b????????????_?????_${f3}_?????_$op"
  }

  def S_TYPE(funct3: Int): String = {
    val f3 = toBinary(funct3, 3)
    s"b???????_?????_?????_${f3}_?????_0100011"
  }

  def B_TYPE(funct3: Int): String = {
    val f3 = toBinary(funct3, 3)
    s"b???????_?????_?????_${f3}_?????_1100011"
  }

  def U_TYPE(opcode: Int): String = {
    val op = toBinary(opcode, 7)
    s"b????????????????????_?????_$op"
  }

  def J_TYPE(): String =
    "b????????????????????_?????_1101111"

  val instructionPatterns: Seq[RV32IInstruction] = Seq(
    // R-type ALU instructions
    RV32IInstruction("ADD", BitPat(R_TYPE(0x00, 0x0))),
    RV32IInstruction("SUB", BitPat(R_TYPE(0x20, 0x0))),
    RV32IInstruction("AND", BitPat(R_TYPE(0x00, 0x7))),
    RV32IInstruction("OR", BitPat(R_TYPE(0x00, 0x6))),
    RV32IInstruction("XOR", BitPat(R_TYPE(0x00, 0x4))),
    RV32IInstruction("SLL", BitPat(R_TYPE(0x00, 0x1))),
    RV32IInstruction("SRL", BitPat(R_TYPE(0x00, 0x5))),
    RV32IInstruction("SRA", BitPat(R_TYPE(0x20, 0x5))),
    RV32IInstruction("SLT", BitPat(R_TYPE(0x00, 0x2))),
    RV32IInstruction("SLTU", BitPat(R_TYPE(0x00, 0x3))),

    // I-type ALU instructions
    RV32IInstruction("ADDI", BitPat(I_TYPE(0x0, 0x13))),
    RV32IInstruction("ANDI", BitPat(I_TYPE(0x7, 0x13))),
    RV32IInstruction("ORI", BitPat(I_TYPE(0x6, 0x13))),
    RV32IInstruction("XORI", BitPat(I_TYPE(0x4, 0x13))),
    RV32IInstruction("SLTI", BitPat(I_TYPE(0x2, 0x13))),
    RV32IInstruction("SLTIU", BitPat(I_TYPE(0x3, 0x13))),

    // Shift instructions (I-type with funct7)
    RV32IInstruction(
      "SLLI",
      BitPat(R_TYPE(0x00, 0x1).replace("0110011", "0010011"))
    ),
    RV32IInstruction(
      "SRLI",
      BitPat(R_TYPE(0x00, 0x5).replace("0110011", "0010011"))
    ),
    RV32IInstruction(
      "SRAI",
      BitPat(R_TYPE(0x20, 0x5).replace("0110011", "0010011"))
    ),

    // Load instructions
    RV32IInstruction("LB", BitPat(I_TYPE(0x0, 0x03))),
    RV32IInstruction("LH", BitPat(I_TYPE(0x1, 0x03))),
    RV32IInstruction("LW", BitPat(I_TYPE(0x2, 0x03))),
    RV32IInstruction("LBU", BitPat(I_TYPE(0x4, 0x03))),
    RV32IInstruction("LHU", BitPat(I_TYPE(0x5, 0x03))),

    // Store instructions
    RV32IInstruction("SB", BitPat(S_TYPE(0x0))),
    RV32IInstruction("SH", BitPat(S_TYPE(0x1))),
    RV32IInstruction("SW", BitPat(S_TYPE(0x2))),

    // Branch instructions
    RV32IInstruction("BEQ", BitPat(B_TYPE(0x0))),
    RV32IInstruction("BNE", BitPat(B_TYPE(0x1))),
    RV32IInstruction("BLT", BitPat(B_TYPE(0x4))),
    RV32IInstruction("BGE", BitPat(B_TYPE(0x5))),
    RV32IInstruction("BLTU", BitPat(B_TYPE(0x6))),
    RV32IInstruction("BGEU", BitPat(B_TYPE(0x7))),

    // Jump instructions
    RV32IInstruction("JAL", BitPat(J_TYPE())),
    RV32IInstruction("JALR", BitPat(I_TYPE(0x0, 0x67))),

    // Upper immediate instructions
    RV32IInstruction("LUI", BitPat(U_TYPE(0x37))),
    RV32IInstruction("AUIPC", BitPat(U_TYPE(0x17))),

    // System instructions
    RV32IInstruction("ECALL", BitPat("b000000000000_00000_000_00000_1110011")),
    RV32IInstruction("EBREAK", BitPat("b000000000001_00000_000_00000_1110011")),
    RV32IInstruction("MRET", BitPat("b001100000010_00000_000_00000_1110011")),

    // CSR instructions
    RV32IInstruction("CSRRW", BitPat(I_TYPE(0x1, 0x73))),
    RV32IInstruction("CSRRS", BitPat(I_TYPE(0x2, 0x73))),
    RV32IInstruction("CSRRC", BitPat(I_TYPE(0x3, 0x73))),
    RV32IInstruction("CSRRWI", BitPat(I_TYPE(0x5, 0x73))),
    RV32IInstruction("CSRRSI", BitPat(I_TYPE(0x6, 0x73))),
    RV32IInstruction("CSRRCI", BitPat(I_TYPE(0x7, 0x73)))
  )

  val decodeFields = Seq(
    PCSelField,
    OpASelField,
    OpBSelField,
    AluOpField,
    WBSelField,
    RegWenField,
    MemWenField,
    MemRenField,
    MemTypeField,
    CSROpField,
    BranchCondField,
    ImmSelField,
    IsEcallField,
    IsEbreakField,
    IsMretField,
    IllegalInstField
  )

  val decodeTable = new DecodeTable(instructionPatterns, decodeFields)
}

