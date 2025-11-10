# 🧠 Event-Based Concurrency — `select()` and `poll()` Notes

## 📘 Overview

To build an **event loop**, a program must have a way to **detect incoming I/O events** (like new network packets).  
This is achieved using **I/O multiplexing APIs** such as `select()` or `poll()`.

These system calls allow the program to:

- Check if any file descriptors (FDs) are ready for **reading**, **writing**, or have **errors**.
    
- Avoid blocking indefinitely while waiting for I/O.
    
- Efficiently manage multiple I/O sources **without threads**.
    

---

## ⚙️ The `select()` System Call

### 🧩 Function Prototype
```c
int select(int nfds,
           fd_set *restrict readfds,
           fd_set *restrict writefds,
           fd_set *restrict errorfds,
           struct timeval *restrict timeout);

```

### 📖 Description

- `select()` monitors sets of file descriptors:
    
    - **readfds** → to check if ready for reading
        
    - **writefds** → to check if ready for writing
        
    - **errorfds** → to check if exceptional conditions occurred
        
- **`nfds`** → highest-numbered descriptor in any set + 1
    
- **Return value:**
    
    - Number of ready descriptors
        
    - 0 if timeout occurs
        
    - -1 if an error happens
        

---

## 🧩 Key Concepts

### 1. **Blocking vs. Non-Blocking Interfaces**

|Type|Description|Example Behavior|
|---|---|---|
|**Blocking (Synchronous)**|The call **waits** until the operation completes|`read()` waits until data arrives|
|**Non-Blocking (Asynchronous)**|The call **returns immediately**, allowing other work|`select()` or `poll()` checks readiness before I/O|
- **In Event-Based Programming**, **non-blocking I/O** is essential.  
    A single blocking call can **freeze the entire event loop**, halting all progress.
    

---

### 2. **Checking Descriptor States**

`select()` allows checking:

- **Readable descriptors:** Data available to read (e.g., a new client message)
    
- **Writable descriptors:** Ready to send data (e.g., send buffer not full)
    

This lets a server handle **both input and output events** efficiently.

---

### 3. **Timeout Argument**

`timeout` controls how long `select()` waits:

- **`NULL`** → wait indefinitely until an event occurs
    
- **`{0, 0}`** (zero timeout) → return immediately (polling behavior)
    
- **Custom value** → wait up to a specific duration
    

> Example: Setting timeout to zero creates a **non-blocking loop** that continuously checks for ready descriptors.

---

## 🔁 Example: Simple Non-Blocking Event Loop Using `select()`
```c
#include <stdio.h>
#include <sys/select.h>
#include <unistd.h>

int main() {
    fd_set readfds;
    struct timeval timeout;
    int nfds = 1;  // check stdin (fd 0)

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(0, &readfds);  // monitor stdin

        timeout.tv_sec = 0;   // non-blocking
        timeout.tv_usec = 0;

        int ready = select(nfds, &readfds, NULL, NULL, &timeout);

        if (ready > 0 && FD_ISSET(0, &readfds)) {
            char buffer[128];
            read(0, buffer, sizeof(buffer));
            printf("You typed: %s\n", buffer);
        }
        // perform other tasks here...
    }
    return 0;
}
```

✅ **Key idea:** The program repeatedly checks if input is available without blocking — enabling concurrency in a single-threaded event loop.

---

## 🧰 The `poll()` Alternative

- Similar purpose and behavior as `select()`.
    
- Uses an array of `pollfd` structures instead of bitsets.
    
- Easier to scale with large numbers of file descriptors.
    
- Reference: _Stevens & Rago, Advanced Programming in the UNIX Environment (SR05)_.