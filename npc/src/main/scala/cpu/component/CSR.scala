package cpu.component

import chisel3._
import chisel3.util._
import common._

class CSR(implicit override val config: CPUConfig) extends Module with Constants {
  val io = IO(new Bundle {
    val trapValid = Input(Bool())
    val csrCmd = Input(CSROp())
    val inWaddr = Input(UInt(12.W))
    val inWdata = Input(UInt(XLEN.W))
    val raddr = Input(UInt(12.W))
    val rdata = Output(UInt(XLEN.W))
    val pc = Input(UInt(XLEN.W))
    val mepc = Output(UInt(XLEN.W))
    val mtvec = Output(UInt(XLEN.W))
  })

  val mstatus = RegInit(0.U(XLEN.W))
  val mepc    = RegInit(0.U(XLEN.W))
  val mcause  = RegInit(0.U(XLEN.W))
  val mtvec   = RegInit(0.U(XLEN.W))
  
  io.mepc  := mepc
  io.mtvec := mtvec

  val csrFile = collection.mutable.LinkedHashMap(
    MSTATUS_ADDR -> mstatus,
    MEPC_ADDR    -> mepc,
    MCAUSE_ADDR  -> mcause,
    MTVEC_ADDR   -> mtvec
  )

  io.rdata := Mux(csrFile.contains(io.raddr), csrFile(io.raddr), 0.U)

  val wen = (io.csrCmd =/= CSROp.X) || io.trapValid
  
  when (wen) {
    val wdata = Mux(io.trapValid, io.inWdata, 
                  MuxLookup(io.csrCmd.asUInt, 0.U, Seq(
                    CSROp.W.asUInt -> io.inWdata,
                    CSROp.S.asUInt -> (io.rdata | io.inWdata),
                    CSROp.C.asUInt -> (io.rdata & ~io.inWdata)
                  )))

    when(csrFile.contains(io.inWaddr)) {
      csrFile(io.inWaddr) := wdata
    }
  }
}
