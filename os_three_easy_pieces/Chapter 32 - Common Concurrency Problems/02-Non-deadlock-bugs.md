## 📘 Overview

**Non-deadlock bugs** are the **most common type** of concurrency bugs (about 70% in Lu et al.’s study).  
They don’t cause total system hangs like deadlocks, but instead cause:

- **Crashes**
    
- **Incorrect results**
    
- **Unexpected behavior**
    

---

## 🧩 Two Major Types of Non-Deadlock Bugs

According to **Lu et al. [L+08]**, non-deadlock bugs mostly fall into two categories:

1. **Atomicity Violation Bugs**
    
2. **Order Violation Bugs**
    

---

## ⚙️ 1. Atomicity Violation Bugs

### 💡 Definition

> “The desired serializability among multiple memory accesses is violated —  
> i.e., a code region is intended to be atomic, but atomicity is not enforced during execution.”  
> — _Lu et al._

In simple terms:

- Two (or more) threads access shared data that must be accessed atomically.
    
- But, one thread gets **interrupted mid-operation**, letting another thread modify the data → causing **incorrect results or crashes**.
    

---

### 🧱 Example: MySQL Atomicity Violation
```c
// Thread 1
if (thd->proc_info) {
    fputs(thd->proc_info, ...);
}

// Thread 2
thd->proc_info = NULL;

```

#### 🧨 The Problem:

1. Thread 1 checks `thd->proc_info` (non-NULL).
    
2. It’s interrupted before calling `fputs()`.
    
3. Thread 2 sets `thd->proc_info = NULL`.
    
4. Thread 1 resumes — `fputs()` dereferences `NULL` → **crash**.
    

### 🧩 Why It Happens:

- The **check** and **use** of `proc_info` are **not atomic**.
    
- There’s an **atomicity assumption**: that nothing changes `proc_info` between these two operations.
    
- But this assumption fails without synchronization.
    

---

### 🧰 Fix: Use Locks to Enforce Atomicity
```c
pthread_mutex_t proc_info_lock = PTHREAD_MUTEX_INITIALIZER;

// Thread 1
pthread_mutex_lock(&proc_info_lock);
if (thd->proc_info) {
    fputs(thd->proc_info, ...);
}
pthread_mutex_unlock(&proc_info_lock);

// Thread 2
pthread_mutex_lock(&proc_info_lock);
thd->proc_info = NULL;
pthread_mutex_unlock(&proc_info_lock);
```
✅ **Why It Works:**

- Both threads hold `proc_info_lock` before accessing `proc_info`.
    
- Only one thread can read or modify it at a time.
    
- The **atomicity** between “check” and “use” is now enforced.
    

---

## ⚙️ 2. Order Violation Bugs

### 💡 Definition

> “The desired order between two (groups of) memory accesses is flipped —  
> i.e., operation A should always happen before operation B, but the order is not enforced.”

In simple terms:

- A **race** happens between **initialization** and **usage**.
    
- One thread **uses** a variable or structure **before it is initialized** by another.
    

---

### 🧱 Example: Ordering Bug
```c
// Thread 1
void init() {
    mThread = PR_CreateThread(mMain, ...);
}

// Thread 2
void mMain(...) {
    mState = mThread->State;
}
```
#### 🧨 The Problem:

- Thread 2 assumes `mThread` has already been initialized.
    
- If Thread 2 starts running **immediately after creation**,  
    `mThread` might still be `NULL`.
    
- Dereferencing `mThread->State` → **NULL pointer crash**.
    

---

### 🧰 Fix: Enforce Ordering with Condition Variables
```c
pthread_mutex_t mtLock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t mtCond = PTHREAD_COND_INITIALIZER;
int mtInit = 0;

// Thread 1
void init() {
    mThread = PR_CreateThread(mMain, ...);

    // Signal that thread initialization is complete
    pthread_mutex_lock(&mtLock);
    mtInit = 1;
    pthread_cond_signal(&mtCond);
    pthread_mutex_unlock(&mtLock);
}

// Thread 2
void mMain(...) {
    // Wait until initialization is done
    pthread_mutex_lock(&mtLock);
    while (mtInit == 0)
        pthread_cond_wait(&mtCond, &mtLock);
    pthread_mutex_unlock(&mtLock);

    mState = mThread->State;
}
```


}

✅ Why It Works:

    Thread 2 waits on a condition variable until mtInit becomes 1.

    Thread 1 signals when initialization is done.

    Guarantees that initialization happens before usage.

🧠 Non-Deadlock Bugs — Summary
Type	Definition	Common Cause	Fix
Atomicity Violation	Intended atomic region not protected	Missing synchronization around shared data	Use mutex locks
Order Violation	Operation B runs before A (incorrect order)	Lack of coordination between threads	Use condition variables or semaphores
📊 Real-World Findings (Lu et al. [L+08])

    97% of non-deadlock bugs are atomicity or order violations.

    Understanding these patterns helps prevent the majority of concurrency issues.

    Automated tools can focus on detecting these patterns to catch most concurrency bugs early.

⚠️ Note:

Not all concurrency bugs are as simple as these examples —
some require deeper logic understanding or significant code restructuring to fix.