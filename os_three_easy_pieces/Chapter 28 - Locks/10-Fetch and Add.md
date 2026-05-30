### Key Notes on Fetch-And-Add

---

#### **Definition**

- **Fetch-And-Add**:
    - A hardware-supported atomic instruction that increments a value and returns its old value.
    - **C Pseudocode**:
        
        ```c
        int FetchAndAdd(int *ptr) {
            int old = *ptr;
            *ptr = old + 1;
            return old;
        }
        ```
        
    - **How It Works**:
        1. Reads the current value at `*ptr`.
        2. Atomically increments the value at `*ptr` by 1.
        3. Returns the old value.

---

#### **Building a Ticket Lock with Fetch-And-Add**

- **Concept**:
    
    - Uses two variables: `ticket` and `turn`.
    - **How It Works**:
        - Each thread gets a unique "ticket" when attempting to acquire the lock.
        - The thread waits until its ticket matches the current `turn` to enter the critical section.
        - Guarantees fairness by ensuring threads are served in the order they arrive.
- **Code Example**:
    
    ```c
    typedef struct __lock_t {
        int ticket;
        int turn;
    } lock_t;
    
    void init(lock_t *lock) {
        lock->ticket = 0;
        lock->turn = 0;
    }
    
    void lock(lock_t *lock) {
        int myturn = FetchAndAdd(&lock->ticket);
        while (lock->turn != myturn)
            ; // spin
    }
    
    void unlock(lock_t *lock) {
        lock->turn = lock->turn + 1;
    }
    ```
    
- **Explanation**:
    
    1. **Acquiring the Lock**:
        - A thread calls `FetchAndAdd(&lock->ticket)` to get its ticket number (`myturn`).
        - The thread spins until `lock->turn == myturn`, indicating it is the thread's turn to enter the critical section.
    2. **Releasing the Lock**:
        - The thread increments `lock->turn` to allow the next thread (if any) to acquire the lock.

---

#### **Advantages of Ticket Locks**

1. **Fairness**:
    - Threads are served in the order they request the lock (FIFO order).
    - Guarantees **progress** for all threads: once a thread gets a ticket, it will eventually acquire the lock.
2. **Avoids Starvation**:
    - Unlike spin locks (e.g., test-and-set), no thread can be indefinitely delayed or starved.
3. **Simplicity**:
    - The implementation is straightforward and easy to understand.

---

#### **Disadvantages of Ticket Locks**

1. **Spin-Waiting**:
    - Threads still spin while waiting for their turn, wasting CPU cycles.
2. **Scalability Issues**:
    - On systems with high contention or many CPUs, spinning threads can cause performance degradation due to cache coherence traffic.

---

#### **Lauer’s Law**

- **Principle**:
    - "Less code is better code."
    - Writing concise, clear, and minimal code reduces bugs and improves maintainability.
    - **Quote**: "If the same people had twice as much time, they could produce as good of a system in half the code."

---

#### **Key Takeaways**

- **Fetch-And-Add**:
    - A hardware-supported atomic instruction used to build synchronization primitives like ticket locks.
- **Ticket Locks**:
    - Use `ticket` and `turn` variables to ensure fairness and progress for all threads.
    - Threads are served in the order they request the lock, avoiding starvation.
- **Advantages**:
    - Guarantees fairness and progress.
    - Simple and easy to implement.
- **Disadvantages**:
    - Involves spin-waiting, leading to wasted CPU cycles under contention.
    - May not scale well on systems with many CPUs or high contention.
- **Lauer’s Law**:
    - Emphasizes the importance of writing concise, clear, and minimal code for better systems.