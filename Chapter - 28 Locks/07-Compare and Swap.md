### Key Notes on Compare-And-Swap

---

#### **28.9 Compare-And-Swap**

- **Definition**:
    - A hardware-supported atomic instruction used for synchronization.
    - Known as **compare-and-swap** (SPARC) or **compare-and-exchange** (x86).
    - **C Pseudocode**:
        
        ```c
        int CompareAndSwap(int *ptr, int expected, int new) {
            int original = *ptr;
            if (original == expected)
                *ptr = new;
            return original;
        }
        ```
        
    - **How It Works**:
        1. Compares the value at `*ptr` with `expected`.
        2. If they are equal:
            - Updates `*ptr` to `new`.
        3. If they are not equal:
            - Does nothing.
        4. Returns the original value at `*ptr` to indicate success or failure.

---

#### **Building a Spin Lock with Compare-And-Swap**

- **Code Example**:
    
    ```c
    void lock(lock_t *lock) {
        while (CompareAndSwap(&lock->flag, 0, 1) == 1)
            ; // spin
    }
    ```
    
    - **How It Works**:
        - Checks if `lock->flag` is `0` (lock is free).
        - If `flag == 0`, atomically sets `flag = 1` to acquire the lock.
        - If `flag == 1`, the thread spins until the lock is released.
    - **Unlocking**:
        - The `unlock()` function remains the same as in the test-and-set implementation:
            
            ```c
            void unlock(lock_t *lock) {
                lock->flag = 0;
            }
            ```
            

---

#### **Comparison with Test-And-Set**

- **Similarities**:
    - Both instructions can be used to build spin locks.
    - Behavior of spin locks built with compare-and-swap is identical to those built with test-and-set.
- **Differences**:
    - **Compare-And-Swap** is more powerful than test-and-set.
    - It allows for more advanced synchronization mechanisms, such as **lock-free synchronization**.

---

#### **Applications of Compare-And-Swap**

- **Lock-Free Synchronization**:
    - Compare-and-swap enables the development of advanced synchronization techniques that do not rely on traditional locks.
    - These techniques are useful in high-performance systems where minimizing contention and overhead is critical.

---

#### **Key Takeaways**

- **Compare-And-Swap**:
    - A versatile and powerful atomic instruction for synchronization.
    - Can be used to build spin locks and more advanced synchronization mechanisms.
- **Spin Locks**:
    - Spin locks built with compare-and-swap behave similarly to those built with test-and-set.
    - Threads spin while waiting for the lock to be released, which can lead to inefficiencies under contention.
- **Future Use**:
    - Compare-and-swap's power will be leveraged in advanced topics like **lock-free synchronization**.