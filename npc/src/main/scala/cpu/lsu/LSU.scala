package cpu.lsu

import chisel3._
import chisel3.util._
import common._

class LSU(implicit override val config: CPUConfig)
    extends chisel3.Module
    with Constants {
  val io = IO(new chisel3.Bundle {
    val addrIn = Input(UInt(config.xlen.W))
    val wdataIn = Input(UInt(config.xlen.W))
    val memTypeIn = Input(MemType())
    val ren = Input(Bool())
    val wen = Input(Bool())
    val rdataMem = Input(UInt(config.xlen.W))
    val addrOut = Output(UInt(config.xlen.W))
    val wdataOut = Output(UInt(config.xlen.W))
    val memTypeOut = Output(MemType())
    val wenOut = Output(Bool())
    val rdataOut = Output(UInt(config.xlen.W))
    val misalign = Output(Bool())
  })
  val addr = io.addrIn
  val mtU = io.memTypeIn.asUInt
  val hMis = addr(0) =/= 0.U
  val wMis = addr(1, 0) =/= 0.U
  val mis = MuxLookup(mtU, false.B)(
    Seq(
      MemType.B.asUInt -> false.B,
      MemType.BU.asUInt -> false.B,
      MemType.H.asUInt -> hMis,
      MemType.HU.asUInt -> hMis,
      MemType.W.asUInt -> wMis
    )
  )
  val byte = io.rdataMem(7, 0)
  val half = io.rdataMem(15, 0)
  val rB = Cat(Fill(config.xlen - 8, byte(7)), byte)
  val rBU = Cat(0.U((config.xlen - 8).W), byte)
  val rH = Cat(Fill(config.xlen - 16, half(15)), half)
  val rHU = Cat(0.U((config.xlen - 16).W), half)
  val rW = io.rdataMem
  val rSel = MuxLookup(mtU, rW)(
    Seq(
      MemType.B.asUInt -> rB,
      MemType.BU.asUInt -> rBU,
      MemType.H.asUInt -> rH,
      MemType.HU.asUInt -> rHU,
      MemType.W.asUInt -> rW
    )
  )
  val wMaskB = "h000000ff".U(config.xlen.W)
  val wMaskH = "h0000ffff".U(config.xlen.W)
  val wMaskW = "hffffffff".U(config.xlen.W)
  val wMask = MuxLookup(mtU, wMaskW)(
    Seq(
      MemType.B.asUInt -> wMaskB,
      MemType.H.asUInt -> wMaskH,
      MemType.W.asUInt -> wMaskW
    )
  )
  io.addrOut := addr
  io.memTypeOut := io.memTypeIn
  io.wdataOut := io.wdataIn & wMask
  io.wenOut := io.wen
  io.rdataOut := rSel
  io.misalign := mis
}
