
### Key Notes on Building Working Spin Locks with Test-And-Set

---

#### **28.7 Building Spin Locks with Test-And-Set**

- **Why Test-And-Set?**
    - Disabling interrupts does not work on multiprocessors.
    - Simple approaches using loads and stores fail to ensure mutual exclusion.
    - Hardware support, such as the **test-and-set instruction**, was introduced to build proper locks.
    - Test-and-set is available on all modern systems, even single CPU systems.

---

#### **Test-And-Set Instruction**

- **Definition**:
    - Atomically performs two operations:
        1. Tests the old value of a memory location.
        2. Sets the memory location to a new value.
    - **Code Example**:
        
        ```c
        int TestAndSet(int *old_ptr, int new) {
            int old = *old_ptr; // fetch old value at old_ptr
            *old_ptr = new;    // store 'new' into old_ptr
            return old;        // return the old value
        }
        ```
        
    - **Key Feature**: The test and set operation is **atomic**, ensuring no other thread can interfere during its execution.

---

#### **Spin Lock Implementation Using Test-And-Set**

- **Code Example**:
    
    ```c
    typedef struct __lock_t {
        int flag;
    } lock_t;
    
    void init(lock_t *lock) {
        // 0: lock is available, 1: lock is held
        lock->flag = 0;
    }
    
    void lock(lock_t *lock) {
        while (TestAndSet(&lock->flag, 1) == 1)
            ; // spin-wait (do nothing)
    }
    
    void unlock(lock_t *lock) {
        lock->flag = 0;
    }
    ```
    
- **How It Works**:
    
    1. **Acquiring the Lock**:
        - A thread calls `lock()` and executes `TestAndSet(&lock->flag, 1)`.
        - If `flag == 0` (lock is free):
            - `TestAndSet()` returns 0.
            - The thread sets `flag = 1` and acquires the lock.
        - If `flag == 1` (lock is held):
            - `TestAndSet()` returns 1.
            - The thread spin-waits until the lock is released.
    2. **Releasing the Lock**:
        - The thread calls `unlock()` to set `flag = 0`, making the lock available.

---

#### **Why This Lock Works**

- **Atomicity**:
    
    - The test (checking the old value) and set (updating the new value) are performed as a single atomic operation.
    - This ensures that only one thread can acquire the lock at a time, providing **mutual exclusion**.
- **Two Scenarios**:
    
    1. **Lock is Free**:
        - `TestAndSet()` returns 0, and the thread acquires the lock.
    2. **Lock is Held**:
        - `TestAndSet()` returns 1, and the thread spin-waits until the lock is released.

---

#### **Advantages of Spin Locks**

- **Simplicity**:
    - Easy to implement and understand.
- **Hardware Support**:
    - Relies on atomic hardware instructions, which are widely available.

---

#### **Disadvantages of Spin Locks**

1. **Spin-Waiting**:
    - Threads waste CPU cycles while waiting for the lock to be released.
2. **Single Processor Limitation**:
    - On a single CPU, spin locks require a **preemptive scheduler** to interrupt spinning threads and allow the lock-holding thread to execute.
    - Without preemption, spin locks are ineffective on single CPUs.
3. **High Contention**:
    - Spin locks are inefficient under high contention, as multiple threads waste CPU cycles spinning.

---

#### **Peterson’s Algorithm (Historical Context)**

- **Overview**:
    
    - A software-based locking algorithm for two threads using only loads and stores.
    - **Code Example**:
        
        ```c
        int flag[2];
        int turn;
        
        void init() {
            flag[0] = flag[1] = 0; // no thread intends to hold the lock
            turn = 0;              // thread 0's turn
        }
        
        void lock() {
            flag[self] = 1;        // indicate intent to hold the lock
            turn = 1 - self;       // give the other thread priority
            while (flag[1-self] == 1 && turn == 1 - self)
                ; // spin-wait
        }
        
        void unlock() {
            flag[self] = 0;        // release the lock
        }
        ```
        
- **How It Works**:
    
    - **`flag[]`**: Indicates whether a thread intends to hold the lock.
    - **`turn`**: Indicates which thread has priority to acquire the lock.
    - Ensures mutual exclusion by combining intent (`flag[]`) and priority (`turn`).
- **Limitations**:
    
    - Works only for two threads.
    - Relies on atomicity of loads and stores, which is not guaranteed on modern hardware.
    - Inefficient and largely obsolete due to the availability of hardware support like test-and-set.

---

#### **Key Takeaways**

- **Test-And-Set**:
    - A hardware-supported atomic instruction that enables the creation of simple and effective spin locks.
    - Ensures mutual exclusion by atomically testing and setting a lock variable.
- **Spin Locks**:
    - Simple to implement but inefficient under high contention or on single CPUs without preemption.
- **Peterson’s Algorithm**:
    - A historical software-based locking solution for two threads, now largely obsolete due to hardware advancements.
- **Concurrency Debugging Tip**:
    - Think like a **malicious scheduler** to identify potential race conditions and flaws in synchronization primitives.