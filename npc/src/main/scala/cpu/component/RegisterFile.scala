package cpu.component

import chisel3._
import chisel3.util._
import common._

class RegisterFile(implicit val config: CPUConfig) extends chisel3.Module {
  val io = IO(new chisel3.Bundle {
    val rs1Addr = Input(UInt(5.W))
    val rs2Addr = Input(UInt(5.W))
    val rdAddr = Input(UInt(5.W))
    val wdata = Input(UInt(config.xlen.W))
    val wen = Input(Bool())
    val rs1Data = Output(UInt(config.xlen.W))
    val rs2Data = Output(UInt(config.xlen.W))
  })
  val regs = RegInit(VecInit(Seq.fill(config.nrRegs)(0.U(config.xlen.W))))
  val wEnVec = VecInit(
    Seq.tabulate(config.nrRegs)(i =>
      io.wen && io.rdAddr === i.U && (i.U =/= 0.U)
    )
  )
  val nextVec = VecInit(
    Seq.tabulate(config.nrRegs)(i => Mux(wEnVec(i), io.wdata, regs(i)))
  )
  regs := nextVec
  val idxW = log2Ceil(config.nrRegs)
  val rs1IdxWide = Mux(io.rs1Addr < config.nrRegs.U, io.rs1Addr, 0.U)
  val rs2IdxWide = Mux(io.rs2Addr < config.nrRegs.U, io.rs2Addr, 0.U)
  val rs1Idx = rs1IdxWide(idxW - 1, 0)
  val rs2Idx = rs2IdxWide(idxW - 1, 0)
  val rs1 = Mux(io.rs1Addr === 0.U, 0.U, regs(rs1Idx))
  val rs2 = Mux(io.rs2Addr === 0.U, 0.U, regs(rs2Idx))
  io.rs1Data := rs1
  io.rs2Data := rs2
}
