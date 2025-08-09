package cpu.component

import chisel3._
import common._

class RegisterFile(implicit override val config: CPUConfig) extends Module with Constants {
  val io = IO(new Bundle {
    val rs1Addr = Input(UInt(5.W))
    val rs2Addr = Input(UInt(5.W))
    val rdAddr = Input(UInt(5.W))
    val wdata = Input(UInt(XLEN.W))
    val wen = Input(Bool())
    val rs1Data = Output(UInt(XLEN.W))
    val rs2Data = Output(UInt(XLEN.W))
  })

  val regs = RegInit(VecInit(Seq.fill(NR_REGS)(0.U(XLEN.W))))

  val rs1ReadAddr = if (NR_REGS == 16) io.rs1Addr(3,0) else io.rs1Addr
  val rs2ReadAddr = if (NR_REGS == 16) io.rs2Addr(3,0) else io.rs2Addr
  val rdWriteAddr = if (NR_REGS == 16) io.rdAddr(3,0) else io.rdAddr

  io.rs1Data := Mux(io.rs1Addr === 0.U, 0.U, regs(rs1ReadAddr))
  io.rs2Data := Mux(io.rs2Addr === 0.U, 0.U, regs(rs2ReadAddr))

  when(io.wen && io.rdAddr =/= 0.U) {
    regs(rdWriteAddr) := io.wdata
  }
}
