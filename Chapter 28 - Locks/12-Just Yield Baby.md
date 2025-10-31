### Key Notes on A Simple Approach: Just Yield, Baby

---

#### **28.13 A Simple Approach: Just Yield**

- **Problem**:
    - When a thread holding a lock is interrupted, other threads spin endlessly waiting for the lock to be released, wasting CPU cycles.
    - The goal is to reduce this inefficiency by avoiding unnecessary spinning.

---

#### **Yield-Based Lock Implementation**

- **Code Example**:
    
    ```c
    void init() {
        flag = 0;
    }
    
    void lock() {
        while (TestAndSet(&flag, 1) == 1)
            yield(); // give up the CPU
    }
    
    void unlock() {
        flag = 0;
    }
    ```
    
- **How It Works**:
    
    1. **Acquiring the Lock**:
        - A thread calls `lock()` and executes `TestAndSet(&flag, 1)`.
        - If the lock is free (`flag == 0`), the thread sets `flag = 1` and acquires the lock.
        - If the lock is held (`flag == 1`), the thread calls `yield()` to give up the CPU and allow another thread to run.
    2. **Releasing the Lock**:
        - The thread calls `unlock()` to set `flag = 0`, making the lock available.

---

#### **What is `yield()`?**

- **Definition**:
    - A system call that moves the calling thread from the **running** state to the **ready** state.
    - Allows the operating system to schedule another thread to run.
- **Thread States**:
    - **Running**: The thread is actively executing on the CPU.
    - **Ready**: The thread is ready to run but waiting for CPU time.
    - **Blocked**: The thread is waiting for an event (e.g., I/O or lock release).

---

#### **Advantages of Yield-Based Locks**

1. **Reduces Spinning**:
    - Instead of wasting CPU cycles spinning, threads yield the CPU to other threads.
    - Particularly effective in scenarios with two threads on a single CPU.
2. **Improves Efficiency**:
    - Reduces the waste of CPU time compared to pure spin-waiting.

---

#### **Disadvantages of Yield-Based Locks**

1. **Context Switch Overhead**:
    - Yielding involves a context switch, which can be costly.
    - In scenarios with many threads (e.g., 100 threads contending for a lock):
        - Each thread yields the CPU, leading to a large number of context switches.
        - This "run-and-yield" pattern still results in significant inefficiency.
2. **Starvation**:
    - A thread may get stuck in an endless yield loop if other threads repeatedly acquire and release the lock before it gets a chance to run.
    - Yield-based locks do not address starvation directly.

---

#### **Example Scenarios**

1. **Two Threads on One CPU**:
    - **Thread 0** holds the lock and is interrupted.
    - **Thread 1** tries to acquire the lock, finds it held, and calls `yield()`.
    - The CPU schedules **Thread 0**, which releases the lock, allowing **Thread 1** to acquire it.
    - **Result**: Yielding works well in this simple case.
2. **Many Threads Contending for a Lock**:
    - 100 threads contend for a lock.
    - If one thread acquires the lock and is preempted, the other 99 threads repeatedly call `yield()`.
    - **Result**: Although better than spinning, this approach still incurs high context switch overhead and does not prevent starvation.

---

#### **Key Takeaways**

- **Yield-Based Locks**:
    - Use the `yield()` system call to reduce CPU waste by descheduling threads that cannot acquire a lock.
    - Effective in simple scenarios (e.g., two threads on one CPU) but inefficient under high contention.
- **Advantages**:
    - Reduces spinning and improves efficiency compared to pure spin locks.
- **Disadvantages**:
    - Context switch overhead remains significant.
    - Does not address starvation, as threads can still get stuck in an endless yield loop.
- **Next Steps**:
    - Explore more advanced locking mechanisms that combine hardware and OS support to address both inefficiency and starvation.