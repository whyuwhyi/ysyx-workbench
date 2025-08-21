package common

import chisel3._

object PCSel extends ChiselEnum { val PC_4, PC_BR, PC_JMP, PC_JALR = Value }
object OpASel extends ChiselEnum { val RS1, PC, X = Value }
object OpBSel extends ChiselEnum { val RS2, IMM, X = Value }
object AluOp extends ChiselEnum {
  val ADD, SUB, AND, OR, XOR, SLL, SRL, SRA, SLT, SLTU, COPY_B = Value
}
object WBSel extends ChiselEnum { val ALU, MEM, PC4, CSR = Value }
object MemType extends ChiselEnum { val X, B, H, W, BU, HU = Value }
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

object ControlSignalsBundle {
  def fromList(signals: List[UInt]): ControlSignalsBundle = {
    val bundle = Wire(new ControlSignalsBundle)
    bundle.pcSel := signals(0).asTypeOf(PCSel())
    bundle.opASel := signals(1).asTypeOf(OpASel())
    bundle.opBSel := signals(2).asTypeOf(OpBSel())
    bundle.aluOp := signals(3).asTypeOf(AluOp())
    bundle.wbSel := signals(4).asTypeOf(WBSel())
    bundle.regWen := signals(5).asBool
    bundle.memWen := signals(6).asBool
    bundle.memRen := signals(7).asBool
    bundle.memType := signals(8).asTypeOf(MemType())
    bundle.csrCmd := signals(9).asTypeOf(CSROp())
    bundle.branchCond := signals(10).asTypeOf(BranchCond())
    bundle.immSel := signals(11).asTypeOf(ImmSel())
    bundle.isEcall := signals(12).asBool
    bundle.isEbreak := signals(13).asBool
    bundle.isMret := signals(14).asBool
    bundle.illegalInst := signals(15).asBool
    bundle
  }
}
