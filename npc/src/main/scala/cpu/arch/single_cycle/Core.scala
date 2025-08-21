package cpu.arch.single_cycle

import chisel3._
import chisel3.util._
import common._
import cpu.idu._
import cpu.csr._

class Core extends Module with Constants {
  val io = IO(new Bundle {})

  val datapathInst = Module(new Datapath)
  val iduInst = Module(new IDU)
  val csrFileInst = Module(new CSRFile)

  iduInst.io.inst := datapathInst.io.inst

  datapathInst.io.pcSel := iduInst.io.pcSel
  datapathInst.io.opASel := iduInst.io.opASel
  datapathInst.io.opBSel := iduInst.io.opBSel
  datapathInst.io.aluOp := iduInst.io.aluOp
  datapathInst.io.wbSel := iduInst.io.wbSel
  datapathInst.io.regWen := iduInst.io.regWen && !csrFileInst.io.takeTrap
  datapathInst.io.memWen := iduInst.io.memWen
  datapathInst.io.memRen := iduInst.io.memRen
  datapathInst.io.memType := iduInst.io.memType
  datapathInst.io.branchCond := iduInst.io.branchCond
  datapathInst.io.immSel := iduInst.io.immSel

  csrFileInst.io.trapValid := iduInst.io.illegalInst || iduInst.io.isEbreak || iduInst.io.isEcall
  csrFileInst.io.isEcall := iduInst.io.isEcall
  csrFileInst.io.isEbreak := iduInst.io.isEbreak
  csrFileInst.io.isMret := iduInst.io.isMret
  csrFileInst.io.csrCmd := iduInst.io.csrCmd

  csrFileInst.io.raddr := datapathInst.io.inst(31, 20)
  csrFileInst.io.inWaddr := datapathInst.io.inst(31, 20)
  val csrZimm = Cat(0.U((XLEN - 5).W), datapathInst.io.inst(19, 15))
  val csrUseImm = datapathInst.io.inst(14)
  csrFileInst.io.inWdata := Mux(csrUseImm, csrZimm, datapathInst.io.rs1Data)
  csrFileInst.io.pc := datapathInst.io.pc

  datapathInst.io.redirValid := csrFileInst.io.takeTrap
  datapathInst.io.redirPc := csrFileInst.io.nextPc
  datapathInst.io.csrRdata := csrFileInst.io.rdata
}
