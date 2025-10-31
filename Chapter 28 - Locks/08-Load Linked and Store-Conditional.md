### Key Notes on Load-Linked and Store-Conditional

---

#### **Definition**

- **Load-Linked (LL)** and **Store-Conditional (SC)**:
    - A pair of atomic instructions provided by some architectures (e.g., MIPS, Alpha, PowerPC, ARM) to build locks and other concurrent structures.
    - **C Pseudocode**:
        
        ```c
        int LoadLinked(int *ptr) {
            return *ptr;
        }
        
        int StoreConditional(int *ptr, int value) {
            if (no update to *ptr since LL to this addr) {
                *ptr = value;
                return 1; // success!
            } else {
                return 0; // failed to update
            }
        }
        ```
        
    - **How It Works**:
        1. **Load-Linked (LL)**:
            - Reads a value from memory and places it in a register.
            - Tracks the memory location for subsequent operations.
        2. **Store-Conditional (SC)**:
            - Updates the memory location only if no other thread has modified it since the last load-linked operation.
            - Returns:
                - `1` on success (value updated).
                - `0` on failure (value not updated).

---

#### **Building a Lock with Load-Linked and Store-Conditional**

- **Code Example**:
    
    ```c
    void lock(lock_t *lock) {
        while (1) {
            while (LoadLinked(&lock->flag) == 1)
                ; // spin until it’s zero
            if (StoreConditional(&lock->flag, 1) == 1)
                return; // success: lock acquired
            // otherwise: retry
        }
    }
    
    void unlock(lock_t *lock) {
        lock->flag = 0;
    }
    ```
    
- **How It Works**:
    
    1. **Acquiring the Lock**:
        - The thread spins until `LoadLinked(&lock->flag)` returns `0` (indicating the lock is free).
        - The thread then attempts to set `lock->flag = 1` using `StoreConditional()`.
        - If successful, the thread acquires the lock and exits the loop.
        - If `StoreConditional()` fails (another thread modified the flag), the thread retries.
    2. **Releasing the Lock**:
        - The `unlock()` function sets `lock->flag = 0`, making the lock available.

---

#### **Shortened Lock Implementation**

- **Concise Version**:
    
    ```c
    void lock(lock_t *lock) {
        while (LoadLinked(&lock->flag) || !StoreConditional(&lock->flag, 1))
            ; // spin
    }
    ```
    
    - **Explanation**:
        - Combines the spinning and retry logic into a single condition.
        - Equivalent to the longer version but more compact.

---

#### **Why It Works**

- **Atomicity**:
    - The combination of `LoadLinked` and `StoreConditional` ensures that only one thread can successfully update the lock's flag at a time.
- **Failure Handling**:
    - If a thread fails the `StoreConditional` step (because another thread modified the flag), it retries the process.

---

#### **Advantages**

1. **Mutual Exclusion**:
    - Ensures only one thread can acquire the lock at a time.
2. **Efficiency**:
    - Avoids unnecessary updates to the lock variable.
    - Reduces contention compared to simpler spin locks (e.g., test-and-set).
3. **Hardware Support**:
    - Relies on hardware-provided atomicity for correctness.

---

#### **Disadvantages**

1. **Spin-Waiting**:
    - Threads still spin while waiting for the lock, wasting CPU cycles.
2. **Complexity**:
    - Requires specific hardware support for `LoadLinked` and `StoreConditional`.
3. **Limited Use Cases**:
    - Not universally available on all architectures.

---

#### **Key Takeaways**

- **Load-Linked and Store-Conditional**:
    - A pair of atomic instructions that work together to ensure mutual exclusion.
    - Used to build efficient locks and other synchronization primitives.
- **Lock Implementation**:
    - Threads spin until they successfully acquire the lock using `StoreConditional`.
- **Concise Version**:
    - A shorter implementation combines spinning and retry logic into a single condition.
- **Advantages**:
    - Provides mutual exclusion with reduced contention compared to simpler spin locks.
- **Disadvantages**:
    - Still involves spin-waiting and requires specific hardware support.