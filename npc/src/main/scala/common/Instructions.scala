package common

import chisel3._
import chisel3.util._


sealed trait InstructionFormat
case class RType(opcode: Int, funct3: Int, funct7: Int) extends InstructionFormat
case class IType(opcode: Int, funct3: Int) extends InstructionFormat
case class SType(opcode: Int, funct3: Int) extends InstructionFormat
case class BType(opcode: Int, funct3: Int) extends InstructionFormat
case class UType(opcode: Int) extends InstructionFormat
case class JType(opcode: Int) extends InstructionFormat
case class SystemType(opcode: Int, funct3: Int, imm: Int) extends InstructionFormat

object InstructionFormat {
  private def intToBinary(value: Int, width: Int): String = {
    val binary = value.toBinaryString
    if (binary.length > width) binary.takeRight(width)
    else "0" * (width - binary.length) + binary
  }
  
  def toBitPat(format: InstructionFormat): BitPat = format match {
    case RType(opcode, funct3, funct7) =>
      BitPat(s"b${intToBinary(funct7, 7)}_?????_?????_${intToBinary(funct3, 3)}_?????_${intToBinary(opcode, 7)}")
    case IType(opcode, funct3) =>
      BitPat(s"b????????????_?????_${intToBinary(funct3, 3)}_?????_${intToBinary(opcode, 7)}")
    case SType(opcode, funct3) =>
      BitPat(s"b???????_?????_?????_${intToBinary(funct3, 3)}_?????_${intToBinary(opcode, 7)}")
    case BType(opcode, funct3) =>
      BitPat(s"b???????_?????_?????_${intToBinary(funct3, 3)}_?????_${intToBinary(opcode, 7)}")
    case UType(opcode) =>
      BitPat(s"b????????????????????_?????_${intToBinary(opcode, 7)}")
    case JType(opcode) =>
      BitPat(s"b????????????????????_?????_${intToBinary(opcode, 7)}")
    case SystemType(opcode, funct3, imm) =>
      BitPat(s"b${intToBinary(imm, 12)}_00000_${intToBinary(funct3, 3)}_00000_${intToBinary(opcode, 7)}")
  }
}

case class ControlSignalsData(
  pcSel: PCSel.Type = PCSel.PC_4,
  opASel: OpASel.Type = OpASel.RS1,
  opBSel: OpBSel.Type = OpBSel.RS2,
  aluOp: AluOp.Type = AluOp.ADD,
  wbSel: WBSel.Type = WBSel.ALU,
  regWen: Boolean = false,
  memWen: Boolean = false,
  memRen: Boolean = false,
  memType: MemType.Type = MemType.X,
  csrCmd: CSROp.Type = CSROp.NONE,
  branchCond: BranchCond.Type = BranchCond.NONE,
  immSel: ImmSel.Type = ImmSel.NONE,
  isEcall: Boolean = false,
  isEbreak: Boolean = false,
  isMret: Boolean = false,
  illegalInst: Boolean = true
) {
  
  def toList: List[UInt] = List(
    pcSel.asUInt,
    opASel.asUInt,
    opBSel.asUInt,
    aluOp.asUInt,
    wbSel.asUInt,
    regWen.B.asUInt,
    memWen.B.asUInt,
    memRen.B.asUInt,
    memType.asUInt,
    csrCmd.asUInt,
    branchCond.asUInt,
    immSel.asUInt,
    isEcall.B.asUInt,
    isEbreak.B.asUInt,
    isMret.B.asUInt,
    illegalInst.B.asUInt
  )
  
  def withPCSel(sel: PCSel.Type): ControlSignalsData = copy(pcSel = sel)
  def withOpASel(sel: OpASel.Type): ControlSignalsData = copy(opASel = sel)
  def withOpBSel(sel: OpBSel.Type): ControlSignalsData = copy(opBSel = sel)
  def withAluOp(op: AluOp.Type): ControlSignalsData = copy(aluOp = op)
  def withWBSel(sel: WBSel.Type): ControlSignalsData = copy(wbSel = sel)
  def withRegWen(en: Boolean = true): ControlSignalsData = copy(regWen = en)
  def withMemWen(en: Boolean = true): ControlSignalsData = copy(memWen = en)
  def withMemRen(en: Boolean = true): ControlSignalsData = copy(memRen = en)
  def withMemType(memType: MemType.Type): ControlSignalsData = copy(memType = memType)
  def withCSRCmd(cmd: CSROp.Type): ControlSignalsData = copy(csrCmd = cmd)
  def withBranchCond(cond: BranchCond.Type): ControlSignalsData = copy(branchCond = cond)
  def withImmSel(sel: ImmSel.Type): ControlSignalsData = copy(immSel = sel)
  def withEcall: ControlSignalsData = copy(isEcall = true)
  def withEbreak: ControlSignalsData = copy(isEbreak = true)
  def withMret: ControlSignalsData = copy(isMret = true)
  def withLegal: ControlSignalsData = copy(illegalInst = false)
}

object InstructionHelpers {
  def alu(op: AluOp.Type): ControlSignalsData = {
    ControlSignalsData()
      .withAluOp(op)
      .withRegWen()
      .withLegal
  }
  
  def aluImm(op: AluOp.Type, immType: ImmSel.Type = ImmSel.I): ControlSignalsData = {
    ControlSignalsData()
      .withOpBSel(OpBSel.IMM)
      .withAluOp(op)
      .withRegWen()
      .withImmSel(immType)
      .withLegal
  }
  
  def load(memType: MemType.Type): ControlSignalsData = {
    ControlSignalsData()
      .withOpBSel(OpBSel.IMM)
      .withAluOp(AluOp.ADD)
      .withWBSel(WBSel.MEM)
      .withRegWen()
      .withMemRen()
      .withMemType(memType)
      .withImmSel(ImmSel.I)
      .withLegal
  }
  
  def store(memType: MemType.Type): ControlSignalsData = {
    ControlSignalsData()
      .withOpBSel(OpBSel.IMM)
      .withAluOp(AluOp.ADD)
      .withMemWen()
      .withMemType(memType)
      .withImmSel(ImmSel.S)
      .withLegal
  }
  
  def branch(cond: BranchCond.Type): ControlSignalsData = {
    ControlSignalsData()
      .withPCSel(PCSel.PC_BR)
      .withAluOp(AluOp.SUB)
      .withBranchCond(cond)
      .withImmSel(ImmSel.B)
      .withLegal
  }
  
  def jal: ControlSignalsData = {
    ControlSignalsData()
      .withPCSel(PCSel.PC_JMP)
      .withOpASel(OpASel.PC)
      .withOpBSel(OpBSel.IMM)
      .withAluOp(AluOp.ADD)
      .withWBSel(WBSel.PC4)
      .withRegWen()
      .withImmSel(ImmSel.J)
      .withLegal
  }
  
  def jalr: ControlSignalsData = {
    ControlSignalsData()
      .withPCSel(PCSel.PC_JALR)
      .withOpASel(OpASel.PC)
      .withOpBSel(OpBSel.IMM)
      .withAluOp(AluOp.ADD)
      .withWBSel(WBSel.PC4)
      .withRegWen()
      .withImmSel(ImmSel.I)
      .withLegal
  }
  
  def csr(cmd: CSROp.Type): ControlSignalsData = {
    ControlSignalsData()
      .withOpBSel(OpBSel.IMM)
      .withAluOp(AluOp.ADD)
      .withWBSel(WBSel.CSR)
      .withRegWen()
      .withCSRCmd(cmd)
      .withImmSel(ImmSel.I)
      .withLegal
  }
  
  def system: ControlSignalsData = {
    ControlSignalsData().withLegal
  }
}

object RV32I {
  import InstructionHelpers._
  
  private def inst(format: InstructionFormat, signals: ControlSignalsData): (BitPat, List[UInt]) = {
    InstructionFormat.toBitPat(format) -> signals.toList
  }
  
  val instructions: Array[(BitPat, List[UInt])] = Array(
    // R-type ALU instructions
    inst(RType(0x33, 0x0, 0x00), alu(AluOp.ADD)),     // ADD
    inst(RType(0x33, 0x0, 0x20), alu(AluOp.SUB)),     // SUB
    inst(RType(0x33, 0x7, 0x00), alu(AluOp.AND)),     // AND
    inst(RType(0x33, 0x6, 0x00), alu(AluOp.OR)),      // OR
    inst(RType(0x33, 0x4, 0x00), alu(AluOp.XOR)),     // XOR
    inst(RType(0x33, 0x1, 0x00), alu(AluOp.SLL)),     // SLL
    inst(RType(0x33, 0x5, 0x00), alu(AluOp.SRL)),     // SRL
    inst(RType(0x33, 0x5, 0x20), alu(AluOp.SRA)),     // SRA
    inst(RType(0x33, 0x2, 0x00), alu(AluOp.SLT)),     // SLT
    inst(RType(0x33, 0x3, 0x00), alu(AluOp.SLTU)),    // SLTU
    
    // I-type ALU instructions
    inst(IType(0x13, 0x0), aluImm(AluOp.ADD)),        // ADDI
    inst(IType(0x13, 0x7), aluImm(AluOp.AND)),        // ANDI
    inst(IType(0x13, 0x6), aluImm(AluOp.OR)),         // ORI
    inst(IType(0x13, 0x4), aluImm(AluOp.XOR)),        // XORI
    inst(IType(0x13, 0x2), aluImm(AluOp.SLT)),        // SLTI
    inst(IType(0x13, 0x3), aluImm(AluOp.SLTU)),       // SLTIU
    
    // Shift instructions (special I-type with funct7 field)
    inst(RType(0x13, 0x1, 0x00), aluImm(AluOp.SLL)),  // SLLI
    inst(RType(0x13, 0x5, 0x00), aluImm(AluOp.SRL)),  // SRLI
    inst(RType(0x13, 0x5, 0x20), aluImm(AluOp.SRA)),  // SRAI
    
    // Load instructions
    inst(IType(0x03, 0x0), load(MemType.B)),          // LB
    inst(IType(0x03, 0x1), load(MemType.H)),          // LH
    inst(IType(0x03, 0x2), load(MemType.W)),          // LW
    inst(IType(0x03, 0x4), load(MemType.BU)),         // LBU
    inst(IType(0x03, 0x5), load(MemType.HU)),         // LHU
    
    // Store instructions
    inst(SType(0x23, 0x0), store(MemType.B)),         // SB
    inst(SType(0x23, 0x1), store(MemType.H)),         // SH
    inst(SType(0x23, 0x2), store(MemType.W)),         // SW
    
    // Branch instructions
    inst(BType(0x63, 0x0), branch(BranchCond.EQ)),    // BEQ
    inst(BType(0x63, 0x1), branch(BranchCond.NE)),    // BNE
    inst(BType(0x63, 0x4), branch(BranchCond.LT)),    // BLT
    inst(BType(0x63, 0x5), branch(BranchCond.GE)),    // BGE
    inst(BType(0x63, 0x6), branch(BranchCond.LTU)),   // BLTU
    inst(BType(0x63, 0x7), branch(BranchCond.GEU)),   // BGEU
    
    // Jump instructions
    inst(JType(0x6F), jal),                           // JAL
    inst(IType(0x67, 0x0), jalr),                     // JALR
    
    // Upper immediate instructions  
    inst(UType(0x37), aluImm(AluOp.COPY_B, ImmSel.U)),  // LUI
    inst(UType(0x17), ControlSignalsData().withOpASel(OpASel.PC).withOpBSel(OpBSel.IMM)
      .withAluOp(AluOp.ADD).withRegWen().withImmSel(ImmSel.U).withLegal),  // AUIPC
    
    // System instructions
    inst(SystemType(0x73, 0x0, 0x000), system.withEcall),    // ECALL
    inst(SystemType(0x73, 0x0, 0x001), system.withEbreak),   // EBREAK
    inst(SystemType(0x73, 0x0, 0x302), system.withMret),     // MRET
    
    // CSR instructions
    inst(IType(0x73, 0x1), csr(CSROp.RW)),            // CSRRW
    inst(IType(0x73, 0x2), csr(CSROp.RS)),            // CSRRS
    inst(IType(0x73, 0x3), csr(CSROp.RC)),            // CSRRC
    inst(IType(0x73, 0x5), csr(CSROp.RW)),            // CSRRWI
    inst(IType(0x73, 0x6), csr(CSROp.RS)),            // CSRRSI
    inst(IType(0x73, 0x7), csr(CSROp.RC))             // CSRRCI
  )
}