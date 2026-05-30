### Key Notes on Semaphores

#### Overview

- Semaphores are synchronization primitives introduced by Edsger Dijkstra.
- They can be used as both **locks** and **condition variables** to solve concurrency problems.
- Semaphores are defined as objects with an integer value that can be manipulated using two primary operations: `sem_wait()` and `sem_post()`.

---

#### **Definition of a Semaphore**

- A semaphore is an integer-based synchronization primitive.
- It is initialized with an integer value that determines its behavior.
- Two main operations:
    1. **`sem_wait()`**: Decrements the semaphore value by 1. If the value becomes negative, the calling thread is blocked until another thread increments the semaphore.
    2. **`sem_post()`**: Increments the semaphore value by 1. If there are threads waiting, one of them is woken up.

---

#### **Code Examples**

1. **Initializing a Semaphore**
    
    ```c
    #include <semaphore.h>
    sem_t s;
    sem_init(&s, 0, 1);
    ```
    
    - `sem_t s`: Declares a semaphore `s`.
    - `sem_init(&s, 0, 1)`: Initializes the semaphore `s` with a value of `1`.
        - The second argument (`0`) indicates that the semaphore is shared between threads in the same process.
2. **Semaphore Wait and Post Operations**
    
    ```c
    int sem_wait(sem_t *s) {
        decrement the value of semaphore s by one
        wait if value of semaphore s is negative
    }
    
    int sem_post(sem_t *s) {
        increment the value of semaphore s by one
        if there are one or more threads waiting, wake one
    }
    ```
    
3. **Using a Binary Semaphore as a Lock**
    
    ```c
    sem_t m;
    sem_init(&m, 0, X); // Initialize semaphore with value X
    sem_wait(&m);       // Enter critical section
    // critical section here
    sem_post(&m);       // Exit critical section
    ```
    

---

#### **Key Characteristics**

1. **Blocking and Waking**:
    
    - `sem_wait()` blocks the thread if the semaphore value is negative.
    - `sem_post()` increments the semaphore value and wakes up one waiting thread if any are present.
2. **Negative Semaphore Values**:
    
    - When the semaphore value is negative, it represents the number of threads waiting to be woken up.
3. **Atomic Operations**:
    
    - The operations of `sem_wait()` and `sem_post()` are atomic, ensuring no race conditions occur during their execution.

---

#### **Binary Semaphore**

- A binary semaphore is a semaphore initialized to `1` and used as a **lock**.
- Example:
    
    ```c
    sem_t m;
    sem_init(&m, 0, 1); // Initialize binary semaphore with value 1
    sem_wait(&m);       // Lock
    // critical section
    sem_post(&m);       // Unlock
    ```
    

---

#### **Historical Context**

- Dijkstra originally named the semaphore operations as `P()` (for "prolaag" or "passering") and `V()` (for "verhoog" or "vrijgave").
- These terms are sometimes referred to as **down** (P) and **up** (V).