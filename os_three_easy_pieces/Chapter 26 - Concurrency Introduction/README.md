# 🧵 Concurrency: An Introduction

## Overview

Operating Systems provide abstractions such as:

- **Virtual CPUs** → enables multiple programs to run _as if simultaneously_.
    
- **Virtual memory** → gives each process the illusion of its own private address space.
    

This chapter introduces a new abstraction inside a single process: the **thread**.

---

## 26.0 What Is a Thread?

A **thread** represents a single point of execution within a process.

### Characteristics

- Each thread has:
    
    - Its own **program counter (PC)**.
        
    - Its own **register set**.
        
    - Its own **stack**.
        
- Threads **share the same address space** and data.
    

### Context Switching

When switching between threads:

- Registers of the current thread (T1) are saved.
    
- Registers of the next thread (T2) are restored.
    
- Unlike process switches, **the address space remains the same** — no need to switch page tables.
    

---

## 26.1 Thread Stacks

A single-threaded process has one stack:
```
+--------------------+
| Stack (grows down) |
+--------------------+
| Heap  (grows up)   |
+--------------------+
| Program Code       |
+--------------------+
```

A multi-threaded process has **one stack per thread**:
```c
+--------------------+
| Stack (Thread 1)   |
| Stack (Thread 2)   |
+--------------------+
| Heap               |
| Program Code       |
+--------------------+

```

![[Pasted image 20251028083936.png]]

Each stack contains:

- Local variables
    
- Function arguments
    
- Return addresses
    

🧠 **Thread-local storage** refers to variables local to each thread’s stack.

---

## 26.2 Why Use Threads?

### 1. **Parallelism**

Threads allow multiple CPUs to process parts of a task simultaneously.

> Example: Splitting an array addition across multiple processors.

### 2. **Avoiding Blocking**

Threads help overlap I/O with computation:

- One thread may wait for I/O.
    
- Others can continue computation.
    

Alternative: Multiple processes  
→ But threads share memory easily, making them better for tightly-coupled tasks.

---

## 26.3 Example: Thread Creation

### Code Example — `t0.c`
```c
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include "common.h"
#include "common_threads.h"

void *mythread(void *arg) {
    printf("%s\n", (char *) arg);
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t p1, p2;
    printf("main: begin\n");
    Pthread_create(&p1, NULL, mythread, "A");
    Pthread_create(&p2, NULL, mythread, "B");
    Pthread_join(p1, NULL);
    Pthread_join(p2, NULL);
    printf("main: end\n");
    return 0;
}

```

### Explanation

- `Pthread_create()` creates threads `T1` and `T2`.
    
- `Pthread_join()` waits for their completion.
    
- Threads run independently; execution order is **non-deterministic**.
    

Possible outputs:
```
main: begin
A
B
main: end
```

Or:
```
main: begin
B
A
main: end
```


Or interleaved in other ways depending on scheduler timing.

🧩 **Key point:** Thread scheduling introduces **non-determinism**.

---

## 26.4 Shared Data Problem

### Code Example — `t1.c`
```c
#include <stdio.h>
#include <pthread.h>
#include "common.h"
#include "common_threads.h"

static volatile int counter = 0;

void *mythread(void *arg) {
    printf("%s: begin\n", (char *) arg);
    for (int i = 0; i < 1e7; i++) {
        counter = counter + 1;
    }
    printf("%s: done\n", (char *) arg);
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t p1, p2;
    printf("main: begin (counter = %d)\n", counter);
    Pthread_create(&p1, NULL, mythread, "A");
    Pthread_create(&p2, NULL, mythread, "B");
    Pthread_join(p1, NULL);
    Pthread_join(p2, NULL);
    printf("main: done with both (counter = %d)\n", counter);
    return 0;
}
```

Expected Output
```bash
main: done with both (counter = 20000000)
```
### Actual Output

Different every run
```bash
main: done with both (counter = 19345221)
```

This is due to **race conditions**.

---

## 26.5 The Heart of the Problem: Uncontrolled Scheduling

### Assembly Breakdown

Example compiled code for `counter = counter + 1`:
```asm
mov 0x8049a1c, %eax   ; load counter into register
add $0x1, %eax        ; add 1
mov %eax, 0x8049a1c   ; store back to memory
```

### Execution Race Example

|Step|Thread|%eax|counter|
|---|---|---|---|
|1|T1 loads counter|50|50|
|2|T1 adds 1|51|50|
|3|Context switch → T2 runs|||
|4|T2 loads counter|50|50|
|5|T2 adds 1|51|50|
|6|T2 stores 51|51|**51**|
|7|Context switch → T1 resumes|51|51|
|8|T1 stores 51|51|**51**|

💥 **Result:** 2 increments → counter = 51 (should be 52).

This nondeterministic behavior defines a **race condition** (or **data race**).

## 26.6 Key Concepts

| Term                      | Description                                                                           |
| ------------------------- | ------------------------------------------------------------------------------------- |
| **Critical Section**      | Code accessing shared resources that must not run concurrently.                       |
| **Race Condition**        | Multiple threads accessing shared data simultaneously, causing unpredictable results. |
| **Indeterminate Program** | Output changes between runs due to races.                                             |
| **Mutual Exclusion**      | Guarantee that only one thread executes a critical section at a time.                 |

---

## 26.7 Atomic Operations

### Concept

Atomicity = “**all or nothing**”.

If the following executes atomically:
```asm
mov 0x8049a1c, %eax
add $0x1, %eax
mov %eax, 0x8049a1c
```

then no interleaving can occur.

### Hypothetical Atomic Instruction
```asm
memory-add 0x8049a1c, $0x1
```

This would make the operation **uninterruptible**.

Since real CPUs lack complex atomic instructions, we rely on:

- **Hardware primitives** (like `test-and-set`, `compare-and-swap`)
    
- **Synchronization primitives** (locks, semaphores)
    
- **OS support** to build safe concurrent systems.
    

---

## 26.8 The Bigger Problem: Waiting for Another

Threads may also **depend** on others’ progress.

Example:

- A thread performing I/O must sleep until data arrives.
    
- Another must wake it.
    

➡️ Introduces **condition variables** and **synchronization mechanisms**, which are studied later.

---

## 26.9 Why This Matters in OS Design

Operating Systems are the **first concurrent programs**:

- Multiple interrupts
    
- Processes updating shared kernel structures
    

Example:  
Two processes call `write()` to append to a file:

- Both update the file’s inode and allocation structures.
    
- These updates must be **atomic** to avoid corruption.
    

Kernel data (page tables, process lists, file systems, etc.) require **synchronization primitives**.

## 🧩 Summary

| Concept                        | Description                                                           |
| ------------------------------ | --------------------------------------------------------------------- |
| **Threads**                    | Independent execution units within a process sharing memory.          |
| **Parallelism**                | Multiple CPUs performing work simultaneously.                         |
| **Blocking I/O**               | Threads allow computation to continue during I/O waits.               |
| **Race Condition**             | Unpredictable results due to concurrent shared access.                |
| **Critical Section**           | Shared code requiring mutual exclusion.                               |
| **Atomicity**                  | All-or-nothing execution guarantee.                                   |
| **Synchronization Primitives** | Mechanisms (locks, semaphores, etc.) ensuring safe concurrent access. |


## 🧠 References

- Dijkstra, E. W. — _“Solution of a Problem in Concurrent Programming Control”_ (1965)
    
- Dijkstra, E. W. — _“Cooperating Sequential Processes”_ (1968)
    
- Gray & Reuter — _“Transaction Processing: Concepts and Techniques”_ (1992)
    
- Stevens & Rago — _“Advanced Programming in the UNIX Environment”_ (2005)