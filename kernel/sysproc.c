#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_trace(void) {
  int mask;
  argint(0, &mask);
  if (mask <= 0) {
    return 0;
  }

  myproc()->tracemask = mask;

  return 1;
}

uint64
sys_pgaccess(void) {
  uint64 vstart, buf;
  int num, i;
  struct proc *p;
  pte_t *pte;
  uint64 mask = 0;

  argaddr(0, &vstart);
  argint(1, &num);
  argaddr(2, &buf);

  // since we are using a bit mask to store the result,
  // if the number of examined page exceed the number of the bit of
  // the mask, we terminate the system call.
  if (num > sizeof(mask) * 8) {
    return -1;
  }

  // if range of the examined address space exceeds the maximum
  // virtual address, terminate the system call.
  if (vstart + num * PGSIZE > MAXVA) {
    return -1;
  }

  p = myproc();

  for (i = 0;i < num; i++) {
    pte = walk(p->pagetable, vstart, 0);

    // if the pte is valid and is accessed, update the mask
    if ((*pte & PTE_V) && (*pte & PTE_A) > 0) {
      mask |= (1 << i);
      
      // use xor to clear the a bit
      *pte ^= PTE_A;
    }
  
    vstart += PGSIZE;
  }

  copyout(p->pagetable, buf, (char *)&mask, sizeof(mask));
  return 1;
}

uint64 sys_sigalarm(void) {
  int interval;
  uint64 handler;
  struct proc *p;
  p = myproc();

  argint(0, &interval);
  argaddr(1, &handler);
  if (interval < 0 || handler < 0) {
    return -1;
  }

  p->sigalarm.interval = interval;
  p->sigalarm.handler = handler;
  p->sigalarm.elapsed = 0;

  return 0;
}

uint64 sys_sigreturn(void) {
  struct proc *p;
  p = myproc();
  // copy the trapframe back
  memmove(p->trapframe, &p->sigalarm.trapframe, sizeof(struct trapframe));
  return 0;
}