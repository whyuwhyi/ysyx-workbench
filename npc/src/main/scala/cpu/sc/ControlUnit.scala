package cpu.sc

import chisel3._
import chisel3.util._
import common._
import common.Arch

class ControlIO(implicit override val config: CPUConfig) extends BaseCoreIO {
  val inst = Input(UInt(config.xlen.W))
  val pcSel = Output(PCSel())
  val opASel = Output(OpASel())
  val opBSel = Output(OpBSel())
  val aluOp = Output(AluOp())
  val regWen = Output(Bool())
  val wbSel = Output(WBSel())
  val memWen = Output(Bool())
  val memRen = Output(Bool())
  val memType = Output(MemType())
  val csrCmd = Output(CSROp())
  val illegalInst = Output(Bool())
  val isEcall = Output(Bool())
  val isEbreak = Output(Bool())
  val isMret = Output(Bool())
}

class ControlUnit(implicit override val config: CPUConfig)
    extends chisel3.Module
    with Arch {
  val io = IO(new ControlIO)
  val opcode = io.inst(6, 0)
  val funct3 = io.inst(14, 12)
  val funct7 = io.inst(31, 25)
  val isOpImm = opcode === Opcodes.OP_IMM
  val isOp = opcode === Opcodes.OP
  val isLoad = opcode === Opcodes.LOAD
  val isStore = opcode === Opcodes.STORE
  val isBranch = opcode === Opcodes.BRANCH
  val isJal = opcode === Opcodes.JAL
  val isJalr = opcode === Opcodes.JALR
  val isLui = opcode === Opcodes.LUI
  val isAuipc = opcode === Opcodes.AUIPC
  val isSys = opcode === Opcodes.SYSTEM
  val isCsrRW = isSys && (funct3 === Funct3.CSRRW || funct3 === Funct3.CSRRWI)
  val isCsrRS = isSys && (funct3 === Funct3.CSRRS || funct3 === Funct3.CSRRSI)
  val isCsrRC = isSys && (funct3 === Funct3.CSRRC || funct3 === Funct3.CSRRCI)
  val isEcall = isSys && funct3 === Funct3.PRIV && io.inst(31, 20) === 0.U
  val isEbreak = isSys && funct3 === Funct3.PRIV && io.inst(31, 20) === 1.U
  val isMret = isSys && funct3 === Funct3.PRIV && io.inst(31, 20) === "h302".U
  val csrCmdU = Mux(
    isCsrRW,
    CSROp.W.asUInt,
    Mux(isCsrRS, CSROp.S.asUInt, Mux(isCsrRC, CSROp.C.asUInt, CSROp.X.asUInt))
  )
  val pcSelU = Mux(
    isJal,
    PCSel.PC_JMP.asUInt,
    Mux(
      isJalr,
      PCSel.PC_JALR.asUInt,
      Mux(isBranch, PCSel.PC_BR.asUInt, PCSel.PC_4.asUInt)
    )
  )
  val opASelU = Mux(
    isAuipc || isJal || isJalr,
    OpASel.PC.asUInt,
    Mux(isBranch, OpASel.RS1.asUInt, OpASel.RS1.asUInt)
  )
  val opBSelU = Mux(
    isOp || isBranch,
    OpBSel.RS2.asUInt,
    Mux(
      isOpImm || isLoad || isStore || isLui || isAuipc,
      OpBSel.IMM.asUInt,
      OpBSel.X.asUInt
    )
  )
  val aluKey = Cat(funct7, funct3)
  val aluSel = Mux(
    isLui,
    AluOp.COPY_B.asUInt,
    Mux(
      isBranch,
      MuxLookup(funct3, AluOp.SUB.asUInt)(
        Seq(
          "b000".U -> AluOp.SUB.asUInt,
          "b001".U -> AluOp.SUB.asUInt,
          "b100".U -> AluOp.SLT.asUInt,
          "b101".U -> AluOp.SLT.asUInt,
          "b110".U -> AluOp.SLTU.asUInt,
          "b111".U -> AluOp.SLTU.asUInt
        )
      ),
      Mux(
        isOp || isOpImm,
        MuxLookup(aluKey, AluOp.ADD.asUInt)(
          Seq(
            Cat("b0000000".U, "b000".U) -> Mux(
              isOpImm,
              AluOp.ADD.asUInt,
              Mux(io.inst(30), AluOp.SUB.asUInt, AluOp.ADD.asUInt)
            ),
            Cat("b0000000".U, "b001".U) -> AluOp.SLL.asUInt,
            Cat("b0000000".U, "b010".U) -> AluOp.SLT.asUInt,
            Cat("b0000000".U, "b011".U) -> AluOp.SLTU.asUInt,
            Cat("b0000000".U, "b100".U) -> AluOp.XOR.asUInt,
            Cat("b0000000".U, "b101".U) -> AluOp.SRL.asUInt,
            Cat("b0100000".U, "b101".U) -> AluOp.SRA.asUInt,
            Cat("b0000000".U, "b110".U) -> AluOp.OR.asUInt,
            Cat("b0000000".U, "b111".U) -> AluOp.AND.asUInt
          )
        ),
        AluOp.ADD.asUInt
      )
    )
  )
  val regWenU =
    isLui || isAuipc || isOpImm || isOp || isLoad || isJal || isJalr || isCsrRW || isCsrRS || isCsrRC
  val wbSelU = Mux(
    isLoad,
    WBSel.MEM.asUInt,
    Mux(
      isJal || isJalr,
      WBSel.PC4.asUInt,
      Mux(isCsrRW || isCsrRS || isCsrRC, WBSel.CSR.asUInt, WBSel.ALU.asUInt)
    )
  )
  val memWenU = isStore
  val memRenU = isLoad
  val memTypeU = Mux(
    isLoad || isStore,
    MuxLookup(funct3, MemType.W.asUInt)(
      Seq(
        "b000".U -> MemType.B.asUInt,
        "b001".U -> MemType.H.asUInt,
        "b010".U -> MemType.W.asUInt,
        "b100".U -> MemType.BU.asUInt,
        "b101".U -> MemType.HU.asUInt,
        "b000".U -> MemType.B.asUInt,
        "b001".U -> MemType.H.asUInt,
        "b010".U -> MemType.W.asUInt
      )
    ),
    MemType.X.asUInt
  )
  val illegal =
    !(isOpImm || isOp || isLoad || isStore || isBranch || isJal || isJalr || isLui || isAuipc || isSys)
  val pcSelT = PCSel.safe(pcSelU)._1
  val opASelT = OpASel.safe(opASelU)._1
  val opBSelT = OpBSel.safe(opBSelU)._1
  val aluOpT = AluOp.safe(aluSel)._1
  val wbSelT = WBSel.safe(wbSelU)._1
  val memTypeT = MemType.safe(memTypeU)._1
  val csrCmdT = CSROp.safe(csrCmdU)._1
  io.pcSel := pcSelT
  io.opASel := opASelT
  io.opBSel := opBSelT
  io.aluOp := aluOpT
  io.regWen := regWenU
  io.wbSel := wbSelT
  io.memWen := memWenU
  io.memRen := memRenU
  io.memType := memTypeT
  io.csrCmd := csrCmdT
  io.illegalInst := illegal
  io.isEcall := isEcall
  io.isEbreak := isEbreak
  io.isMret := isMret
}
