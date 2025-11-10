### Key Notes: Adding Mutual Exclusion and Avoiding Deadlock

#### **1. Problem: Lack of Mutual Exclusion**

- The filling of a buffer and incrementing the index into the buffer is a **critical section**.
- Critical sections must be **guarded carefully** to avoid race conditions.
- **Binary semaphores** (mutex) are used to ensure mutual exclusion.

---

#### **2. Initial Attempt: Adding Locks**

- Locks were added around the entire `put()` and `get()` parts of the code.
- **Code Example:**

```c
void *producer(void *arg) {
    int i;
    for (i = 0; i < loops; i++) {
        sem_wait(&empty); // Line P1
        sem_wait(&mutex); // Line P1.5 (lock)
        put(i);           // Line P2
        sem_post(&mutex); // Line P2.5 (unlock)
        sem_post(&full);  // Line P3
    }
}

void *consumer(void *arg) {
    int i;
    for (i = 0; i < loops; i++) {
        sem_wait(&full);   // Line C1
        sem_wait(&mutex);  // Line C1.5 (lock)
        int tmp = get();   // Line C2
        sem_post(&mutex);  // Line C2.5 (unlock)
        sem_post(&empty);  // Line C3
        printf("%d\n", tmp);
    }
}
```

- **Issue:** This approach leads to **deadlock**.

---

#### **3. Why Deadlock Occurs**

- **Scenario:**
    - A consumer thread runs first, acquires the mutex (Line C1.5), and calls `sem_wait(&full)` (Line C1). Since there is no data, the consumer blocks but **still holds the mutex**.
    - A producer thread then runs and calls `sem_wait(&mutex)` (Line P1.5). However, the mutex is already held by the consumer, so the producer blocks.
    - **Result:** Both threads are stuck waiting for each other, creating a **deadlock**.

---

#### **4. Solution: Reducing the Scope of the Lock**

- To avoid deadlock, **reduce the scope of the lock**.
- Move the `mutex` acquire (`sem_wait(&mutex)`) and release (`sem_post(&mutex)`) **inside the critical section**.
- The `full` and `empty` semaphore operations are left **outside** the critical section.

---

#### **5. Correct Implementation**

- **Code Example:**

```c
void *producer(void *arg) {
    int i;
    for (i = 0; i < loops; i++) {
        sem_wait(&empty); // Line P1
        sem_wait(&mutex); // Line P1.5 (lock)
        put(i);           // Line P2
        sem_post(&mutex); // Line P2.5 (unlock)
        sem_post(&full);  // Line P3
    }
}

void *consumer(void *arg) {
    int i;
    for (i = 0; i < loops; i++) {
        sem_wait(&full);   // Line C1
        sem_wait(&mutex);  // Line C1.5 (lock)
        int tmp = get();   // Line C2
        sem_post(&mutex);  // Line C2.5 (unlock)
        sem_post(&empty);  // Line C3
        printf("%d\n", tmp);
    }
}
```

---

#### **6. Key Takeaways**

- **Deadlock Cause:** Both producer and consumer threads can block each other if the mutex is held while waiting for a semaphore.
- **Solution:** Reduce the scope of the mutex lock to only the critical section (e.g., `put()` and `get()` operations).
- **Bounded Buffer Pattern:** This is a common and effective pattern in multithreaded programming. It ensures proper synchronization and avoids deadlock.
- **Important Note:** Always analyze the sequence of operations to identify potential deadlocks in multithreaded programs.