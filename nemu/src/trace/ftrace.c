#include <common.h>
#include <elf.h>

#define MAX_FUNC 65536

typedef struct {
  paddr_t addr;
  word_t size;
  const char *name;
} FuncInfo;

static FuncInfo funcs[MAX_FUNC];
static int func_cnt = 0;
static char *strtab = NULL;
static int call_depth = 0;

void init_ftrace(const char *elf_path) {
  if (!elf_path) {
    Log("Function trace may can not work well: no ELF file provided");
    return;
  }

  FILE *fp = fopen(elf_path, "rb");
  if (!fp) {
    Log("Function trace may can not work well: cannot open ELF file %s",
        elf_path);
    return;
  }

  Elf32_Ehdr ehdr;
  fread(&ehdr, 1, sizeof(ehdr), fp);
  assert(*(vaddr_t *)ehdr.e_ident == 0x464c457f);

  Elf32_Shdr shdr[ehdr.e_shnum];
  fseek(fp, ehdr.e_shoff, SEEK_SET);
  fread(shdr, sizeof(Elf32_Shdr), ehdr.e_shnum, fp);

  Elf32_Shdr shstr = shdr[ehdr.e_shstrndx];
  char shstrtab[shstr.sh_size];
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
  assert(symtab && strtab_hdr);

  Elf32_Sym *syms = malloc(symtab->sh_size);
  fseek(fp, symtab->sh_offset, SEEK_SET);
  fread(syms, 1, symtab->sh_size, fp);
  int sym_count = symtab->sh_size / sizeof(Elf32_Sym);

  strtab = malloc(strtab_hdr->sh_size);
  fseek(fp, strtab_hdr->sh_offset, SEEK_SET);
  fread(strtab, 1, strtab_hdr->sh_size, fp);

  for (int i = 0; i < sym_count; i++) {
    if (ELF32_ST_TYPE(syms[i].st_info) == STT_FUNC && syms[i].st_size > 0) {
      funcs[func_cnt].addr = syms[i].st_value;
      funcs[func_cnt].size = syms[i].st_size;
      funcs[func_cnt].name = strtab + syms[i].st_name;
      func_cnt++;
    }
  }
  free(syms);
  fclose(fp);

  Log("Function trace initialized with %d functions", func_cnt);
}

const char *ftrace_func_name(vaddr_t addr) {
  for (int i = 0; i < func_cnt; i++) {
    if (addr >= funcs[i].addr && addr < funcs[i].addr + funcs[i].size) {
      return funcs[i].name;
    }
  }
  return NULL;
}

static char buf[256];

void ftrace_call(vaddr_t from, vaddr_t to) {
  for (int i = 0; i < call_depth; i++) {
    buf[i] = ' ';
  }

  const char *name = ftrace_func_name(to);
  if (name) {
    snprintf(buf + call_depth, sizeof(buf) - call_depth,
             "[" FMT_WORD "] call -> [" FMT_WORD "] <%s>", from, to, name);
  } else {
    snprintf(buf + call_depth, sizeof(buf) - call_depth,
             "[" FMT_WORD "] call -> [" FMT_WORD "]", from, to);
  }
  Log("%s", buf);
  call_depth = call_depth > 10 ? 10 : call_depth + 1;
}

void ftrace_ret(vaddr_t from, vaddr_t to) {
  call_depth = call_depth > 0 ? call_depth - 1 : 0;
  for (int i = 0; i < call_depth; i++) {
    buf[i] = ' ';
  }

  const char *name = ftrace_func_name(from);
  if (name) {
    snprintf(buf + call_depth, sizeof(buf) - call_depth,
             "[" FMT_WORD "] ret -> [" FMT_WORD "] <%s>", from, to, name);
  } else {
    snprintf(buf + call_depth, sizeof(buf) - call_depth,
             "[" FMT_WORD "] ret -> [" FMT_WORD "]", from, to);
  }
  Log("%s", buf);
}

bool is_fcall(word_t inst) {
  word_t opcode = inst & 0x7f;
  word_t rd = (inst >> 7) & 0x1f;
  return (opcode == 0x6f && rd == 1) || (opcode == 0x67 && rd == 1);
}

bool is_fret(word_t inst) {
  word_t opcode = inst & 0x7f;
  word_t rd = (inst >> 7) & 0x1f;
  word_t rs1 = (inst >> 15) & 0x1f;
  word_t imm = (inst >> 20) & 0xfff;
  return (opcode == 0x67 && rd == 0 && rs1 == 1 && imm == 0);
}
