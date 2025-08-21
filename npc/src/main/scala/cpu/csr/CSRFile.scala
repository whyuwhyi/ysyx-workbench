package cpu.csr

import chisel3._
import chisel3.util._
import common._
import common.CSROp

class CSRFile extends Module with Constants {
  val io = IO(new Bundle {
    val trapValid = Input(Bool())
    val isEcall = Input(Bool())
    val isEbreak = Input(Bool())
    val isMret = Input(Bool())
    val csrCmd = Input(CSROp())
    val raddr = Input(UInt(12.W))
    val inWaddr = Input(UInt(12.W))
    val inWdata = Input(UInt(XLEN.W))
    val rdata = Output(UInt(XLEN.W))
    val pc = Input(UInt(XLEN.W))
    val nextPc = Output(UInt(XLEN.W))
    val takeTrap = Output(Bool())
  })

  val mstatus = RegInit(0.U(XLEN.W))
  val mepc = RegInit(0.U(XLEN.W))
  val mcause = RegInit(0.U(XLEN.W))
  val mtvec = RegInit(0.U(XLEN.W))

  val rdata = MuxLookup(io.raddr, 0.U(XLEN.W))(
    Seq(
      MSTATUS_ADDR.U(12.W) -> mstatus,
      MEPC_ADDR.U(12.W) -> mepc,
      MCAUSE_ADDR.U(12.W) -> mcause,
      MTVEC_ADDR.U(12.W) -> mtvec
    )
  )
  io.rdata := rdata

  when(io.csrCmd =/= CSROp.NONE) {
    val w = MuxLookup(io.csrCmd.asUInt, 0.U(XLEN.W))(
      Seq(
        CSROp.RW.asUInt -> io.inWdata,
        CSROp.RS.asUInt -> (rdata | io.inWdata),
        CSROp.RC.asUInt -> (rdata & ~io.inWdata)
      )
    )
    switch(io.inWaddr) {
      is(MSTATUS_ADDR.U(12.W)) { mstatus := w }
      is(MEPC_ADDR.U(12.W)) { mepc := w }
      is(MCAUSE_ADDR.U(12.W)) { mcause := w }
      is(MTVEC_ADDR.U(12.W)) { mtvec := w }
    }
  }

  val extTrap = io.trapValid || io.isEbreak || io.isEcall
  val causeVal =
    Mux(io.isEcall, ECALL_M_CAUSE.U(XLEN.W), ILLEGAL_INST_CAUSE.U(XLEN.W))
  when(extTrap) {
    mepc := io.pc
    mcause := causeVal
  }
  io.nextPc := Mux(io.isMret, mepc, mtvec)
  io.takeTrap := extTrap || io.isMret
}
