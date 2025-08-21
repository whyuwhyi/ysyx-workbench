package cpu.lsu

import chisel3._
import chisel3.util._
import common._
import common.BuildFlags

class LSU extends Module with Constants {
  val io = IO(new Bundle {
    val addrIn = Input(UInt(XLEN.W))
    val wdataIn = Input(UInt(XLEN.W))
    val memTypeIn = Input(MemType())
    val ren = Input(Bool())
    val wen = Input(Bool())
    val rdataOut = Output(UInt(XLEN.W))
    val misalign = Output(Bool())
  })

  val byteSel = io.addrIn(1, 0)

  io.misalign := MuxLookup(io.memTypeIn, false.B)(
    Seq(
      MemType.B -> false.B,
      MemType.H -> byteSel(0),
      MemType.W -> byteSel.orR,
      MemType.BU -> false.B,
      MemType.HU -> byteSel(0)
    )
  )

  val wmask = MuxLookup(io.memTypeIn, 0.U(8.W))(
    Seq(
      MemType.B -> (1.U << byteSel),
      MemType.H -> (3.U << (byteSel << 1)),
      MemType.W -> 0xff.U,
      MemType.BU -> (1.U << byteSel),
      MemType.HU -> (3.U << (byteSel << 1))
    )
  )

  val rawRdata = Wire(UInt(XLEN.W))

  if (BuildFlags.sim) {
    val simDMemInst = Module(new sim.SimDMem(xlen = XLEN))
    simDMemInst.io.ren := io.ren
    simDMemInst.io.raddr := io.addrIn
    simDMemInst.io.wen := io.wen
    simDMemInst.io.waddr := io.addrIn
    simDMemInst.io.wdata := io.wdataIn
    simDMemInst.io.wmask := wmask
    rawRdata := simDMemInst.io.rdata
  } else {
    rawRdata := 0.U
  }

  val rBU = Cat(0.U((XLEN - 8).W), rawRdata(7, 0))
  val rHU = Cat(0.U((XLEN - 16).W), rawRdata(15, 0))
  val rBS = Cat(Fill(XLEN - 8, rawRdata(7)), rawRdata(7, 0))
  val rHS = Cat(Fill(XLEN - 16, rawRdata(15)), rawRdata(15, 0))

  io.rdataOut := MuxLookup(io.memTypeIn, rawRdata)(
    Seq(
      MemType.B -> rBS,
      MemType.H -> rHS,
      MemType.W -> rawRdata,
      MemType.BU -> rBU,
      MemType.HU -> rHU
    )
  )
}
