# 8.4 Threads in Linux

A **thread** is a unit of execution within a process. Linux schedules and runs threads similarly to processes, and each thread has its own **Thread ID (TID)**.

The key difference is resource sharing:

- Separate processes generally have separate system resources and memory.
- Threads within the **same process share system resources and some memory**.
- Each thread still has its own identifier and is independently scheduled by the kernel.

---

## 8.4.1 Single-Threaded vs. Multithreaded Processes

### Single-threaded

A process containing only one thread is **single-threaded**.

All processes initially start with one thread, normally called the **main thread**.

### Multithreaded

A process containing multiple threads is **multithreaded**.

The main thread can create additional threads, similar conceptually to how a process can use `fork()` to create another process.

```
Process
└── Main Thread
    ├── Thread 2
    ├── Thread 3
    └── Thread 4
```

A single-threaded process:

```
Process
└── Main Thread
```

---

## Why Use Multiple Threads?

### Parallel execution

A multithreaded process can potentially execute multiple threads simultaneously across multiple processors/cores.

This can improve performance for workloads with substantial computation.

### Lower overhead than processes

Threads generally start faster than processes.

They can also communicate through **shared memory**, which can be easier/more efficient than communication between separate processes through mechanisms such as:

- Pipes
- Network connections

### Managing I/O

Threads can also be useful when a program needs to handle multiple I/O resources.

Instead of creating a separate process with `fork()` for each input/output stream, threads can provide a similar mechanism without the overhead of creating another process.

---

# 8.4.2 Viewing Threads

By default:

```
ps
top
```

primarily show **processes**, rather than individual threads.

For `ps`, the `m` option can be used to display thread information.

## `ps m`

```
ps m
```

Example conceptually:

```
PID     COMMAND
3587    bash
        └── thread
3592    bash
        └── thread
12534   Xorg
        ├── thread
        ├── thread
        ├── thread
        └── thread
```

In `ps m` output:

- A line containing a **PID** represents a process.
- Lines containing `-` in the PID column represent threads belonging to that process.
- A process with only itself listed has one thread.
- Multiple thread entries indicate a multithreaded process.

---

# Displaying PID and TID

To explicitly display the **PID, TID, and command**:

```
ps m -o pid,tid,command
```

The custom output format selects:

```
pid  → Process ID
tid  → Thread ID
command → Command associated with the process/thread
```

Example:

```
PID     TID     COMMAND
3587    -       bash
        3587
3592    -       bash
        3592
12534   -       Xorg
        12534
        13227
        14443
        14448
```

---

# PID vs. TID

An important observation is that the **main thread's TID is normally the same as the process's PID**.

For a single-threaded process:

```
PID = TID
```

For a multithreaded process:

```
PID 12534
│
├── TID 12534   ← main thread
├── TID 13227
├── TID 14443
└── TID 14448
```

So the process's PID identifies the process, while each individual thread has its own TID.

---

# Interacting with Individual Threads

Although Linux exposes individual threads, you generally **don't interact with them the same way you interact with processes**.

Acting on one thread at a time requires understanding how the multithreaded application was implemented, and manipulating an individual thread may not always be appropriate.

For threat hunting/resource analysis, however, identifying individual TIDs can be useful because threads within the same process can behave differently and consume resources independently.

---

# Threads and Resource Monitoring

Threads can complicate resource monitoring because **multiple threads belonging to the same process can consume resources simultaneously**.

For example, `top` does **not display individual threads by default**.

Press:

```
H
```

inside `top` to enable thread display.

Other resource-monitoring utilities may similarly require an option or additional configuration to show threads.