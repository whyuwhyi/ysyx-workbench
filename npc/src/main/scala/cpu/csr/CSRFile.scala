package cpu.csr

import chisel3._
import chisel3.util._
import common._
import common.CSROp

class CSRFile extends Module with Constants {
  val io = IO(new Bundle {
    val isEcall = Input(Bool())
    val isMret = Input(Bool())
    val csrOp = Input(CSROp())
    val addr = Input(UInt(12.W))
    val wdata = Input(UInt(XLEN.W))
    val rdata = Output(UInt(XLEN.W))
    val pc = Input(UInt(XLEN.W))
    val nextPc = Output(UInt(XLEN.W))
  })

  val mstatus = RegInit(0x1800.U(XLEN.W))
  val mepc = RegInit(0.U(XLEN.W))
  val mcause = RegInit(0.U(XLEN.W))
  val mtvec = RegInit(0.U(XLEN.W))

  val _rdata = MuxLookup(io.addr, 0.U(XLEN.W))(
    Seq(
      MSTATUS_ADDR.U(12.W) -> mstatus,
      MEPC_ADDR.U(12.W) -> mepc,
      MCAUSE_ADDR.U(12.W) -> mcause,
      MTVEC_ADDR.U(12.W) -> mtvec
    )
  )
  io.rdata := _rdata

  when(io.csrOp =/= CSROp.NONE) {
    val w = MuxLookup(io.csrOp, 0.U(XLEN.W))(
      Seq(
        CSROp.RW -> io.wdata,
        CSROp.RS -> (_rdata | io.wdata),
        CSROp.RC -> (_rdata & ~io.wdata)
      )
    )
    switch(io.addr) {
      is(MSTATUS_ADDR.U(12.W)) { mstatus := w }
      is(MEPC_ADDR.U(12.W)) { mepc := w }
      is(MCAUSE_ADDR.U(12.W)) { mcause := w }
      is(MTVEC_ADDR.U(12.W)) { mtvec := w }
    }
  }

  when(io.isEcall) {
    mepc := io.pc
    mcause := ECALL_M_CAUSE.U(XLEN.W)
    mstatus := 0x1800.U(XLEN.W)
  }

  when(io.isMret) {
    mstatus := 0x0080.U(XLEN.W)
  }

  io.nextPc := Mux(io.isEcall, mtvec, Mux(io.isMret, mepc, 0.U(XLEN.W)))
}
