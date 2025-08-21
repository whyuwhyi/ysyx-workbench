package cpu.exu

import chisel3._
import chisel3.util._
import common._
import common.BuildFlags

class RegisterFile extends Module with Constants {
  val io = IO(new Bundle {
    val rs1Addr = Input(UInt(log2Ceil(NR_REG).W))
    val rs2Addr = Input(UInt(log2Ceil(NR_REG).W))
    val rdAddr = Input(UInt(log2Ceil(NR_REG).W))
    val wdata = Input(UInt(XLEN.W))
    val wen = Input(Bool())
    val rs1Data = Output(UInt(XLEN.W))
    val rs2Data = Output(UInt(XLEN.W))
  })
  val regs = RegInit(VecInit(Seq.fill(NR_REG)(0.U(XLEN.W))))
  val rs1 = Mux(io.rs1Addr === 0.U, 0.U, regs(io.rs1Addr))
  val rs2 = Mux(io.rs2Addr === 0.U, 0.U, regs(io.rs2Addr))
  io.rs1Data := rs1
  io.rs2Data := rs2
  when(io.wen && (io.rdAddr =/= 0.U)) { regs(io.rdAddr) := io.wdata }
  if (BuildFlags.sim) {
    val simProbeRFInst = Module(new sim.SimProbeRF(xlen = XLEN, nrReg = NR_REG))
    simProbeRFInst.io.gprs := regs.asUInt
  }
}
