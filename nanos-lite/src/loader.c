#include <elf.h>
#include <fs.h>
#include <proc.h>
#include <stdint.h>

#ifdef __LP64__
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Phdr Elf64_Phdr
#else
#define Elf_Ehdr Elf32_Ehdr
#define Elf_Phdr Elf32_Phdr
#endif

#if defined(__ISA_AM_NATIVE__)
#define EXPECT_TYPE EM_X86_64
#elif defined(__ISA_X86__)
#define EXPECT_TYPE EM_X86_64
#elif defined(__ISA_RISCV32__)
#define EXPECT_TYPE EM_RISCV
#elif defined(__ISA_RISCV32E__)
#define EXPECT_TYPE EM_RISCV
#elif defined(__ISA_RISCV32MINI__)
#define EXPECT_TYPE EM_RISCV
#elif defined(__ISA_RISCV64__)
#define EXPECT_TYPE EM_RISCV
#else
#error Unsupported ISA
#endif

void ramdisk_read(void *buf, size_t offset, size_t len);

static uintptr_t loader(PCB *pcb, const char *filename) {
  int fd = fs_open(filename, 0, 0);
  if (fd < 0) {
    panic("file %s not found\n", filename);
  }
  Elf_Ehdr ehdr;
  fs_read(fd, &ehdr, sizeof(Elf_Ehdr));
  assert(*(uint32_t *)ehdr.e_ident == 0x464c457f);
  assert(ehdr.e_machine == EXPECT_TYPE);

  Elf_Phdr ph[ehdr.e_phnum];
  fs_lseek(fd, ehdr.e_phoff, SEEK_SET);
  fs_read(fd, ph, sizeof(Elf_Phdr) * ehdr.e_phnum);
  for (int i = 0; i < ehdr.e_phnum; i++) {
    if (ph[i].p_type == PT_LOAD) {
      Log("LOAD segment %d: vaddr=0x%x, filesz=0x%x, memsz=0x%x", 
          i, ph[i].p_vaddr, ph[i].p_filesz, ph[i].p_memsz);
      
      fs_lseek(fd, ph[i].p_offset, SEEK_SET);
      fs_read(fd, (void *)ph[i].p_vaddr, ph[i].p_filesz);
      
      // Check seed4_volatile before memset
      if (ph[i].p_vaddr <= 0x83014c08 && 0x83014c08 < ph[i].p_vaddr + ph[i].p_memsz) {
        uint32_t *seed4_ptr = (uint32_t *)0x83014c08;
        Log("seed4_volatile before memset: 0x%x", *seed4_ptr);
      }
      
      memset((void *)(ph[i].p_vaddr + ph[i].p_filesz), 0,
             ph[i].p_memsz - ph[i].p_filesz);
             
      // Check seed4_volatile after memset  
      if (ph[i].p_vaddr <= 0x83014c08 && 0x83014c08 < ph[i].p_vaddr + ph[i].p_memsz) {
        uint32_t *seed4_ptr = (uint32_t *)0x83014c08;
        Log("seed4_volatile after memset: 0x%x", *seed4_ptr);
      }
    }
  }
  fs_close(fd);
  return ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void (*)())entry)();
}
