package cpu.lsu

import chisel3._
import chisel3.util._
import common._
import common.BuildFlags

class LSU extends Module with Constants {
  val io = IO(new Bundle {
    val addr = Input(UInt(XLEN.W))
    val wdata = Input(UInt(XLEN.W))
    val memType = Input(MemType())
    val ren = Input(Bool())
    val wen = Input(Bool())
    val rdata = Output(UInt(XLEN.W))
    val misalign = Output(Bool())
  })

  val byteSel = io.addr(1, 0)

  io.misalign := MuxLookup(io.memType, false.B)(
    Seq(
      MemType.B -> false.B,
      MemType.H -> byteSel(0),
      MemType.W -> byteSel.orR,
      MemType.BU -> false.B,
      MemType.HU -> byteSel(0)
    )
  )

  val wmask = MuxLookup(io.memType, 4.U(8.W))(
    Seq(
      MemType.B -> 1.U,
      MemType.H -> 3.U,
      MemType.W -> 15.U,
      MemType.BU -> 1.U,
      MemType.HU -> 3.U
    )
  )

  val rawRdata = Wire(UInt(XLEN.W))

  if (BuildFlags.sim) {
    val simDMemInst = Module(new sim.SimDMem(xlen = XLEN))
    simDMemInst.io.addr := io.addr
    simDMemInst.io.ren := io.ren
    rawRdata := simDMemInst.io.rdata
    simDMemInst.io.wen := io.wen
    simDMemInst.io.wdata := io.wdata
    simDMemInst.io.wmask := wmask
  }

  val rBU = Cat(0.U((XLEN - 8).W), rawRdata(7, 0))
  val rHU = Cat(0.U((XLEN - 16).W), rawRdata(15, 0))
  val rBS = Cat(Fill(XLEN - 8, rawRdata(7)), rawRdata(7, 0))
  val rHS = Cat(Fill(XLEN - 16, rawRdata(15)), rawRdata(15, 0))

  io.rdata := MuxLookup(io.memType, rawRdata)(
    Seq(
      MemType.B -> rBS,
      MemType.H -> rHS,
      MemType.W -> rawRdata,
      MemType.BU -> rBU,
      MemType.HU -> rHU
    )
  )
}
