package cpu.component

import chisel3._
import chisel3.util._
import common._

class ALU(implicit override val config: CPUConfig)
    extends chisel3.Module
    with Constants {
  val io = IO(new chisel3.Bundle {
    val opA = Input(UInt(config.xlen.W))
    val opB = Input(UInt(config.xlen.W))
    val aluOp = Input(AluOp())
    val out = Output(UInt(config.xlen.W))
    val zero = Output(Bool())
  })
  val shamt = io.opB(4, 0)
  val addRes = (io.opA +& io.opB)(config.xlen - 1, 0)
  val subRes = (io.opA +& (~io.opB + 1.U))(config.xlen - 1, 0)
  val sllRes = (io.opA << shamt)(config.xlen - 1, 0)
  val srlRes = (io.opA >> shamt)(config.xlen - 1, 0)
  val sraRes = (io.opA.asSInt >> shamt).asUInt
  val sltRes = (io.opA.asSInt < io.opB.asSInt).asUInt
  val sltuRes = (io.opA < io.opB).asUInt
  val copyB = io.opB
  val default = 0.U(config.xlen.W)
  val table = Seq(
    AluOp.ADD.asUInt -> addRes,
    AluOp.SUB.asUInt -> subRes,
    AluOp.AND.asUInt -> (io.opA & io.opB),
    AluOp.OR.asUInt -> (io.opA | io.opB),
    AluOp.XOR.asUInt -> (io.opA ^ io.opB),
    AluOp.SLL.asUInt -> sllRes,
    AluOp.SRL.asUInt -> srlRes,
    AluOp.SRA.asUInt -> sraRes,
    AluOp.SLT.asUInt -> sltRes,
    AluOp.SLTU.asUInt -> sltuRes,
    AluOp.COPY_B.asUInt -> copyB
  )
  io.out := MuxLookup(io.aluOp.asUInt, default)(table)
  io.zero := io.out === 0.U
}
