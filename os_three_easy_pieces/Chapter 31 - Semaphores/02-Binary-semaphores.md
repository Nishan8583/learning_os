### Key Notes on Binary Semaphores (Locks)

#### Overview

- A **binary semaphore** is a semaphore used as a lock, with only two states: held (0) and not held (1).
- It is used to protect critical sections in concurrent programming by ensuring mutual exclusion.

#### Key Concepts

1. **Initialization**:
    
    - The semaphore must be initialized to `1` to function as a lock.
    - This ensures that the first thread can acquire the lock without waiting.
2. **Critical Section Protection**:
    
    - Surround the critical section with `sem_wait()` and `sem_post()` calls.
    - `sem_wait()` decrements the semaphore value:
        - If the value is greater than or equal to 0, the thread continues execution.
        - If the value is less than 0, the thread is put to sleep (blocked).
    - `sem_post()` increments the semaphore value:
        - If there are waiting threads, one is woken up.
        - If no threads are waiting, the semaphore value is simply incremented.

#### Example Scenarios

1. **Single Thread Using a Semaphore**:
    
    - Thread 0 calls `sem_wait()`, decrementing the semaphore value to `0` and enters the critical section.
    - After completing the critical section, Thread 0 calls `sem_post()`, restoring the semaphore value to `1`.
    
    **Thread Trace**:
    
    |Value of Semaphore|Thread 0 State|Thread 1 State|
    |---|---|---|
    |1|Run|Ready|
    |1|`sem_wait()`|Ready|
    |0|Critical Section|Ready|
    |0|`sem_post()`|Ready|
    |1|Run|Ready|
    
2. **Two Threads Using a Semaphore**:
    
    - Thread 0 acquires the lock by calling `sem_wait()` (semaphore value becomes `0`) and enters the critical section.
    - Thread 1 tries to acquire the lock by calling `sem_wait()`, but since the semaphore value is `0`, it decrements the value to `-1` and goes to sleep.
    - When Thread 0 finishes and calls `sem_post()`, the semaphore value is incremented to `0`, and Thread 1 is woken up to acquire the lock.
    
    **Thread Trace**:
    
    |Value of Semaphore|Thread 0 State|Thread 1 State|
    |---|---|---|
    |1|Run|Ready|
    |1|`sem_wait()`|Ready|
    |0|Critical Section|Ready|
    |0|Interrupt; Switch to T1|Run|
    |0|Ready|`sem_wait()`|
    |-1|Ready|Sleep|
    |-1|Run|Sleep|
    |-1|Critical Section End|Sleep|
    |-1|`sem_post()`|Sleep|
    |0|Run|Ready|
    |0|Interrupt; Switch to T1|Run|
    |0|Ready|`sem_wait()`|
    |0|Ready|Critical Section|
    |0|Ready|`sem_post()`|
    |1|Ready|Run|
    

#### Key Takeaways

- **Binary Semaphore**:
    - Used as a lock to ensure mutual exclusion.
    - Initial value must be `1`.
    - Only two states: held (0) and not held (1).
- **Thread Behavior**:
    - A thread calling `sem_wait()` decrements the semaphore value.
    - If the semaphore value is less than `0`, the thread is blocked (put to sleep).
    - A thread calling `sem_post()` increments the semaphore value and wakes up a waiting thread if any.
- **Scheduler States**:
    - **Run**: Thread is actively running.
    - **Ready**: Thread is ready to run but not currently running.
    - **Sleep**: Thread is blocked and waiting for the semaphore.

#### Additional Notes

- Binary semaphores can be implemented in a simpler manner than generalized semaphores since they only have two states.
- For more complex scenarios, such as multiple threads queuing for a lock, the semaphore value will decrease further into negative values as more threads are blocked. Each `sem_post()` will increment the value and wake up one waiting thread.