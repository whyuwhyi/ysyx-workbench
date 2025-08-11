package cpu.csr

import chisel3._
import chisel3.util._
import common._

class CSRFile(implicit override val config: CPUConfig)
    extends chisel3.Module
    with Constants {
  val io = IO(new chisel3.Bundle {
    val trapValid = Input(Bool())
    val isEcall = Input(Bool())
    val isEbreak = Input(Bool())
    val isMret = Input(Bool())
    val csrCmd = Input(CSROp())
    val raddr = Input(UInt(12.W))
    val inWaddr = Input(UInt(12.W))
    val inWdata = Input(UInt(config.xlen.W))
    val rdata = Output(UInt(config.xlen.W))
    val pc = Input(UInt(config.xlen.W))
    val nextPc = Output(UInt(config.xlen.W))
    val takeTrap = Output(Bool())
  })
  val mstatus = RegInit(0.U(config.xlen.W))
  val mepc = RegInit(0.U(config.xlen.W))
  val mcause = RegInit(0.U(config.xlen.W))
  val mtvec = RegInit(0.U(config.xlen.W))
  val rSelMstatus = io.raddr === MSTATUS_ADDR
  val rSelMepc = io.raddr === MEPC_ADDR
  val rSelMcause = io.raddr === MCAUSE_ADDR
  val rSelMtvec = io.raddr === MTVEC_ADDR
  val rVals = Seq(mstatus, mepc, mcause, mtvec)
  val rHits = Seq(rSelMstatus, rSelMepc, rSelMcause, rSelMtvec)
  val rFold =
    rHits.zip(rVals).map { case (h, v) => Mux(h, v, 0.U(config.xlen.W)) }
  io.rdata := rFold.reduce(_ | _)
  val csrKey = io.csrCmd.asUInt
  val csrW = io.inWdata
  val csrS = io.rdata | io.inWdata
  val csrC = io.rdata & (~io.inWdata)
  val csrWriteData = MuxLookup(csrKey, 0.U(config.xlen.W))(
    Seq(
      CSROp.W.asUInt -> csrW,
      CSROp.S.asUInt -> csrS,
      CSROp.C.asUInt -> csrC
    )
  )
  val trapTaken = io.trapValid || io.isEbreak || io.isEcall
  val wrAddr = Mux(trapTaken, MCAUSE_ADDR, io.inWaddr)
  val wrData = Mux(
    trapTaken,
    Mux(io.isEcall, ECALL_M_CAUSE, ILLEGAL_INST_CAUSE),
    csrWriteData
  )
  val wrEn = trapTaken || (io.csrCmd =/= CSROp.X)
  val wSelMstatus = wrEn && wrAddr === MSTATUS_ADDR
  val wSelMepc = wrEn && wrAddr === MEPC_ADDR
  val wSelMcause = wrEn && wrAddr === MCAUSE_ADDR
  val wSelMtvec = wrEn && wrAddr === MTVEC_ADDR
  val mstatusN = Mux(wSelMstatus, wrData, mstatus)
  val mepcN = Mux(wSelMepc, io.pc, mepc)
  val mcauseN = Mux(wSelMcause, wrData, mcause)
  val mtvecN = Mux(wSelMtvec, wrData, mtvec)
  mstatus := mstatusN
  mepc := mepcN
  mcause := mcauseN
  mtvec := mtvecN
  val mretTaken = io.isMret
  val pc4 = io.pc + 4.U
  val nextTrap = mtvec
  val nextMret = mepc
  val nextPcSel = Cat(mretTaken, trapTaken)
  io.nextPc := MuxLookup(nextPcSel, pc4)(
    Seq("b01".U -> nextTrap, "b10".U -> nextMret)
  )
  io.takeTrap := trapTaken || mretTaken
}
