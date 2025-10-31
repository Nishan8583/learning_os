
# **Introduction to Concurrency**

- **Problem in Concurrent Programming**:
    - Executing a series of instructions atomically is challenging due to:
        - Interrupts on a single processor.
        - Multiple threads executing on multiple processors concurrently.
    - **Solution**: Use **locks** to ensure critical sections execute atomically.

---

#### **28.1 Locks: The Basic Idea**

- **Critical Section Example**:
    
    ```java
    balance = balance + 1;
    ```
    
    - Critical sections could also involve complex updates like adding elements to a linked list.
- **Using Locks**:
    
    - Add code around the critical section:
        
        ```c
        lock_t mutex; // globally-allocated lock 'mutex'
        ...
        lock(&mutex);
        balance = balance + 1;
        unlock(&mutex);
        ```
        
    - **Lock Variable**:
        - A lock is a variable that holds the state of the lock:
            - **Available (unlocked/free)**: No thread holds the lock.
            - **Acquired (locked/held)**: Exactly one thread holds the lock and is in the critical section.
        - Additional information (e.g., which thread holds the lock, queue for lock acquisition) may also be stored but is hidden from the user.

---

#### **Semantics of `lock()` and `unlock()`**

- **`lock()`**:
    - Tries to acquire the lock:
        - If **free**, the thread acquires the lock and enters the critical section (becomes the **owner** of the lock).
        - If **held**, the thread calling `lock()` will **wait** until the lock is released.
- **`unlock()`**:
    - Releases the lock:
        - If no threads are waiting, the lock state changes to **free**.
        - If threads are waiting, one of them will eventually acquire the lock and enter the critical section.

---

#### **Purpose and Benefits of Locks**

- **Control Over Scheduling**:
    - Threads are usually scheduled by the OS in any arbitrary fashion.
    - Locks give programmers control by ensuring **only one thread** can execute a critical section at a time.
- **Guarantee Atomicity**:
    - Locks ensure that critical sections are executed as if they were a single atomic instruction.
- **Transform Chaos into Control**:
    - Locks help manage the unpredictability of OS scheduling, providing a structured way to handle concurrency.