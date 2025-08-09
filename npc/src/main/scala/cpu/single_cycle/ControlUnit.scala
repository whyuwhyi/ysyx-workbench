package cpu.single_cycle

import chisel3._
import chisel3.util._
import common._

class ControlIO(implicit override val config: CPUConfig) extends BaseCoreIO {
  val inst = Input(UInt(XLEN.W))
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

class ControlUnit(implicit override val config: CPUConfig) extends Module with Constants {
  val io = IO(new ControlIO)

  val defaultSignals = List(PCSel.PC_4, OpASel.X, OpBSel.X, AluOp.X, false.B, WBSel.X, false.B, false.B, MemType.X, CSROp.X, true.B, false.B, false.B, false.B) 
  
  val instructionMap = Array(
    LUI    -> List(PCSel.PC_4, OpASel.PC,  OpBSel.IMM, AluOp.COPY_B, true.B,  WBSel.ALU, false.B, false.B, MemType.X,  CSROp.X, false.B, false.B, false.B, false.B),
    AUIPC  -> List(PCSel.PC_4, OpASel.PC,  OpBSel.IMM, AluOp.ADD,    true.B,  WBSel.ALU, false.B, false.B, MemType.X,  CSROp.X, false.B, false.B, false.B, false.B),
    JAL    -> List(PCSel.PC_JMP,OpASel.PC, OpBSel.IMM, AluOp.ADD,    true.B,  WBSel.PC4, false.B, false.B, MemType.X,  CSROp.X, false.B, false.B, false.B, false.B),
    JALR   -> List(PCSel.PC_JALR,OpASel.RS1,OpBSel.IMM, AluOp.ADD,    true.B,  WBSel.PC4, false.B, false.B, MemType.X,  CSROp.X, false.B, false.B, false.B, false.B),
    BEQ    -> List(PCSel.PC_BR, OpASel.RS1, OpBSel.RS2, AluOp.SUB,    false.B, WBSel.X,   false.B, false.B, MemType.X,  CSROp.X, false.B, false.B, false.B, false.B),
    BNE    -> List(PCSel.PC_BR, OpASel.RS1, OpBSel.RS2, AluOp.SUB,    false.B, WBSel.X,   false.B, false.B, MemType.X,  CSROp.X, false.B, false.B, false.B, false.B),
    BLT    -> List(PCSel.PC_BR, OpASel.RS1, OpBSel.RS2, AluOp.SLT,    false.B, WBSel.X,   false.B, false.B, MemType.X,  CSROp.X, false.B, false.B, false.B, false.B),
    BGE    -> List(PCSel.PC_BR, OpASel.RS1, OpBSel.RS2, AluOp.SLT,    false.B, WBSel.X,   false.B, false.B, MemType.X,  CSROp.X, false.B, false.B, false.B, false.B),
    BLTU   -> List(PCSel.PC_BR, OpASel.RS1, OpBSel.RS2, AluOp.SLTU,   false.B, WBSel.X,   false.B, false.B, MemType.X,  CSROp.X, false.B, false.B, false.B, false.B),
    BGEU   -> List(PCSel.PC_BR, OpASel.RS1, OpBSel.RS2, AluOp.SLTU,   false.B, WBSel.X,   false.B, false.B, MemType.X,  CSROp.X, false.B, false.B, false.B, false.B),
    LB     -> List(PCSel.PC_4, OpASel.RS1, OpBSel.IMM, AluOp.ADD,    true.B,  WBSel.MEM, false.B, true.B,  MemType.B,  CSROp.X, false.B, false.B, false.B, false.B),
    LH     -> List(PCSel.PC_4, OpASel.RS1, OpBSel.IMM, AluOp.ADD,    true.B,  WBSel.MEM, false.B, true.B,  MemType.H,  CSROp.X, false.B, false.B, false.B, false.B),
    LW     -> List(PCSel.PC_4, OpASel.RS1, OpBSel.IMM, AluOp.ADD,    true.B,  WBSel.MEM, false.B, true.B,  MemType.W,  CSROp.X, false.B, false.B, false.B, false.B),
    LBU    -> List(PCSel.PC_4, OpASel.RS1, OpBSel.IMM, AluOp.ADD,    true.B,  WBSel.MEM, false.B, true.B,  MemType.BU, CSROp.X, false.B, false.B, false.B, false.B),
    LHU    -> List(PCSel.PC_4, OpASel.RS1, OpBSel.IMM, AluOp.ADD,    true.B,  WBSel.MEM, false.B, true.B,  MemType.HU, CSROp.X, false.B, false.B, false.B, false.B),
    SB     -> List(PCSel.PC_4, OpASel.RS1, OpBSel.IMM, AluOp.ADD,    false.B, WBSel.X,   true.B,  false.B, MemType.B,  CSROp.X, false.B, false.B, false.B, false.B),
    SH     -> List(PCSel.PC_4, OpASel.RS1, OpBSel.IMM, AluOp.ADD,    false.B, WBSel.X,   true.B,  false.B, MemType.H,  CSROp.X, false.B, false.B, false.B, false.B),
    SW     -> List(PCSel.PC_4, OpASel.RS1, OpBSel.IMM, AluOp.ADD,    false.B, WBSel.X,   true.B,  false.B, MemType.W,  CSROp.X, false.B, false.B, false.B, false.B),
    ADDI   -> List(PCSel.PC_4, OpASel.RS1, OpBSel.IMM, AluOp.ADD,    true.B,  WBSel.ALU, false.B, false.B, MemType.X,  CSROp.X, false.B, false.B, false.B, false.B),
    SLTI   -> List(PCSel.PC_4, OpASel.RS1, OpBSel.IMM, AluOp.SLT,    true.B,  WBSel.ALU, false.B, false.B, MemType.X,  CSROp.X, false.B, false.B, false.B, false.B),
    SLTIU  -> List(PCSel.PC_4, OpASel.RS1, OpBSel.IMM, AluOp.SLTU,   true.B,  WBSel.ALU, false.B, false.B, MemType.X,  CSROp.X, false.B, false.B, false.B, false.B),
    XORI   -> List(PCSel.PC_4, OpASel.RS1, OpBSel.IMM, AluOp.XOR,    true.B,  WBSel.ALU, false.B, false.B, MemType.X,  CSROp.X, false.B, false.B, false.B, false.B),
    ORI    -> List(PCSel.PC_4, OpASel.RS1, OpBSel.IMM, AluOp.OR,     true.B,  WBSel.ALU, false.B, false.B, MemType.X,  CSROp.X, false.B, false.B, false.B, false.B),
    ANDI   -> List(PCSel.PC_4, OpASel.RS1, OpBSel.IMM, AluOp.AND,    true.B,  WBSel.ALU, false.B, false.B, MemType.X,  CSROp.X, false.B, false.B, false.B, false.B),
    SLLI   -> List(PCSel.PC_4, OpASel.RS1, OpBSel.IMM, AluOp.SLL,    true.B,  WBSel.ALU, false.B, false.B, MemType.X,  CSROp.X, false.B, false.B, false.B, false.B),
    SRLI   -> List(PCSel.PC_4, OpASel.RS1, OpBSel.IMM, AluOp.SRL,    true.B,  WBSel.ALU, false.B, false.B, MemType.X,  CSROp.X, false.B, false.B, false.B, false.B),
    SRAI   -> List(PCSel.PC_4, OpASel.RS1, OpBSel.IMM, AluOp.SRA,    true.B,  WBSel.ALU, false.B, false.B, MemType.X,  CSROp.X, false.B, false.B, false.B, false.B),
    ADD    -> List(PCSel.PC_4, OpASel.RS1, OpBSel.RS2, AluOp.ADD,    true.B,  WBSel.ALU, false.B, false.B, MemType.X,  CSROp.X, false.B, false.B, false.B, false.B),
    SUB    -> List(PCSel.PC_4, OpASel.RS1, OpBSel.RS2, AluOp.SUB,    true.B,  WBSel.ALU, false.B, false.B, MemType.X,  CSROp.X, false.B, false.B, false.B, false.B),
    SLL    -> List(PCSel.PC_4, OpASel.RS1, OpBSel.RS2, AluOp.SLL,    true.B,  WBSel.ALU, false.B, false.B, MemType.X,  CSROp.X, false.B, false.B, false.B, false.B),
    SLT    -> List(PCSel.PC_4, OpASel.RS1, OpBSel.RS2, AluOp.SLT,    true.B,  WBSel.ALU, false.B, false.B, MemType.X,  CSROp.X, false.B, false.B, false.B, false.B),
    SLTU   -> List(PCSel.PC_4, OpASel.RS1, OpBSel.RS2, AluOp.SLTU,   true.B,  WBSel.ALU, false.B, false.B, MemType.X,  CSROp.X, false.B, false.B, false.B, false.B),
    XOR    -> List(PCSel.PC_4, OpASel.RS1, OpBSel.RS2, AluOp.XOR,    true.B,  WBSel.ALU, false.B, false.B, MemType.X,  CSROp.X, false.B, false.B, false.B, false.B),
    SRL    -> List(PCSel.PC_4, OpASel.RS1, OpBSel.RS2, AluOp.SRL,    true.B,  WBSel.ALU, false.B, false.B, MemType.X,  CSROp.X, false.B, false.B, false.B, false.B),
    SRA    -> List(PCSel.PC_4, OpASel.RS1, OpBSel.RS2, AluOp.SRA,    true.B,  WBSel.ALU, false.B, false.B, MemType.X,  CSROp.X, false.B, false.B, false.B, false.B),
    OR     -> List(PCSel.PC_4, OpASel.RS1, OpBSel.RS2, AluOp.OR,     true.B,  WBSel.ALU, false.B, false.B, MemType.X,  CSROp.X, false.B, false.B, false.B, false.B),
    AND    -> List(PCSel.PC_4, OpASel.RS1, OpBSel.RS2, AluOp.AND,    true.B,  WBSel.ALU, false.B, false.B, MemType.X,  CSROp.X, false.B, false.B, false.B, false.B),
    CSRRW  -> List(PCSel.PC_4, OpASel.RS1, OpBSel.RS2, AluOp.X,      true.B,  WBSel.CSR, false.B, false.B, MemType.X,  CSROp.W, false.B, false.B, false.B, false.B),
    CSRRS  -> List(PCSel.PC_4, OpASel.RS1, OpBSel.RS2, AluOp.X,      true.B,  WBSel.CSR, false.B, false.B, MemType.X,  CSROp.S, false.B, false.B, false.B, false.B),
    CSRRC  -> List(PCSel.PC_4, OpASel.RS1, OpBSel.RS2, AluOp.X,      true.B,  WBSel.CSR, false.B, false.B, MemType.X,  CSROp.C, false.B, false.B, false.B, false.B),
    CSRRWI -> List(PCSel.PC_4, OpASel.RS1, OpBSel.IMM, AluOp.X,      true.B,  WBSel.CSR, false.B, false.B, MemType.X,  CSROp.W, false.B, false.B, false.B, false.B),
    CSRRSI -> List(PCSel.PC_4, OpASel.RS1, OpBSel.IMM, AluOp.X,      true.B,  WBSel.CSR, false.B, false.B, MemType.X,  CSROp.S, false.B, false.B, false.B, false.B),
    CSRRCI -> List(PCSel.PC_4, OpASel.RS1, OpBSel.IMM, AluOp.X,      true.B,  WBSel.CSR, false.B, false.B, MemType.X,  CSROp.C, false.B, false.B, false.B, false.B),
    ECALL  -> List(PCSel.PC_4, OpASel.X,   OpBSel.X,   AluOp.X,      false.B, WBSel.X,   false.B, false.B, MemType.X,  CSROp.X, false.B, true.B,  false.B, false.B),
    EBREAK -> List(PCSel.PC_4, OpASel.X,   OpBSel.X,   AluOp.X,      false.B, WBSel.X,   false.B, false.B, MemType.X,  CSROp.X, false.B, false.B, true.B,  false.B),
    MRET   -> List(PCSel.PC_4, OpASel.X,   OpBSel.X,   AluOp.X,      false.B, WBSel.X,   false.B, false.B, MemType.X,  CSROp.X, false.B, false.B, false.B, true.B)
  )

  val signals = Wire(new ControlIO)
  val ctrlSignalsMap = signals.elements.toSeq.map(e => e._1 -> e._2)
  ctrlSignalsMap.zip(ListLookup.passthrough(io.inst, defaultSignals, instructionMap)).foreach { case ((_, signal), value) => signal := value }
  
  io <> signals
}
