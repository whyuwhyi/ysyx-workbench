package common

import chisel3._

object PCSel extends ChiselEnum {
  val NONE, PC_4, PC_BR, PC_JAL, PC_JALR = Value
}
object OpASel extends ChiselEnum { val NONE, RS1, PC = Value }
object OpBSel extends ChiselEnum { val NONE, RS2, IMM = Value }
object AluOp extends ChiselEnum {
  val NONE, ADD, SUB, AND, OR, XOR, SLL, SRL, SRA, SLT, SLTU, COPY_B = Value
}
object WBSel extends ChiselEnum { val NONE, ALU, MEM, PC4, CSR = Value }
object MemType extends ChiselEnum { val NONE, B, H, W, BU, HU = Value }
object CSROp extends ChiselEnum { val NONE, RW, RS, RC = Value }
object BranchCond extends ChiselEnum {
  val NONE, EQ, NE, LT, GE, LTU, GEU = Value
}
object ImmSel extends ChiselEnum { val NONE, I, S, B, U, J = Value }

class BranchFlags extends Bundle with Constants {
  val isZero = Bool()
  val isLessSigned = Bool()
  val isLessUnsigned = Bool()
}

class ControlSignalsBundle extends Bundle {
  val pcSel = PCSel()
  val opASel = OpASel()
  val opBSel = OpBSel()
  val aluOp = AluOp()
  val wbSel = WBSel()
  val regWen = Bool()
  val memWen = Bool()
  val memRen = Bool()
  val memType = MemType()
  val csrCmd = CSROp()
  val branchCond = BranchCond()
  val immSel = ImmSel()
  val isEcall = Bool()
  val isEbreak = Bool()
  val isMret = Bool()
  val illegalInst = Bool()
}
