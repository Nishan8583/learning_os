## ⚠️ Tip: Don’t Block in Event-Based Servers

**Rule #1:** _Never make a blocking call inside an event-based server._

- Event-based servers rely on **fine-grained control** of task scheduling.
    
- If **any function blocks**, the **entire event loop** stops.
    
- This causes:
    
    - Stalled server responsiveness
        
    - Frustrated clients
        
    - Inefficient CPU usage
        

✅ **Design Rule:** Always use **non-blocking or asynchronous** operations.

---

## 🧩 33.4 Why Simpler? No Locks Needed

### ✨ Advantage of Event-Based Concurrency:

- With a **single-threaded event loop**, only **one event** is processed at a time.
    
- Therefore:
    
    - No **data races**
        
    - No **deadlocks**
        
    - No **locks or condition variables** required
        
- **Reason:**  
    There are **no concurrent threads** running simultaneously; the event loop itself ensures mutual exclusion naturally.
    

✅ **Key Benefit:** Avoids all traditional thread-related synchronization bugs.

---

## ⚠️ 33.5 Problem — Blocking System Calls

### 🧱 The Issue:

- Event-based loops are single-threaded.
    
- If a handler makes a **blocking system call**, such as:
    
    - `open()` → to open a file
        
    - `read()` → to read data from disk
        
- Then **the entire event loop halts** until the call finishes.
    

### 🌀 Example Scenario:
```c
Client requests → "GET /index.html"
Server handler → calls open("index.html") → may block (disk access)
Then calls read() → may block again (waiting for I/O)
```
### 🧨 Thread-Based Server:

- Uses **multiple threads**.
    
- While one thread is waiting on disk I/O, others can continue processing.
    
- This allows **natural overlap** between computation and I/O.
    

### ❌ Event-Based Server:

- Only **one event loop** runs.
    
- If it blocks → the **whole system waits**.
    
- Hence the **core rule**:
    
    > “No blocking calls allowed in event-based systems.”
    
## ⚙️ 33.6 Solution — Asynchronous I/O (AIO)

### 💡 Idea:

Use **Asynchronous I/O (AIO)** to issue I/O requests that **return immediately** while the operation completes in the background.

- Application keeps running while I/O happens asynchronously.
    
- Later, it checks or gets notified when the I/O is done.
    

---

### 🧱 The `aiocb` Structure (AIO Control Block)

A simplified form of the control block (used on macOS and UNIX-like systems):
```c
struct aiocb {
    int aio_fildes;            // File descriptor
    off_t aio_offset;          // File offset
    volatile void *aio_buf;    // Buffer location
    size_t aio_nbytes;         // Number of bytes to transfer
};
```
#### 🧩 Steps to Perform an Asynchronous Read:

1. **Fill in the control block:**
    
    - `aio_fildes` → file descriptor
        
    - `aio_offset` → offset in file
        
    - `aio_buf` → memory buffer
        
    - `aio_nbytes` → size of read
        
2. **Issue the asynchronous read:**
Used to determine if the async operation is done:
```c
int aio_read(struct aiocb *aiocbp);
```
- Returns `0` → I/O completed successfully
    
- Returns `EINPROGRESS` → I/O still in progress
    

#### Example:
```c
struct aiocb cb;
/* fill cb with file info, offset, buffer, etc. */
aio_read(&cb);

while (aio_error(&cb) == EINPROGRESS) {
    // keep doing other work while waiting
    // (or poll occasionally)
}
```

### ⚙️ Alternative: Using Interrupts (UNIX Signals)

Instead of polling:

- The OS can send a **signal** when an asynchronous I/O completes.
    
- The signal handler runs and processes the finished I/O.
    
- This avoids wasteful polling and improves efficiency.
    

---

### 🧠 Key Takeaways

|Concept|Description|
|---|---|
|**Problem**|Blocking I/O halts the single-threaded event loop|
|**Solution**|Use **Asynchronous I/O (AIO)** to perform non-blocking file/disk operations|
|**Core Structure**|`struct aiocb` (AIO Control Block)|
|**API**|`aio_read()`, `aio_error()`|
|**Notification Methods**|Polling or Interrupts (signals)|
|**Benefit**|Event loop continues running without waiting for slow I/O|
|**Limitation**|AIO not available everywhere; some systems use hybrid models|