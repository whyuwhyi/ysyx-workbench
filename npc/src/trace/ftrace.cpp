#include <common.h>
#include <elf.h>

typedef struct {
  uint32_t addr;
  uint32_t size;
  const char *name;
} FuncInfo;

static FuncInfo funcs[256];
static int func_cnt = 0;
static char *strtab = NULL;
static int call_depth = 0;

void init_ftrace(const char *elf_path) {
  Log("Function trace initialized");

  if (!elf_path) {
    return;
  }

  FILE *fp = fopen(elf_path, "rb");
  if (!fp) {
    return;
  }

  Elf32_Ehdr ehdr;
  if (fread(&ehdr, 1, sizeof(ehdr), fp) != sizeof(ehdr)) {
    fclose(fp);
    return;
  }

  if (*(uint32_t *)ehdr.e_ident != 0x464c457f) {
    fclose(fp);
    return;
  }

  Elf32_Shdr *shdr = (Elf32_Shdr *)malloc(ehdr.e_shnum * sizeof(Elf32_Shdr));
  fseek(fp, ehdr.e_shoff, SEEK_SET);
  fread(shdr, sizeof(Elf32_Shdr), ehdr.e_shnum, fp);

  Elf32_Shdr shstr = shdr[ehdr.e_shstrndx];
  char *shstrtab = (char *)malloc(shstr.sh_size);
  fseek(fp, shstr.sh_offset, SEEK_SET);
  fread(shstrtab, 1, shstr.sh_size, fp);

  Elf32_Shdr *symtab = NULL, *strtab_hdr = NULL;
  for (int i = 0; i < ehdr.e_shnum; i++) {
    const char *name = shstrtab + shdr[i].sh_name;
    if (strcmp(name, ".symtab") == 0)
      symtab = &shdr[i];
    if (strcmp(name, ".strtab") == 0)
      strtab_hdr = &shdr[i];
  }

  if (!symtab || !strtab_hdr) {
    free(shdr);
    free(shstrtab);
    fclose(fp);
    return;
  }

  Elf32_Sym *syms = (Elf32_Sym *)malloc(symtab->sh_size);
  fseek(fp, symtab->sh_offset, SEEK_SET);
  fread(syms, 1, symtab->sh_size, fp);
  int sym_count = symtab->sh_size / sizeof(Elf32_Sym);

  strtab = (char *)malloc(strtab_hdr->sh_size);
  fseek(fp, strtab_hdr->sh_offset, SEEK_SET);
  fread(strtab, 1, strtab_hdr->sh_size, fp);

  for (int i = 0; i < sym_count && func_cnt < 256; i++) {
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
}

static const char *ftrace_func_name(uint32_t addr) {
  for (int i = 0; i < func_cnt; i++) {
    if (addr >= funcs[i].addr && addr < funcs[i].addr + funcs[i].size) {
      return funcs[i].name;
    }
  }
  return NULL;
}

static void print_indent() {
  for (int i = 0; i < call_depth; i++) {
    Log("  ");
  }
}

void ftrace_call(uint32_t from, uint32_t to) {
  print_indent();

  const char *name = ftrace_func_name(to);
  if (name) {
    Log("[0x%08x] call -> 0x%08x <%s>", from, to, name);
  } else {
    Log("[0x%08x] call -> 0x%08x", from, to);
  }

  call_depth++;
}

void ftrace_ret(uint32_t from, uint32_t to) {
  if (call_depth > 0) {
    call_depth--;
  }

  for (int i = 0; i < call_depth; i++) {
    printf("  ");
  }

  const char *name = ftrace_func_name(from);
  if (name) {
    Log("[0x%08x] ret  <- 0x%08x <%s>", from, to, name);
  } else {
    Log("[0x%08x] ret  <- 0x%08x", from, to);
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
