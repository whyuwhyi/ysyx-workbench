package cpu.exu

import chisel3._
import chisel3.util._
import common._
import common.AluOp

class ALU extends Module with Constants {
  val io = IO(new Bundle {
    val opA = Input(UInt(XLEN.W))
    val opB = Input(UInt(XLEN.W))
    val aluOp = Input(AluOp())
    val out = Output(UInt(XLEN.W))
    val zero = Output(Bool())
    val branchFlags = Output(new BranchFlags)
  })

  val subLike =
    io.aluOp.asUInt === AluOp.SUB.asUInt || io.aluOp.asUInt === AluOp.SLT.asUInt || io.aluOp.asUInt === AluOp.SLTU.asUInt
  val bXor = Mux(subLike, ~io.opB, io.opB)
  val cin = Mux(subLike, 1.U, 0.U)
  val wide = io.opA +& bXor + cin
  val sum = wide(XLEN - 1, 0)
  val carryOut = wide(XLEN)

  val aSign = io.opA(XLEN - 1)
  val bSign = io.opB(XLEN - 1)
  val sumSign = sum(XLEN - 1)
  val sltSigned = Mux(aSign ^ bSign, aSign, sumSign)
  val sltUnsigned = ~carryOut

  val shamt = io.opB(log2Ceil(XLEN) - 1, 0)
  val srl = io.opA >> shamt
  val sra = (io.opA.asSInt >> shamt).asUInt
  val sll = Reverse(Reverse(io.opA) >> shamt)

  val andRes = io.opA & io.opB
  val orRes = io.opA | io.opB
  val xorRes = io.opA ^ io.opB
  val sltRes = Mux(sltSigned, 1.U, 0.U)
  val sltuRes = Mux(sltUnsigned, 1.U, 0.U)

  io.out := MuxLookup(io.aluOp, sum)(
    Seq(
      AluOp.ADD -> sum,
      AluOp.SUB -> sum,
      AluOp.AND -> andRes,
      AluOp.OR -> orRes,
      AluOp.XOR -> xorRes,
      AluOp.SLL -> sll,
      AluOp.SRL -> srl,
      AluOp.SRA -> sra,
      AluOp.SLT -> sltRes,
      AluOp.SLTU -> sltuRes,
      AluOp.COPY_B -> io.opB
    )
  )
  io.zero := io.out === 0.U
  io.branchFlags.isZero := sum === 0.U
  io.branchFlags.isLessSigned := sltSigned
  io.branchFlags.isLessUnsigned := sltUnsigned
}
