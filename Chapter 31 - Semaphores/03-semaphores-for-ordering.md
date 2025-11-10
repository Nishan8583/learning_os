### Key Notes on Semaphores for Ordering

#### Overview

- Semaphores are used to order events in concurrent programs.
- They help one thread wait for an event to occur, while another thread signals when the event has occurred.
- Semaphores act as an ordering primitive, similar to condition variables.

---

#### Example: Parent Waiting for Child

- **Scenario**: A parent thread creates a child thread and waits for it to complete execution before proceeding.
- **Expected Output**:
    
    ```
    parent: begin
    child
    parent: end
    ```
    

---

#### Code Example

```c
sem_t s;

void *child(void *arg) {
    printf("child\n");
    sem_post(&s); // Signal: child is done
    return NULL;
}

int main(int argc, char *argv[]) {
    sem_init(&s, 0, 0); // Initialize semaphore with value 0
    printf("parent: begin\n");
    pthread_t c;
    pthread_create(&c, NULL, child, NULL);
    sem_wait(&s); // Wait for child to signal
    printf("parent: end\n");
    return 0;
}
```

---

#### Key Points

1. **Initial Value of Semaphore**:
    
    - The semaphore should be initialized to `0` to ensure proper synchronization.
    - This ensures the parent waits for the child to signal before proceeding.
2. **How It Works**:
    
    - The parent calls `sem_wait()` to wait for the child to signal.
    - The child signals completion by calling `sem_post()`, which increments the semaphore value.
    - If the semaphore value is greater than 0, the parent does not wait and proceeds.

---

#### Two Cases of Execution

1. **Case 1: Parent Calls `sem_wait()` Before Child Calls `sem_post()`**
    
    - Initial semaphore value: `0`.
    - Parent calls `sem_wait()` and decrements the semaphore to `-1`, then sleeps.
    - Child runs, calls `sem_post()`, increments semaphore to `0`, and wakes the parent.
    - Parent resumes and completes execution.
    
    **Thread Trace**:
    
    |Semaphore Value|Parent State|Child State|
    |---|---|---|
    |0|create(Child)|Ready|
    |0|call `sem_wait()`|Ready|
    |-1|decrement semaphore|Ready|
    |-1|Sleep|Run|
    |0|Wake (Parent)|Run|
    |0|`sem_wait()` returns|Ready|
    
2. **Case 2: Child Calls `sem_post()` Before Parent Calls `sem_wait()`**
    
    - Initial semaphore value: `0`.
    - Child runs first, calls `sem_post()`, increments semaphore to `1`.
    - Parent calls `sem_wait()`, decrements semaphore to `0`, and proceeds without waiting.
    
    **Thread Trace**:
    
    |Semaphore Value|Parent State|Child State|
    |---|---|---|
    |0|create(Child)|Ready|
    |0|Ready|Run|
    |1|Ready|Increment semaphore|
    |1|Run|Ready|
    |0|Decrement semaphore|Ready|
    |0|`sem_wait()` returns|Ready|
    

---

#### Key Takeaways

- **Synchronization**: Semaphores ensure proper synchronization between threads.
- **Initial Value**: The semaphore should be initialized to `0` to ensure the parent waits for the child.
- **Thread Behavior**:
    - If the parent calls `sem_wait()` first, it will sleep until the child signals with `sem_post()`.
    - If the child signals first, the parent will not block and will proceed immediately.