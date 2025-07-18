#include "ftrace.h"
#include <elf.h>

static FTrace f_trace;
static FuncInfo funcs[MAX_FUNC];
static int func_cnt = 0;
static char *strtab = NULL;
static int call_depth = 0;

void init_ftrace() {
  f_trace.current = 0;
  f_trace.call_depth = 0;
  memset(f_trace.traces, 0, sizeof(f_trace.traces));
  Log("Function trace initialized");
}

void init_ftrace_elf(const char *elf_path) {
  if (!elf_path) {
    Log("Function trace: no ELF file provided, using addresses only");
    return;
  }

  FILE *fp = fopen(elf_path, "rb");
  if (!fp) {
    Log("Function trace: cannot open ELF file %s, using addresses only", elf_path);
    return;
  }

  Elf32_Ehdr ehdr;
  if (fread(&ehdr, 1, sizeof(ehdr), fp) != sizeof(ehdr)) {
    Log("Function trace: failed to read ELF header");
    fclose(fp);
    return;
  }

  // Check ELF magic number
  if (*(uint32_t *)ehdr.e_ident != 0x464c457f) {
    Log("Function trace: invalid ELF file");
    fclose(fp);
    return;
  }

  // Read section headers
  Elf32_Shdr *shdr = (Elf32_Shdr *)malloc(ehdr.e_shnum * sizeof(Elf32_Shdr));
  fseek(fp, ehdr.e_shoff, SEEK_SET);
  fread(shdr, sizeof(Elf32_Shdr), ehdr.e_shnum, fp);

  // Read section header string table
  Elf32_Shdr shstr = shdr[ehdr.e_shstrndx];
  char *shstrtab = (char *)malloc(shstr.sh_size);
  fseek(fp, shstr.sh_offset, SEEK_SET);
  fread(shstrtab, 1, shstr.sh_size, fp);

  // Find symbol table and string table
  Elf32_Shdr *symtab = NULL, *strtab_hdr = NULL;
  for (int i = 0; i < ehdr.e_shnum; i++) {
    const char *name = shstrtab + shdr[i].sh_name;
    if (strcmp(name, ".symtab") == 0)
      symtab = &shdr[i];
    if (strcmp(name, ".strtab") == 0)
      strtab_hdr = &shdr[i];
  }

  if (!symtab || !strtab_hdr) {
    Log("Function trace: no symbol table found in ELF file");
    free(shdr);
    free(shstrtab);
    fclose(fp);
    return;
  }

  // Read symbol table
  Elf32_Sym *syms = (Elf32_Sym *)malloc(symtab->sh_size);
  fseek(fp, symtab->sh_offset, SEEK_SET);
  fread(syms, 1, symtab->sh_size, fp);
  int sym_count = symtab->sh_size / sizeof(Elf32_Sym);

  // Read string table
  strtab = (char *)malloc(strtab_hdr->sh_size);
  fseek(fp, strtab_hdr->sh_offset, SEEK_SET);
  fread(strtab, 1, strtab_hdr->sh_size, fp);

  // Extract function symbols
  for (int i = 0; i < sym_count && func_cnt < MAX_FUNC; i++) {
    if (ELF32_ST_TYPE(syms[i].st_info) == STT_FUNC && syms[i].st_size > 0) {
      funcs[func_cnt].addr = syms[i].st_value;
      funcs[func_cnt].size = syms[i].st_size;
      funcs[func_cnt].name = strtab + syms[i].st_name;
      func_cnt++;
    }
  }

  free(syms);
  free(shdr);
  free(shstrtab);
  fclose(fp);

  Log("Function trace initialized with %d functions from %s", func_cnt, elf_path);
}

const char *ftrace_func_name(uint32_t addr) {
  for (int i = 0; i < func_cnt; i++) {
    if (addr >= funcs[i].addr && addr < funcs[i].addr + funcs[i].size) {
      return funcs[i].name;
    }
  }
  return NULL;
}

static void print_indent(int depth) {
  for (int i = 0; i < depth; i++) {
    printf("  ");
  }
}

void ftrace_call(uint32_t caller_pc, uint32_t target_pc) {
  // Print call trace immediately
  print_indent(call_depth);
  const char *name = ftrace_func_name(target_pc);
  if (name) {
    printf("[0x%08x] call -> 0x%08x <%s>\n", caller_pc, target_pc, name);
  } else {
    printf("[0x%08x] call -> 0x%08x\n", caller_pc, target_pc);
  }
  
  // Store in trace buffer
  f_trace.traces[f_trace.current].caller_pc = caller_pc;
  f_trace.traces[f_trace.current].target_pc = target_pc;
  f_trace.traces[f_trace.current].is_call = true;
  f_trace.traces[f_trace.current].depth = call_depth;
  if (name) {
    snprintf(f_trace.traces[f_trace.current].func_name, MAX_FUNC_NAME, "%s", name);
  } else {
    snprintf(f_trace.traces[f_trace.current].func_name, MAX_FUNC_NAME, "func_0x%08x", target_pc);
  }

  call_depth++;
  f_trace.call_depth = call_depth;
  f_trace.current = (f_trace.current + 1) % MAX_FTRACE_LEN;
}

void ftrace_ret(uint32_t ret_pc, uint32_t target_pc) {
  if (call_depth > 0) {
    call_depth--;
  }
  
  // Print return trace immediately
  print_indent(call_depth);
  const char *name = ftrace_func_name(ret_pc);
  if (name) {
    printf("[0x%08x] ret  <- 0x%08x <%s>\n", ret_pc, target_pc, name);
  } else {
    printf("[0x%08x] ret  <- 0x%08x\n", ret_pc, target_pc);
  }

  // Store in trace buffer
  f_trace.traces[f_trace.current].caller_pc = ret_pc;
  f_trace.traces[f_trace.current].target_pc = target_pc;
  f_trace.traces[f_trace.current].is_call = false;
  f_trace.traces[f_trace.current].depth = call_depth;
  if (name) {
    snprintf(f_trace.traces[f_trace.current].func_name, MAX_FUNC_NAME, "%s", name);
  } else {
    snprintf(f_trace.traces[f_trace.current].func_name, MAX_FUNC_NAME, "func_0x%08x", ret_pc);
  }

  f_trace.call_depth = call_depth;
  f_trace.current = (f_trace.current + 1) % MAX_FTRACE_LEN;
}

void ftrace_display() {
  Log("Recent function trace:");

  for (int i = 0; i < MAX_FTRACE_LEN; i++) {
    int index = (f_trace.current + i) % MAX_FTRACE_LEN;
    FuncTrace *trace = &f_trace.traces[index];

    if (trace->caller_pc == 0)
      continue;

    // Print indentation based on call depth
    for (int j = 0; j < trace->depth; j++) {
      printf("  ");
    }

    if (trace->is_call) {
      printf("call  [0x%08x] %s\n", trace->target_pc, trace->func_name);
    } else {
      printf("ret   [0x%08x] %s\n", trace->target_pc, trace->func_name);
    }
  }
}

bool is_fcall(uint32_t inst) {
  uint32_t opcode = inst & 0x7f;
  uint32_t rd = (inst >> 7) & 0x1f;
  return (opcode == 0x6f && rd == 1) || (opcode == 0x67 && rd == 1);
}

bool is_fret(uint32_t inst) {
  uint32_t opcode = inst & 0x7f;
  uint32_t rd = (inst >> 7) & 0x1f;
  uint32_t rs1 = (inst >> 15) & 0x1f;
  uint32_t imm = (inst >> 20) & 0xfff;
  return (opcode == 0x67 && rd == 0 && rs1 == 1 && imm == 0);
}
