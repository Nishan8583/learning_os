### Key Notes on Condition Variables

#### **Introduction to Condition Variables**

- Locks are essential for building concurrent programs, but they are not sufficient for all scenarios.
- Condition variables are used when a thread needs to wait for a specific condition to become true before proceeding.
- Example: A parent thread waiting for a child thread to complete its execution (e.g., `join()`).

---

#### **Example Scenario**

**Code Example: Parent Waiting for Child**

```c
void *child(void *arg) {
    printf("child\n");
    // XXX how to indicate we are done?
    return NULL;
}

int main(int argc, char *argv[]) {
    printf("parent: begin\n");
    pthread_t c;
    Pthread_create(&c, NULL, child, NULL); // child
    // XXX how to wait for child?
    printf("parent: end\n");
    return 0;
}
```

**Desired Output:**

```
parent: begin
child
parent: end
```

---

#### **Inefficient Approach: Spin-Based Waiting**

- A shared variable can be used to indicate the condition, but this approach is inefficient as it wastes CPU cycles by spinning.

**Code Example: Spin-Based Approach**

```c
volatile int done = 0;

void *child(void *arg) {
    printf("child\n");
    done = 1;
    return NULL;
}

int main(int argc, char *argv[]) {
    printf("parent: begin\n");
    pthread_t c;
    Pthread_create(&c, NULL, child, NULL); // child
    while (done == 0)
        ; // spin
    printf("parent: end\n");
    return 0;
}
```

**Issues with Spin-Based Approach:**

1. **Inefficiency**: The parent thread continuously checks the condition, wasting CPU resources.
2. **Incorrectness**: In some cases, spinning may lead to race conditions or incorrect behavior.

---

#### **The Crux: How to Wait for a Condition**

- **Problem**: In multi-threaded programs, a thread often needs to wait for a condition to become true before proceeding.
- **Challenges**:
    - Spinning wastes CPU cycles.
    - Spinning can lead to incorrect behavior in certain scenarios.
- **Solution**: Use condition variables to allow threads to sleep until the condition becomes true.

---

#### **Key Takeaways**

1. **Condition Variables**:
    - Allow threads to wait efficiently for a condition to become true.
    - Avoid the inefficiency and potential issues of spin-based waiting.
2. **Use Case**:
    - Commonly used when one thread needs to wait for another thread to complete a task.
3. **Implementation**:
    - Replace spin-based waiting with condition variables to improve performance and correctness.