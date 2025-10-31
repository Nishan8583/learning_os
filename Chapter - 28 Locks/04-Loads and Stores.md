### Key Notes on A Failed Attempt: Just Using Loads/Stores

---

#### **28.6 A Failed Attempt: Just Using a Single Flag**

- **Concept**:
    
    - Use a single variable (`flag`) to indicate whether a lock is held.
    - **Code Example**:
        
        ```c
        typedef struct __lock_t { int flag; } lock_t;
        
        void init(lock_t *mutex) {
            // 0 -> lock is available, 1 -> held
            mutex->flag = 0;
        }
        
        void lock(lock_t *mutex) {
            while (mutex->flag == 1) // TEST the flag
                ; // spin-wait (do nothing)
            mutex->flag = 1; // now SET it!
        }
        
        void unlock(lock_t *mutex) {
            mutex->flag = 0;
        }
        ```
        
- **How It Works**:
    
    - **`init()`**: Initializes the lock to be available (`flag = 0`).
    - **`lock()`**:
        - Checks if the lock is held (`flag == 1`).
        - If not held, sets the flag to 1 to acquire the lock.
        - If held, spin-waits (continuously checks the flag) until the lock is released.
    - **`unlock()`**: Sets the flag to 0 to release the lock.

---

#### **Problems with This Approach**

1. **Correctness Issue**:
    
    - **No Mutual Exclusion**:
        - The lock fails to ensure that only one thread can enter the critical section at a time.
        - **Interleaving Example**:
            - Assume `flag = 0` initially.
            - **Thread 1** calls `lock()` and checks `flag == 1` (false), then gets interrupted.
            - **Thread 2** calls `lock()` and checks `flag == 1` (still false), then sets `flag = 1` and enters the critical section.
            - **Thread 1** resumes, sets `flag = 1` again, and also enters the critical section.
            - **Result**: Both threads are in the critical section simultaneously, violating mutual exclusion.
            - **Trace**:
                
                ```
                Thread 1: call lock()
                Thread 1: while (flag == 1) // false
                [Interrupt: switch to Thread 2]
                Thread 2: call lock()
                Thread 2: while (flag == 1) // false
                Thread 2: flag = 1
                [Interrupt: switch to Thread 1]
                Thread 1: flag = 1 // sets flag to 1 again
                ```
                
2. **Performance Issue**:
    
    - **Spin-Waiting**:
        - A thread waiting for the lock continuously checks the value of `flag` in a loop.
        - **Wasted CPU Cycles**:
            - On a **uniprocessor**, the thread holding the lock cannot run until a context switch occurs, making the waiting thread's spin-waiting completely wasteful.
            - On a **multiprocessor**, spin-waiting still wastes CPU cycles that could be used for other tasks.

---

#### **Key Takeaways**

- **Correctness Problem**:
    - The lock implementation fails to ensure mutual exclusion due to race conditions caused by concurrent access to the `flag` variable.
- **Performance Problem**:
    - Spin-waiting is inefficient, especially on uniprocessor systems, as it wastes CPU cycles waiting for the lock to be released.
- **Conclusion**:
    - Using a single flag variable with normal loads and stores is insufficient for building a correct and efficient lock.
    - More sophisticated techniques are needed to address both correctness and performance issues.