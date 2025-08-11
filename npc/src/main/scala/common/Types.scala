package common

import chisel3._
import chisel3.util._

object PCSel extends ChiselEnum {
  val PC_4, PC_BR, PC_JMP, PC_JALR, PC_TRAP, PC_MRET = Value
}
object OpASel extends ChiselEnum { val RS1, PC, X = Value }
object OpBSel extends ChiselEnum { val RS2, IMM, X = Value }
object AluOp extends ChiselEnum {
  val ADD, SUB, AND, OR, XOR, SLL, SRL, SRA, SLT, SLTU, COPY_B, X = Value
}
object WBSel extends ChiselEnum { val ALU, MEM, PC4, CSR, X = Value }
object MemType extends ChiselEnum { val X, B, H, W, BU, HU = Value }
object CSROp extends ChiselEnum { val X, W, S, C = Value }
