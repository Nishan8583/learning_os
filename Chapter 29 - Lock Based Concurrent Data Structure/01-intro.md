### Key Notes for Quick Review: Lock-based Concurrent Data Structures

#### 1. **Introduction to Lock-based Concurrent Data Structures**

- Locks are added to data structures to make them thread-safe.
- The challenge lies in adding locks to ensure **correctness** and **high performance** for concurrent access by multiple threads.
- This is a well-studied topic with extensive research, e.g., Moir and Shavit’s survey [MS04].

---

#### 2. **Concurrent Counters**

- **Definition**: A counter is a simple data structure with a basic interface, commonly used in programming.
- **Non-concurrent Counter Implementation**:
    - A simple counter without synchronization is easy to implement but not thread-safe.
    - Example code for a non-concurrent counter:

```c
typedef struct __counter_t {
    int value;
} counter_t;

void init(counter_t *c) {
    c->value = 0;
}

void increment(counter_t *c) {
    c->value++;
}

void decrement(counter_t *c) {
    c->value--;
}

int get(counter_t *c) {
    return c->value;
}
```

- **Challenge**: The above implementation is not thread-safe and cannot handle concurrent access.

---

#### 3. **Making Counters Thread-safe**

- To make the counter thread-safe, locks must be added to protect shared data.
- Example of a thread-safe counter using locks:

```c
#include <pthread.h>

typedef struct __counter_t {
    int value;
    pthread_mutex_t lock;
} counter_t;

void init(counter_t *c) {
    c->value = 0;
    pthread_mutex_init(&c->lock, NULL);
}

void increment(counter_t *c) {
    pthread_mutex_lock(&c->lock);
    c->value++;
    pthread_mutex_unlock(&c->lock);
}

void decrement(counter_t *c) {
    pthread_mutex_lock(&c->lock);
    c->value--;
    pthread_mutex_unlock(&c->lock);
}

int get(counter_t *c) {
    pthread_mutex_lock(&c->lock);
    int val = c->value;
    pthread_mutex_unlock(&c->lock);
    return val;
}
```

- **Explanation**:
    - A `pthread_mutex_t` lock is added to the `counter_t` structure.
    - `pthread_mutex_lock()` is used to acquire the lock before modifying the counter.
    - `pthread_mutex_unlock()` is used to release the lock after the operation is complete.
    - This ensures that only one thread can modify the counter at a time, making it thread-safe.

####  **Basic Design of a Concurrent Counter**

- A simple concurrent counter can be created by adding a **single lock** to the data structure.
- The lock is acquired when a routine manipulates the data structure and released when the routine finishes.
- This design is similar to the **monitor pattern** [BH73], where locks are automatically acquired and released when object methods are called and returned.

---

#### 2. **Code Example: A Counter with Locks**

```c
#include <pthread.h>

typedef struct __counter_t {
    int value;
    pthread_mutex_t lock;
} counter_t;

void init(counter_t *c) {
    c->value = 0;
    pthread_mutex_init(&c->lock, NULL);
}

void increment(counter_t *c) {
    pthread_mutex_lock(&c->lock);
    c->value++;
    pthread_mutex_unlock(&c->lock);
}

void decrement(counter_t *c) {
    pthread_mutex_lock(&c->lock);
    c->value--;
    pthread_mutex_unlock(&c->lock);
}

int get(counter_t *c) {
    pthread_mutex_lock(&c->lock);
    int rc = c->value;
    pthread_mutex_unlock(&c->lock);
    return rc;
}
```

---

#### 3. **Performance Considerations**

- While the above implementation is **correct** and ensures thread safety, it may suffer from **performance issues** due to contention on the single lock.
- A **benchmark test** was conducted to measure performance:
    - Each thread updated a shared counter 1 million times.
    - The number of threads was varied from 1 to 4.
- **Results**:
    - A single thread completed 1 million updates in ~0.03 seconds.
    - With 2 threads, the time increased to over 5 seconds.
    - Performance **degraded further** as more threads were added.

---

#### 4. **Key Observations**

- The synchronized counter **does not scale well** with an increasing number of threads.
- **Perfect scaling** is the ideal scenario, where multiple threads complete tasks in parallel without increasing the total time.
- If performance is acceptable with a single lock, no further optimization is needed.
- However, if performance is poor, more advanced techniques (discussed later in the chapter) are required to improve scalability.

---

#### 5. **Key Takeaways**

- A single lock is the simplest way to make a data structure thread-safe.
- While simple, this approach may lead to **poor scalability** due to lock contention.
- For better performance, consider advanced concurrency techniques if the workload involves multiple threads and high contention.

These notes summarize the key concepts, performance considerations, and code example for implementing a basic thread-safe concurrent counter using locks.