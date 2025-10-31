### Key Notes on Evaluating Spin Locks

---

#### **28.8 Evaluating Spin Locks**

- **Evaluation Axes**:
    1. **Correctness**:
        
        - Spin locks **do provide mutual exclusion**.
        - Only one thread can enter the critical section at a time, ensuring correctness.
    2. **Fairness**:
        
        - Spin locks **do not guarantee fairness**.
        - A waiting thread may **starve** (spin forever) under contention, as there is no guarantee it will eventually acquire the lock.
    3. **Performance**:
        
        - **Single CPU Case**:
            - Performance overhead is high due to **spin-waiting**.
            - If the thread holding the lock is preempted, other threads will spin for the duration of their time slices, wasting CPU cycles.
        - **Multiple CPU Case**:
            - Spin locks perform better when the number of threads is roughly equal to the number of CPUs.
            - Example:
                - Thread A on CPU 1 holds the lock.
                - Thread B on CPU 2 spins while waiting for the lock.
                - If the critical section is short, Thread A quickly releases the lock, and Thread B acquires it without significant delay.

---

#### **Compare-And-Swap Instruction**

- **Definition**:
    
    - A hardware-supported atomic instruction used to build more advanced locking mechanisms.
    - **Code Example**:
        
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
        2. If they are equal, updates `*ptr` to `new`.
        3. Returns the original value at `*ptr`.
- **Use Case**:
    
    - Compare-and-swap is a more powerful atomic operation than test-and-set.
    - It can be used to build locks and other synchronization primitives with better fairness and performance properties.

---

#### **Key Takeaways**

- **Correctness**:
    - Spin locks ensure mutual exclusion but lack fairness guarantees.
- **Fairness**:
    - Simple spin locks may lead to starvation under contention.
- **Performance**:
    - Spin locks are inefficient on single CPUs due to wasted CPU cycles during spin-waiting.
    - On multiple CPUs, spin locks perform better if the critical section is short and the number of threads matches the number of CPUs.
- **Compare-And-Swap**:
    - A hardware-supported atomic instruction that enables the construction of more advanced and efficient locking mechanisms.