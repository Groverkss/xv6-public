#include "types.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "x86.h"
#include "traps.h"
#include "syscall.h"
#include "elf.h"
#include "param.h"

extern char edata[], end[];
extern int acpu;
extern char _binary_user1_start[];
extern char _binary_user1_size[];

char buf[512];

int
main()
{
  struct proc *p;

  if (acpu) {
    cprintf("an application processor\n");
    release_spinlock(&kernel_lock);
    acquire_spinlock(&kernel_lock);
    idtinit();
    lapic_init(cpu());
    curproc[cpu()] = &proc[0]; // XXX
    swtch();
  }
  acpu = 1;
  // clear BSS
  memset(edata, 0, end - edata);

  cprintf("\nxV6\n\n");

  mp_init(); // multiprocessor
  kinit(); // physical memory allocator
  tvinit(); // trap vectors
  idtinit(); // CPU's idt
  pic_init();

  // create fake process zero
  p = &proc[0];
  curproc[cpu()] = p;
  p->state = WAITING;
  p->sz = PAGE;
  p->mem = kalloc(p->sz);
  memset(p->mem, 0, p->sz);
  p->kstack = kalloc(KSTACKSIZE);
  p->tf = (struct Trapframe *) (p->kstack + KSTACKSIZE - sizeof(struct Trapframe));
  memset(p->tf, 0, sizeof(struct Trapframe));
  p->tf->tf_es = p->tf->tf_ds = p->tf->tf_ss = (SEG_UDATA << 3) | 3;
  p->tf->tf_cs = (SEG_UCODE << 3) | 3;
  p->tf->tf_eflags = FL_IF;
  p->pid = 0;
  p->ppid = 0;
  setupsegs(p);

  // turn on interrupts
  irq_setmask_8259A(0xff);
  write_eflags(read_eflags() | FL_IF);

#if 0
  ide_read(0, buf, 1);
  cprintf("sec0.0 %x\n", buf[0] & 0xff);
#endif

#if 1
  p = newproc();
  load_icode(p, _binary_user1_start, (unsigned) _binary_user1_size);
#endif

#if 0
  i = 0;
  p->mem[i++] = 0x90; // nop 
  p->mem[i++] = 0xb8; // mov ..., %eax
  p->mem[i++] = SYS_fork;
  p->mem[i++] = 0;
  p->mem[i++] = 0;
  p->mem[i++] = 0;
  p->mem[i++] = 0xcd; // int
  p->mem[i++] = T_SYSCALL;
  p->mem[i++] = 0xb8; // mov ..., %eax
  p->mem[i++] = SYS_wait;
  p->mem[i++] = 0;
  p->mem[i++] = 0;
  p->mem[i++] = 0;
  p->mem[i++] = 0xcd; // int
  p->mem[i++] = T_SYSCALL;
  p->mem[i++] = 0xb8; // mov ..., %eax
  p->mem[i++] = SYS_exit;
  p->mem[i++] = 0;
  p->mem[i++] = 0;
  p->mem[i++] = 0;
  p->mem[i++] = 0xcd; // int
  p->mem[i++] = T_SYSCALL;
  p->tf->tf_eip = 0;
  p->tf->tf_esp = p->sz;
#endif

  swtch();

  return 0;
}

void
load_icode(struct proc *p, uint8_t *binary, unsigned size)
{
	int i;
	struct Elf *elf;
	struct Proghdr *ph;

	// Check magic number on binary
	elf = (struct Elf*) binary;
        cprintf("elf %x magic %x\n", elf, elf->e_magic);
	if (elf->e_magic != ELF_MAGIC)
		panic("load_icode: not an ELF binary");

  p->tf->tf_eip = elf->e_entry;
  p->tf->tf_esp = p->sz;

	// Map and load segments as directed.
	ph = (struct Proghdr*) (binary + elf->e_phoff);
	for (i = 0; i < elf->e_phnum; i++, ph++) {
		if (ph->p_type != ELF_PROG_LOAD)
			continue;
                cprintf("va %x memsz %d\n", ph->p_va, ph->p_memsz);
		if (ph->p_va + ph->p_memsz < ph->p_va)
			panic("load_icode: overflow in elf header segment");
		if (ph->p_va + ph->p_memsz >= p->sz)
			panic("load_icode: icode wants to be above UTOP");

		// Load/clear the segment
		memcpy(p->mem + ph->p_va, binary + ph->p_offset, ph->p_filesz);
		memset(p->mem + ph->p_va + ph->p_filesz, 0, ph->p_memsz - ph->p_filesz);
	}
}
