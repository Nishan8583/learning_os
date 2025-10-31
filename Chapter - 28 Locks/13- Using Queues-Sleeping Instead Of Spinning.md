### Key Notes on Using Queues: Sleeping Instead of Spinning

---

#### **28.14 Using Queues for Lock Management**

- **Problem with Previous Approaches**:
    - **Spinning**: Threads waste CPU cycles waiting for a lock.
    - **Yielding**: Reduces spinning but incurs high context switch overhead and does not prevent starvation.
    - Both approaches leave scheduling to the OS, which may make poor decisions, leading to inefficiency and starvation.

---

#### **Queue-Based Lock Implementation**

- **Code Example**:
    
    ```c
    typedef struct __lock_t {
        int flag;
        int guard;
        queue_t *q;
    } lock_t;
    
    void lock_init(lock_t *m) {
        m->flag = 0;
        m->guard = 0;
        queue_init(m->q);
    }
    
    void lock(lock_t *m) {
        while (TestAndSet(&m->guard, 1) == 1)
            ; // acquire guard lock by spinning
        if (m->flag == 0) {
            m->flag = 1; // lock is acquired
            m->guard = 0;
        } else {
            queue_add(m->q, gettid());
            m->guard = 0;
            park(); // put thread to sleep
        }
    }
    
    void unlock(lock_t *m) {
        while (TestAndSet(&m->guard, 1) == 1)
            ; // acquire guard lock by spinning
        if (queue_empty(m->q))
            m->flag = 0; // release lock if no waiting threads
        else
            unpark(queue_remove(m->q)); // wake up next thread
        m->guard = 0;
    }
    ```
    

---

#### **How It Works**

1. **Acquiring the Lock**:
    - A thread spins briefly to acquire the **guard lock** (using `TestAndSet`).
    - If the lock is free (`flag == 0`), the thread sets `flag = 1` and acquires the lock.
    - If the lock is held (`flag == 1`):
        - The thread adds itself to the **queue** of waiting threads.
        - Releases the guard lock and calls `park()` to sleep until the lock is available.
2. **Releasing the Lock**:
    - A thread spins briefly to acquire the **guard lock**.
    - If the queue is empty, it sets `flag = 0` to release the lock.
    - If the queue is not empty, it dequeues the next thread and calls `unpark()` to wake it up.
    - Releases the guard lock.

---

#### **Key Features**

1. **Guard Lock**:
    - A spin lock used to protect access to the `flag` and `queue` variables.
    - Ensures that only one thread can manipulate the lock's internal state at a time.
2. **Queue**:
    - Tracks threads waiting for the lock.
    - Ensures fairness by waking threads in the order they arrived.
3. **Park and Unpark**:
    - **`park()`**: Puts the calling thread to sleep until explicitly woken up.
    - **`unpark(threadID)`**: Wakes up a specific thread by its thread ID.

---

#### **Advantages**

1. **Reduces Spinning**:
    - Threads sleep instead of spinning, reducing CPU waste.
2. **Fairness**:
    - Threads are served in the order they arrive, avoiding starvation.
3. **Efficiency**:
    - Threads only spin briefly to acquire the guard lock, minimizing wasted CPU cycles.

---

#### **Disadvantages**

1. **Wakeup/Waiting Race**:
    - A thread may call `park()` just after another thread releases the lock, causing it to sleep indefinitely.
    - **Solution**:
        - Use `setpark()` to indicate that a thread is about to park. If interrupted before calling `park()`, the subsequent `park()` returns immediately.
        - Alternatively, pass the guard lock to the kernel for atomic release and thread wakeup.
2. **Priority Inversion**:
    - A high-priority thread may be blocked by a low-priority thread holding the lock.
    - **Solution**:
        - Implement **priority inheritance**, where the priority of the lock-holding thread is temporarily boosted to match the highest-priority waiting thread.

---

#### **Example Scenarios**

1. **Single Thread Waiting**:
    - **Thread A** holds the lock and is interrupted.
    - **Thread B** tries to acquire the lock, adds itself to the queue, and calls `park()`.
    - When **Thread A** releases the lock, it calls `unpark()` to wake **Thread B**, which acquires the lock.
    - **Result**: No spinning, minimal CPU waste.
2. **Multiple Threads Waiting**:
    - 100 threads contend for a lock.
    - Threads are added to the queue in order and put to sleep using `park()`.
    - When the lock is released, threads are woken up in FIFO order using `unpark()`.
    - **Result**: Fairness is ensured, and CPU waste is minimized.

---

#### **Priority Inversion**

- **Definition**:
    - Occurs when a high-priority thread is blocked waiting for a lock held by a lower-priority thread.
    - If a medium-priority thread starts running, it can prevent the low-priority thread from releasing the lock, causing the high-priority thread to wait indefinitely.
- **Example**:
    - **T1** (low priority) acquires the lock.
    - **T3** (high priority) tries to acquire the lock but is blocked.
    - **T2** (medium priority) runs instead of **T1**, preventing **T1** from releasing the lock.
    - **Result**: **T3** is stuck waiting for **T1**, even though it has the highest priority.
- **Solutions**:
    1. **Priority Inheritance**:
        - Temporarily boost the priority of the lock-holding thread to match the highest-priority waiting thread.
    2. **Equal Priority**:
        - Ensure all threads have the same priority to avoid inversion.

---

#### **Key Takeaways**

- **Queue-Based Locks**:
    - Use a queue to track waiting threads and ensure fairness.
    - Combine hardware support (e.g., `TestAndSet`) with OS support (e.g., `park()` and `unpark()`) to reduce spinning and improve efficiency.
- **Advantages**:
    - Reduces CPU waste by putting threads to sleep instead of spinning.
    - Ensures fairness and avoids starvation.
- **Disadvantages**:
    - Requires OS support for `park()` and `unpark()`.
    - May still involve brief spinning to acquire the guard lock.
    - Vulnerable to priority inversion, which can be mitigated with priority inheritance.
- **Wakeup/Waiting Race**:
    - Use `setpark()` to prevent threads from sleeping indefinitely due to race conditions.
- **Next Steps**:
    - Explore further optimizations and advanced locking mechanisms to address remaining inefficiencies and edge cases.