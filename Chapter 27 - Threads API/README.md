# 🧵 Interlude: Thread API — Notes

  

> From *Operating Systems: Three Easy Pieces*  

> Chapter 27 — Thread API

---
## 📘 Overview

This chapter introduces the main parts of the **POSIX thread (pthreads) API** — the standard interface for creating and managing threads in C.  

While later chapters go deeper into topics like **locks** and **condition variables**, this chapter serves as a **reference summary** for the core API.

---

  
## 💡 CRUX: How to Create and Control Threads

  

> **Question:** What interfaces should the OS provide for thread creation and control?  

> **Goal:** Interfaces should be **simple**, **useful**, and **powerful** enough for multi-threaded programming.


---

## 27.1 🧩 Thread Creation

To create threads in POSIX:

```c

#include <pthread.h>

  

int pthread_create(

    pthread_t *thread,

    const pthread_attr_t *attr,

    void *(*start_routine)(void *),

    void *arg

);

```
### Parameters:

1. **thread** — Pointer to a `pthread_t` structure, used to identify the thread.

2. **attr** — Thread attributes (e.g., stack size, scheduling priority). Usually `NULL`.

3. **start_routine** — Function pointer to the thread’s starting routine.  

   Signature:

   ```c

   void *function(void *arg);

   ```

4. **arg** — Argument passed to the thread’s start function.

### Example: Creating a Thread

```c

#include <stdio.h>

#include <pthread.h>

  

typedef struct {

    int a;

    int b;

} myarg_t;

  

void *mythread(void *arg) {

    myarg_t *args = (myarg_t *) arg;

    printf("%d %d\n", args->a, args->b);

    return NULL;

}

  

int main(int argc, char *argv[]) {

    pthread_t p;

    myarg_t args = {10, 20};

  

    int rc = pthread_create(&p, NULL, mythread, &args);

    ...

}

```

  

📘 **Explanation:**

- Thread `p` is created.

- Arguments are passed as a struct `myarg_t`.

- Each thread runs concurrently, sharing the same address space but with its own **call stack**.

---
## 27.2 ✅ Thread Completion
To wait for a thread to finish, use:

```c

int pthread_join(pthread_t thread, void **value_ptr);

```


- **thread:** Thread to wait for (same one passed to `pthread_create()`).

- **value_ptr:** Receives the return value from the thread (of type `void *`).
### Example: Waiting for Thread Completion


```c

typedef struct { int a; int b; } myarg_t;

typedef struct { int x; int y; } myret_t;

  

void *mythread(void *arg) {

    myret_t *rvals = Malloc(sizeof(myret_t));

    rvals->x = 1;

    rvals->y = 2;

    return (void *) rvals;

}

  

int main(int argc, char *argv[]) {

    pthread_t p;

    myret_t *rvals;

    myarg_t args = {10, 20};

  

    Pthread_create(&p, NULL, mythread, &args);

    Pthread_join(p, (void **) &rvals);

  

    printf("returned %d %d\n", rvals->x, rvals->y);

    free(rvals);

    return 0;

}

```

  

**Notes:**

- urn stack-allocated pointers — stack memory is destroyed on return.
### ❌ Dangerous Example — Returning Stack Variable

  

```c

void *mythread(void *arg) {

    myarg_t *args = (myarg_t *) arg;

    printf("%d %d\n", args->a, args->b);

    myret_t oops; // Allocated on stack: BAD!

    oops.x = 1;

    oops.y = 2;

    return (void *) &oops; // returns pointer to destroyed memory

}

```

### Simpler Argument Passing Example

```c

void *mythread(void *arg) {

    long long int value = (long long int) arg;

    printf("%lld\n", value);

    return (void *) (value + 1);

}

  

int main(int argc, char *argv[]) {

    pthread_t p;

    long long int rvalue;

  

    Pthread_create(&p, NULL, mythread, (void *) 100);

    Pthread_join(p, (void **) &rvalue);

  

    printf("returned %lld\n", rvalue);

    return 0;

}

```

  

🧠 **Insight:** Using `pthread_create()` immediately followed by `pthread_join()` is redundant — similar to a normal function call.  

Real programs typically use multiple threads working concurrently.

---
## 27.3 🔒 Locks (Mutexes)

Used to enforce **mutual exclusion** for **critical sections**:

```c

int pthread_mutex_lock(pthread_mutex_t *mutex);

int pthread_mutex_unlock(pthread_mutex_t *mutex);

```

### Example:

```c

pthread_mutex_t lock;

  

pthread_mutex_lock(&lock);

x = x + 1; // critical section

pthread_mutex_unlock(&lock);

```

  

### ⚠️ Problems if not careful:

1. **Initialization Missing**

Two ways to initialize:
**Static:**

```c

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

```

**Dynamic:**

```c

int rc = pthread_mutex_init(&lock, NULL);

assert(rc == 0);

```

Destroy when done:

```c

pthread_mutex_destroy(&lock);

```

2. **Ignoring Error Codes**

Always check return values.

✅ Use wrapper functions for cleaner code:


```c

void Pthread_mutex_lock(pthread_mutex_t *mutex) {

    int rc = pthread_mutex_lock(mutex);

    assert(rc == 0);

}

```

  

### Other Mutex Functions

  

```c

int pthread_mutex_trylock(pthread_mutex_t *mutex);

int pthread_mutex_timedlock(pthread_mutex_t *mutex, struct timespec *abs_timeout);

```

  

- `trylock`: Fails immediately if lock is held.

- `timedlock`: Waits until timeout or lock acquired.

  

---

  

## 27.4 ⏳ Condition Variables

Used for **signaling between threads**.
### API:

```c

int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);

int pthread_cond_signal(pthread_cond_t *cond);

```

A **condition variable** must always be used **with a lock**.

---

  

### Example: Waiting Thread

  

```c

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

  

Pthread_mutex_lock(&lock);

while (ready == 0)

    Pthread_cond_wait(&cond, &lock);

Pthread_mutex_unlock(&lock);

```

  

### Example: Signaling Thread

  

```c

Pthread_mutex_lock(&lock);

ready = 1;

Pthread_cond_signal(&cond);

Pthread_mutex_unlock(&lock);

```

  

🧠 **Important Concepts:**

- `pthread_cond_wait()` releases the lock while sleeping, then reacquires it before returning.

- Always check the condition in a **`while` loop**, not `if`, to avoid **spurious wakeups**.

- Using a simple flag (`while(ready == 0);`) is **inefficient** and **bug-prone**. Always use condition variables.

  

---

  

## 27.5 ⚙️ Compiling and Running Threads

  

Include header and link pthread library:

  

```bash

gcc -o main main.c -Wall -pthread

```

  

---

  

## 27.6 🧭 Summary

  

The pthread library provides:

- **Thread creation:** `pthread_create()`

- **Thread completion:** `pthread_join()`

- **Mutual exclusion:** `pthread_mutex_lock()` / `unlock()`

- **Synchronization:** `pthread_cond_wait()` / `signal()`

  

Mastering these allows building correct and performant multi-threaded programs.

  

---

  

## 🧠 Thread API Guidelines

  

> General tips for using POSIX threads effectively:

  

- **Keep it simple:** Avoid complex locking logic.

- **Minimize interactions:** Fewer shared resources = fewer bugs.

- **Initialize locks and condition variables.**

- **Check return codes.**

- **Avoid stack references:** Never pass or return pointers to stack variables.

- **Each thread has its own stack.**

- **Always use condition variables** (not flags) for signaling.

- **Use `man pthread_*`** — Linux manual pages are extremely helpful.

  

---

  

## 📚 References

  

- **[B89]** Andrew D. Birrell, *An Introduction to Programming with Threads*, DEC, 1989.  

- **[B97]** David R. Butenhof, *Programming with POSIX Threads*, Addison-Wesley, 1997.  

- **[B+96]** Buttlar et al., *PThreads Programming*, O’Reilly, 1996.  

- **[K+96]** Kleiman et al., *Programming With Threads*, Prentice Hall, 1996.  

- **[X+10]** Weiwei Xiong et al., *Ad Hoc Synchronization Considered Harmful*, OSDI 2010.

  

---

  

## 🧩 Homework (Code)

  

Use **Helgrind** (`valgrind --tool=helgrind`) to analyze multi-threaded programs for data races and deadlocks.

  

### Tasks:

1. Build and analyze `main-race.c` – observe reported data races.

2. Fix data race using locks and observe Helgrind’s output.

3. Analyze `main-deadlock.c` – identify deadlock cause.

4. Compare results with `main-deadlock-global.c`.

5. Examine `main-signal.c` – identify inefficiency due to busy waiting.

6. Compare with `main-signal-cv.c` – condition variable version.

7. Run Helgrind on all examples.

  

---

  

✅ **End of Notes**  

These cover every detail, code example, and explanation from the chapter in Markdown format, suitable for README or study reference.