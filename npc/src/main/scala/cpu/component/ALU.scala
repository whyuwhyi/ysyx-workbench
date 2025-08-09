package cpu.component

import chisel3._
import chisel3.util._
import common._

class ALU(implicit override val config: CPUConfig) extends Module with Constants {
  val io = IO(new Bundle {
    val opA = Input(UInt(XLEN.W))
    val opB = Input(UInt(XLEN.W))
    val aluOp = Input(AluOp())
    val out = Output(UInt(XLEN.W))
    val zero = Output(Bool())
  })

  val shamt = io.opB(4, 0)
  
  io.out := MuxLookup(io.aluOp.asUInt, 0.U, Seq(
    AluOp.ADD.asUInt    -> (io.opA + io.opB),
    AluOp.SUB.asUInt    -> (io.opA - io.opB),
    AluOp.AND.asUInt    -> (io.opA & io.opB),
    AluOp.OR.asUInt     -> (io.opA | io.opB),
    AluOp.XOR.asUInt    -> (io.opA ^ io.opB),
    AluOp.SLL.asUInt    -> (io.opA << shamt),
    AluOp.SRL.asUInt    -> (io.opA >> shamt),
    AluOp.SRA.asUInt    -> (io.opA.asSInt >> shamt).asUInt,
    AluOp.SLT.asUInt    -> (io.opA.asSInt < io.opB.asSInt).asUInt,
    AluOp.SLTU.asUInt   -> (io.opA < io.opB).asUInt,
    AluOp.COPY_B.asUInt -> io.opB
  ))

  io.zero := (io.opA === io.opB)
}
