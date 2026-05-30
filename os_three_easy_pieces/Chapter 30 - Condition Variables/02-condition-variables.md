### Key Notes on Condition Variables

---

#### **Definition and Purpose**

- **Condition Variable**: An explicit queue that threads can use to wait for a condition to become true.
    - Threads can **wait** on a condition when the desired state is not met.
    - Other threads can **signal** the condition when the state changes, waking up waiting threads.
- Origin:
    - Concept introduced by Dijkstra as "private semaphores."
    - Later named "condition variables" by Hoare in his work on monitors.

---

#### **Key Operations**

1. **`pthread_cond_wait(pthread_cond_t *c, pthread_mutex_t *m)`**:
    - Puts the calling thread to sleep until the condition is signaled.
    - Assumes the mutex `m` is locked when called.
    - Atomically releases the lock and puts the thread to sleep.
    - Re-acquires the lock before returning.
2. **`pthread_cond_signal(pthread_cond_t *c)`**:
    - Wakes up one thread waiting on the condition variable `c`.

---

#### **Example: Parent Waiting for Child**

**Code Example: Using Condition Variables**

```c
int done = 0;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;

void thr_exit() {
    Pthread_mutex_lock(&m);
    done = 1;
    Pthread_cond_signal(&c);
    Pthread_mutex_unlock(&m);
}

void *child(void *arg) {
    printf("child\n");
    thr_exit();
    return NULL;
}

void thr_join() {
    Pthread_mutex_lock(&m);
    while (done == 0)
        Pthread_cond_wait(&c, &m);
    Pthread_mutex_unlock(&m);
}

int main(int argc, char *argv[]) {
    printf("parent: begin\n");
    pthread_t p;
    Pthread_create(&p, NULL, child, NULL);
    thr_join();
    printf("parent: end\n");
    return 0;
}
```

**Explanation**:

1. **Parent Thread**:
    - Calls `thr_join()` after creating the child thread.
    - Acquires the lock and checks the `done` variable.
    - If `done == 0`, it calls `pthread_cond_wait()` to release the lock and sleep until signaled.
2. **Child Thread**:
    - Executes and calls `thr_exit()` after completing its task.
    - Acquires the lock, sets `done = 1`, signals the condition variable, and releases the lock.
3. **Parent Thread (Resumes)**:
    - Wakes up when signaled, re-acquires the lock, and checks the condition.
    - Unlocks the lock and continues execution.

**Output**:

```
parent: begin
child
parent: end
```

---

#### **Key Concepts**

1. **State Variable**:
    - The `done` variable is crucial to track the condition's state.
    - Without it, a thread might signal before another thread starts waiting, leading to deadlocks.
2. **While Loop in `wait()`**:
    - Always use a `while` loop instead of an `if` statement to check the condition.
    - Prevents issues with spurious wakeups or race conditions.

---

#### **Common Mistakes**

1. **No State Variable**:
    - If the state variable (`done`) is omitted, a thread might signal before another thread starts waiting, causing the waiting thread to sleep forever.

**Incorrect Code Example: No State Variable**

```c
void thr_exit() {
    Pthread_mutex_lock(&m);
    Pthread_cond_signal(&c);
    Pthread_mutex_unlock(&m);
}

void thr_join() {
    Pthread_mutex_lock(&m);
    Pthread_cond_wait(&c, &m);
    Pthread_mutex_unlock(&m);
}
```

**Issue**:

- If the child signals before the parent starts waiting, the parent will sleep forever.

---

2. **No Locking**:
    - If the lock is not held during `signal()` or `wait()`, race conditions can occur.

**Incorrect Code Example: No Lock**

```c
void thr_exit() {
    done = 1;
    Pthread_cond_signal(&c);
}

void thr_join() {
    if (done == 0)
        Pthread_cond_wait(&c);
}
```

**Issue**:

- If the parent checks `done` and is interrupted before calling `wait()`, the child may signal while the parent is not waiting, causing the parent to sleep forever.

---

#### **Best Practices**

1. **Always Hold the Lock**:
    - Hold the lock when calling `signal()` or `wait()` to ensure correctness.
    - While holding the lock during `signal()` is not always necessary, it is a good practice to avoid subtle bugs.
2. **Use a State Variable**:
    - Always use a state variable (e.g., `done`) to track the condition's state.
    - The state variable ensures that the condition is checked correctly, even if the signal is sent before the wait.

---

#### **Producer-Consumer Example**

**Code Example: Put and Get Routines**

```c
int buffer;
int count = 0; // initially, empty

void put(int value) {
    assert(count == 0);
    count = 1;
    buffer = value;
}

int get() {
    assert(count == 1);
    count = 0;
    return buffer;
}
```

**Explanation**:

- **`put()`**: Adds a value to the buffer and sets `count = 1` to indicate the buffer is full.
- **`get()`**: Retrieves the value from the buffer and sets `count = 0` to indicate the buffer is empty.

---

#### **Summary**

1. **Condition Variables**:
    - Allow threads to wait for a condition to become true efficiently.
    - Use `pthread_cond_wait()` and `pthread_cond_signal()` for waiting and signaling.
2. **State Variable**:
    - Always use a state variable to track the condition's state.
3. **Locking**:
    - Always hold the lock when calling `wait()` or `signal()` to avoid race conditions.
4. **Best Practices**:
    - Use a `while` loop to check the condition in `wait()`.