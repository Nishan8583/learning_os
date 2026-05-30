## ⚙️ 33.7 Another Problem: State Management

### 💡 The Core Issue: Manual Stack Management

- In **thread-based programs**, each thread keeps its state (like local variables) on **its own stack**.
    
- In **event-based systems**, since everything runs in **one event loop**, there is **no separate call stack** per event.
    
- Therefore, when an event handler issues **asynchronous I/O**, it must:
    
    - **Manually save the program state** somewhere (heap or data structure)
        
    - **Restore it later** when the I/O completes  
        → This process is called **manual stack management** [Adya et al., A+02].
        

---

### 🧩 Example: Reading a File and Sending Data Over a Socket

#### 🧱 Thread-Based Code (simple & intuitive)
```c
int rc = read(fd, buffer, size);
rc = write(sd, buffer, size);
```
- When `read()` returns, the local variable `sd` is still on the **stack**.
    
- The thread simply proceeds to write the data to the socket.
    

#### ⚠️ Event-Based Equivalent

1. Issue an **asynchronous read** (e.g., `aio_read()`).
    
2. Wait (or poll) using `aio_error()` to check if it’s complete.
    
3. When complete → need to know **which socket (sd)** to write to.  
    But that information is no longer “on the stack.”
    
```c
struct continuation {
    int fd;
    int sd;
    char *buffer;
    size_t size;
};

// When read starts
continuations[fd] = (struct continuation){fd, sd, buffer, size};

// Later, when aio_error(fd) == 0:
struct continuation *c = lookup(fd);
write(c->sd, c->buffer, c->size);
```
---

### 🧠 Solution: Continuations

- Use a **continuation** — a data structure that records what needs to happen next.
    
- The continuation **stores program state** (e.g., file and socket descriptors).
    
- When the event completes, the server:
    
    1. Looks up the continuation (e.g., via a hash table indexed by file descriptor).
        
    2. Retrieves `sd` and other context.
        
    3. Performs the remaining work (e.g., write to socket).
        

#### Example Concept:
```c
struct continuation {
    int fd;
    int sd;
    char *buffer;
    size_t size;
};

// When read starts
continuations[fd] = (struct continuation){fd, sd, buffer, size};

// Later, when aio_error(fd) == 0:
struct continuation *c = lookup(fd);
write(c->sd, c->buffer, c->size);
```
✅ **Continuations** simulate a “saved stack frame” across asynchronous calls.

---

## 🧭 Aside: UNIX Signals

### ⚙️ What Are Signals?

- **Signals** are a UNIX mechanism for communication with running processes.
    
- When a signal is received:
    
    - The kernel **interrupts** the current process.
        
    - Runs a **signal handler** function.
        
    - Then resumes normal execution.
        

### Common Signals:

|Signal|Meaning|Default Action|
|---|---|---|
|`SIGHUP`|Hang-up|Terminate|
|`SIGINT`|Interrupt (Ctrl+C)|Terminate|
|`SIGSEGV`|Segmentation fault|Terminate|
🧩 Example: Catching a Signal in C
```c
#include <signal.h>
#include <stdio.h>

void handle(int arg) {
    printf("stop wakin’ me up...\n");
}

int main() {
    signal(SIGHUP, handle);
    while (1)
        ; // waiting forever
    return 0;
}
```

- The `kill` command sends signals to processes.
    
- When the signal arrives, the handler runs immediately.
    
- Signals can also be sent **by the kernel** (e.g., on segmentation fault).
## ⚙️ 33.8 What Is Still Difficult With Events

### ⚠️ 1. Scaling to Multiple CPUs

- Event-based servers are **simple** on a single CPU.
    
- On **multicore systems**, multiple event handlers must run **in parallel** → introduces:
    
    - Race conditions
        
    - Need for locks
        
- Thus, the simplicity of event-based design is **lost** on multicore hardware.
    

---

### ⚠️ 2. Paging and Implicit Blocking

- Even if explicit blocking calls are avoided, **page faults** can still cause **implicit blocking**.
    
- When a handler page faults, the event loop **stalls** until the page loads.
    
- This can lead to **serious performance problems**.
    

---

### ⚠️ 3. Changing API Semantics

- Over time, functions may change from **non-blocking → blocking**.
    
- This requires rewriting event handlers into multiple parts to handle new blocking behavior.
    
- Hence, **maintenance and refactoring** become difficult.
    

---

### ⚠️ 4. Asynchronous Disk vs. Network I/O

- Disk and network I/O systems are often **handled differently**:
    
    - Networking → managed via `select()` / `poll()`
        
    - Disk → via **AIO calls**
        
- The **lack of a unified interface** complicates event management.
    

✅ Ideally, one would like to use **a single interface** (`select()` or `epoll()`) for all I/O types — but OSes don’t provide perfect integration.

---

## 🧩 33.9 Summary

### 🧠 Key Insights

|Concept|Explanation|
|---|---|
|**Event-Based Concurrency**|Gives the application control over scheduling; avoids threading issues.|
|**Advantages**|No locks needed, fine-grained control, efficient single-threaded design.|
|**Challenges**|Manual state management, complex asynchronous flow, hard debugging.|
|**Scalability Issues**|Multicore usage requires locks again.|
|**Integration Issues**|Paging and blocking syscalls still cause stalls.|
|**Practical Reality**|Threads and events coexist — both have strengths and tradeoffs.|

### My code on poll can be found [here](https://github.com/Nishan8583/learning_languages/blob/main/c/employee_db/src/server.c#L142)
