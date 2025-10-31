### Key Notes on Evaluating Locks and Controlling Interrupts

---

#### **28.4 Evaluating Locks**

- **Criteria for Evaluating Locks**:
    1. **Mutual Exclusion**:
        - Does the lock prevent multiple threads from entering a critical section simultaneously?
    2. **Fairness**:
        - Does each thread contending for the lock get a fair chance to acquire it?
        - Avoids **starvation**, where a thread never obtains the lock.
    3. **Performance**:
        - **No Contention**: Overhead when a single thread acquires and releases the lock.
        - **Single CPU Contention**: Performance impact when multiple threads contend for the lock on one CPU.
        - **Multiple CPUs Contention**: Performance when threads on different CPUs contend for the lock.

---

#### **28.5 Controlling Interrupts**

- **Early Solution for Mutual Exclusion**:
    
    - Disable interrupts during critical sections to ensure atomic execution.
    - **Code Example**:
        
        ```c
        void lock() {
            DisableInterrupts();
        }
        void unlock() {
            EnableInterrupts();
        }
        ```
        
- **Advantages**:
    
    - **Simplicity**: Easy to understand and implement.
    - Ensures no interruptions during the critical section, making it atomic.
- **Disadvantages**:
    
    1. **Trust Issues**:
        - Requires trusting threads to perform privileged operations (e.g., disabling interrupts).
        - A malicious or buggy thread could:
            - Call `lock()` and monopolize the processor.
            - Enter an infinite loop, causing the OS to lose control, requiring a system restart.
    2. **Incompatibility with Multiprocessors**:
        - Disabling interrupts on one CPU does not prevent threads on other CPUs from entering the critical section.
        - This makes the approach ineffective for multiprocessor systems.
    3. **Interrupt Loss**:
        - Extended periods of disabled interrupts can cause critical interrupts (e.g., disk I/O completion) to be missed, leading to system issues.
- **Limited Use Cases**:
    
    - Used in specific OS contexts to ensure atomicity when accessing internal data structures.
    - Trust issues are mitigated as the OS trusts itself to perform privileged operations.

---

#### **First Attempt: A Simple Flag**

- **Implementation**:
    
    - Use a flag to indicate whether the lock is held.
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
        - Continuously checks (`spin-waits`) if the lock is held (`flag == 1`).
        - Once available, sets the flag to 1 to acquire the lock.
    - **`unlock()`**: Sets the flag to 0 to release the lock.
- **Limitations**:
    
    - **Spin-Waiting**:
        - Inefficient as the thread continuously checks the flag, wasting CPU cycles.
    - **Does Not Scale**:
        - Not suitable for multiprocessor systems or high contention scenarios.