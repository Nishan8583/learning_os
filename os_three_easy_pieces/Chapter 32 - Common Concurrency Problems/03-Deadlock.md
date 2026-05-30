# 🧠 Common Concurrency Problems — Deadlocks and Prevention Strategies

---

## ⚙️ Example: Hidden Deadlock Risk in Multi-Threaded Code
```java
Vector v1, v2;
v1.AddAll(v2);
```
### 🧨 Problem:

- Internally, both `v1` and `v2` are **locked** to ensure thread safety.
    
- `v1.AddAll(v2)` → acquires lock on `v1`, then `v2`
    
- Simultaneously, another thread might call `v2.AddAll(v1)` → acquires `v2`, then `v1`
    
- 🔁 **Circular waiting** occurs → **Deadlock**
## 🧩 Four Conditions for Deadlock [Coffman et al., 1971]

A deadlock occurs **only if all four** of these conditions hold:


| Condition               | Description                                                           |
| ----------------------- | --------------------------------------------------------------------- |
| **1. Mutual Exclusion** | Threads require **exclusive access** to resources (e.g., locks).      |
| **2. Hold and Wait**    | Threads **hold** resources while **waiting** for others.              |
| **3. No Preemption**    | Resources **cannot be forcibly taken** from threads.                  |
| **4. Circular Wait**    | A **cycle** exists: each thread waits for a resource held by another. |
✅ **To prevent deadlock**, ensure **at least one** of these conditions does **not** hold.

---

## 🧠 Deadlock Prevention Techniques

### 1️⃣ Preventing **Circular Wait**

#### 💡 Idea:

> Impose a **global ordering** on lock acquisition so that circular waits cannot form.

#### 🔧 Example:

If two locks exist, `L1` and `L2`:

- Always acquire `L1` before `L2`.

```c
pthread_mutex_lock(&L1);
pthread_mutex_lock(&L2);
// critical section
pthread_mutex_unlock(&L2);
pthread_mutex_unlock(&L1);
```

#### 🧩 Partial Ordering:

- Complex systems may require only **partial lock ordering**.
    
- Example (Linux memory management code):
    
    - `"i_mutex before i_mmap_rwsem before swap_lock before pages_lock"`
        

This avoids cycles but requires **careful design and documentation**.

---

### 💡 Tip: Enforce Lock Ordering Dynamically (By Address)

To ensure a consistent lock order when order of arguments varies:
✅ This ensures a **consistent order** across all threads, avoiding circular waits regardless of parameter order.

---

### 2️⃣ Preventing **Hold-and-Wait**

#### 💡 Idea:

> Acquire **all locks at once** (atomically) before using them.

```c
pthread_mutex_lock(&prevention);  // Prevent other threads from acquiring
pthread_mutex_lock(&L1);
pthread_mutex_lock(&L2);
pthread_mutex_unlock(&prevention);
```

✅ Prevents deadlock by **disabling interleaving** during lock acquisition.  
⚠️ **Downsides:**

- Reduces concurrency — all locks are held longer than necessary.
    
- Requires global knowledge of which locks are needed beforehand.
    

---

### 3️⃣ Breaking **No Preemption** — Using `trylock`

#### 💡 Idea:

> Instead of blocking, **back off** if a lock isn’t available and try again later.

```c
top:
pthread_mutex_lock(&L1);
if (pthread_mutex_trylock(&L2) != 0) { // fail if L2 locked
    pthread_mutex_unlock(&L1);
    goto top; // retry
}
```
✅ **Avoids deadlock** because threads release locks before retrying.  
⚠️ **Risk:** May cause **livelock** — threads continuously retry without progress.  
🧠 **Fix:** Add random backoff delays before retrying.

---

### 4️⃣ Removing **Mutual Exclusion** — Lock-Free Programming

#### 💡 Idea:

> Build **lock-free data structures** using atomic hardware operations (e.g., Compare-and-Swap).

---

### 🧩 Example: Compare-and-Swap (CAS)
```c
int CompareAndSwap(int *address, int expected, int new) {
    if (*address == expected) {
        *address = new;
        return 1; // success
    }
    return 0; // failure
}
```
🧩 Example: Atomic Increment

```c
void AtomicIncrement(int *value, int amount) {
    do {
        int old = *value;
    } while (CompareAndSwap(value, old, old + amount) == 0);
}
```
✅ Avoids locks → no deadlock  
⚠️ But may lead to **livelock** if multiple threads repeatedly retry.

---

### 🧩 Example: Lock-Free List Insertion
```c
void insert(int value) {
    node_t *n = malloc(sizeof(node_t));
    n->value = value;

    do {
        n->next = head;
    } while (CompareAndSwap(&head, n->next, n) == 0);
}
```

✅ Safe concurrent insertion at list head  
⚠️ Complex to generalize (e.g., for deletion, lookups).

---

## 🧭 Deadlock Avoidance (via Scheduling)

### 💡 Idea:

> Use **global knowledge** of thread lock requirements to schedule threads safely.


#### Example 1:

|Thread|Needs L1|Needs L2|
|---|---|---|
|T1|✅|✅|
|T2|✅|✅|
|T3|❌|✅|
|T4|❌|❌|
**Safe Schedule:**

- Don’t run T1 and T2 together.
    
- T3 and T4 can run with either safely.
    

#### Example 2 (High Contention):
|Thread|Needs L1|Needs L2|
|---|---|---|

|   |   |   |
|---|---|---|
|T1|✅|✅|

|   |   |   |
|---|---|---|
|T2|✅|✅|

|   |   |   |
|---|---|---|
|T3|✅|✅|

|   |   |   |
|---|---|---|
|T4|❌|❌|
**Safe Schedule:**

- Run T1, T2, T3 **sequentially** (same CPU)
    
- Run T4 concurrently (no locks used)
    

✅ Guarantees safety  
⚠️ Reduces concurrency → performance loss  
📘 Example: **Dijkstra’s Banker’s Algorithm [D64]**

Used in predictable systems (e.g., embedded), not in general-purpose OS.

---

## 🧩 Deadlock Detection and Recovery

### 💡 Idea:

> Let deadlocks happen occasionally and **detect + recover** later.

### 🔧 Technique:

- Periodically build a **resource allocation graph**
    
- Detect **cycles**
    
- If found → **restart system** or **abort threads**
    

Used in:

- Databases (e.g., MySQL, Oracle)
    
- Operating systems (periodic checkers)
    

✅ **Practical** if deadlocks are **rare**  
⚠️ Not ideal for high-availability systems

---

## 🧠 Engineering Perspective — “Tom West’s Law”

> “Not everything worth doing is worth doing well.”  
> — _Tom West, Soul of a New Machine [K81]_

### Meaning:

- Don’t over-engineer rare problems.
    
- Use practical trade-offs:
    
    - If deadlocks are rare → allow detection/recovery.
        
    - If catastrophic → prevent at all costs (e.g., in aerospace software).


## ✅ Summary Table

|Strategy|Prevents Condition|Example|Trade-offs|
|---|---|---|---|
|**Lock Ordering**|Circular Wait|Always acquire L1 before L2|Requires global coordination|
|**Acquire All Locks**|Hold-and-Wait|Grab all locks under prevention lock|Reduces concurrency|
|**Trylock & Retry**|No Preemption|Use `pthread_mutex_trylock()`|Can livelock|
|**Lock-Free Design**|Mutual Exclusion|Use Compare-And-Swap|Complex, prone to livelock|
|**Scheduling**|Avoidance|Banker’s Algorithm|Not scalable|
|**Detection & Recovery**|None (allows deadlock)|Resource graph cycle detection|Practical for rare deadlocks|