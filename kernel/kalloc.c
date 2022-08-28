// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);
int kincref(uint64 pa);
int kdecref(uint64 pa);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

struct {
  int count[PHYSTOP / PGSIZE];
  struct spinlock lock;
} refcount;

void
kinitrefcount() {
  int i;
  initlock(&refcount.lock, "refcount");
  acquire(&refcount.lock);
  for (i = 0;i < PHYSTOP/PGSIZE; i++) {
    refcount.count[i] = 1;
  }
  release(&refcount.lock);
}

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  kinitrefcount();
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE) {
    kfree(p);
  }
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  acquire(&kmem.lock);
  if (kdecref((uint64) pa) == 0) {
    // Fill with junk to catch dangling refs.
    memset(pa, 1, PGSIZE);

    r = (struct run*)pa;

    r->next = kmem.freelist;
    kmem.freelist = r;
  }
  release(&kmem.lock);

}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r) {
    kmem.freelist = r->next;
  }
  release(&kmem.lock);

  acquire(&refcount.lock);
  refcount.count[(uint64)r / PGSIZE] = 1;
  release(&refcount.lock);

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}

uint64 freemem(void) {
  uint64 n = 0;
  struct run *r;
  acquire(&kmem.lock);
  r = kmem.freelist;
  while (r) {
    n += PGSIZE;
    r = r->next;
  }
  release(&kmem.lock);
  return n;
}

// return number of the reference after increment the reference count
int kincref(uint64 pa) {
  acquire(&refcount.lock);
  refcount.count[pa / PGSIZE]++;
  release(&refcount.lock);
  return refcount.count[pa / PGSIZE];
}

int kdecref(uint64 pa) {
  acquire(&refcount.lock);
  refcount.count[pa / PGSIZE]--;
  release(&refcount.lock);
  return refcount.count[pa / PGSIZE];
}