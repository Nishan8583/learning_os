In `htop`, these columns describe different aspects of a process's memory usage and state:

|Column|Meaning|What it represents|
|---|---|---|
|**VIRT**|Virtual memory|Total virtual address space available to/used by the process|
|**RES**|Resident memory|Actual physical RAM currently occupied by the process|
|**SHR**|Shared memory|Portion of the process's resident memory that may be shared with other processes|
|**S**|Process state|Current scheduling/state of the process|

### 1. `VIRT` — Virtual Memory

**VIRT** is the total amount of **virtual address space** associated with the process.

It can include:

- Code/text
- Heap
- Stack
- Shared libraries
- Memory-mapped files
- Shared memory
- Allocated but currently unused memory
- Memory that has been swapped out

So:

```
VIRT ≠ actual RAM usage
```

For example:

```
VIRT = 10G
RES  = 500M
```

does **not** mean the process is using 10 GB of physical RAM. It has a 10-GB virtual address space, but only ~500 MB is currently resident in RAM.

---

### 2. `RES` — Resident Set Size

**RES** is generally the most useful column for answering:

> "How much physical RAM is this process currently occupying?"

For example:

```
RES = 450M
```

means roughly **450 MB of the process's memory is currently resident in physical RAM**.

However, some of that RAM can be shared with other processes.

That's where `SHR` matters.

---

### 3. `SHR` — Shared Memory

**SHR** is the amount of the process's resident memory that is potentially **shared with other processes**.

A common example is shared libraries.

Suppose you have:

```
Process A
  libc.so
  libpthread.so
  application code
  heap
```

and another process also uses `libc.so`.

The library's memory pages can be mapped into both processes rather than having two independent copies in RAM.

So you might see:

```
RES = 500M
SHR = 100M
```

Conceptually:

```
500 MB resident memory
├── ~400 MB private
└── ~100 MB potentially shared
```

`RES - SHR` is therefore sometimes used as a rough approximation of **private resident memory**, but it isn't a perfect accounting of a process's true unique RAM usage.

---

### 4. `S` — Process State

`S` isn't memory-related. It shows the **current process state**.

Common values:

|`S`|Meaning|
|---|---|
|`R`|**Running/runnable** — currently running or ready to run|
|`S`|**Sleeping** — waiting for an event, usually interruptible|
|`D`|**Uninterruptible sleep** — commonly waiting on I/O|
|`T`|**Stopped/traced**|
|`Z`|**Zombie**|
|`I`|**Idle kernel thread**|

For example:

```
PID     USER   PRI  NI   VIRT   RES   SHR  S  CPU%  MEM%
1234    bob    20   0    2.5G   350M  80M  S   2.0   0.4
```

You could interpret this as:

- `VIRT 2.5G` → process has a 2.5-GB virtual address space
- `RES 350M` → ~350 MB is resident in RAM
- `SHR 80M` → ~80 MB of that resident memory is potentially shared
- `S` → process is currently sleeping/waiting
- `CPU% 2.0` → currently using ~2% CPU

### The important mental model

Think of it as:

```
                Virtual Address Space
                       VIRT
                ┌─────────────────┐
                │                 │
                │  mapped files   │
                │  libraries      │
                │  heap           │
                │  stack          │
                │  etc.           │
                │                 │
                └────────┬────────┘
                         │
                  pages currently
                  resident in RAM
                         │
                    ┌────▼────┐
                    │   RES   │
                    │         │
                    │ private │
                    │   +     │
                    │ shared  │◄── SHR
                    └─────────┘
```

**For threat hunting:** `RES` is generally much more interesting than `VIRT` when you're looking for processes consuming actual RAM. But an unusually large `VIRT` can still be useful as a clue—especially when investigating memory mappings, JIT runtimes, `mmap()`, memory-mapped files, or suspicious allocations.

---

## Question:

```
So VIRT means virtual memory, in modern OS, process does not have actual access to physical memory, instead OS virtualizes memory for them. OS maintains per process page table, where process gets virtual addresses, which are mapped to physical memory, SO VIRT means the amount of virtual memory/virtual address space for the process ?
```


Yes — **that's essentially correct**. The important distinction is that `VIRT` is better understood as **virtual address space**, not necessarily "virtual memory that is actually being used."

Your mental model is good:

```
Process
   │
   │ uses virtual addresses
   ▼
┌─────────────────────┐
│ Per-process         │
│ virtual address     │
│ space               │
└──────────┬──────────┘
           │
           │ Page tables
           ▼
┌─────────────────────┐
│ Physical memory     │
│ (RAM)               │
└─────────────────────┘
```

For example, a process might have:

```
VIRT = 4 GB
RES  = 500 MB
```

This means roughly:

- **VIRT 4 GB** → the process has **4 GB of virtual address space mapped/allocated** in its memory layout.
- **RES 500 MB** → about **500 MB of those pages are currently resident in physical RAM**.
- The remaining virtual address space might correspond to pages that are:
    - not currently resident,
    - backed by files,
    - shared libraries,
    - reserved address space,
    - swapped out, etc.

### One small correction

It's not quite that the process has **no access to physical memory**.

Rather, user-space processes normally **access memory through virtual addresses**, and the CPU's **MMU + page tables** translate those virtual addresses into physical addresses.

For example:

```
Process executes:

mov rax, [0x7f1234000]
             │
             │ virtual address
             ▼
        ┌───────────┐
        │    MMU    │
        │ + page    │
        │   tables  │
        └─────┬─────┘
              │
              │ physical address
              ▼
          RAM page
```

And importantly, **not every virtual page needs to have a physical RAM page behind it at that moment**. This is why:

```
VIRT ≠ RAM usage
```

and why `RES` is much closer to what you're thinking of when you ask:

> "How much physical memory is this process currently occupying?"

So your summary could be:

> **VIRT = the process's virtual address space, which the OS/CPU manages through virtual-memory mappings rather than giving the process direct access to physical addresses.**

That's the right mental model.