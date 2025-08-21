package cpu.top

import chisel3._
import cpu.arch.single_cycle.Core

object Gen extends App {
  emitVerilog(
    new Core,
    Array(
      "--target-dir",
      "rtl"
    )
  )
}
