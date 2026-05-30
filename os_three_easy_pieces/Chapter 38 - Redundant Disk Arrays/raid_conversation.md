
# ✅ **RAID Overview Recap**

- RAID = Redundant Array of Independent Disks
- RAID-0 = striping only (no redundancy)
- RAID-4 & RAID-5 = striping **with parity** = can survive **1 disk failure**

Both RAID-4 and RAID-5 rely on **XOR parity blocks**.

---
# 🔥 **Before EVERYTHING: How XOR Parity Works**

XOR truth table:

| A   | B   | A ⊕ B |
| --- | --- | ----- |
| 0   | 0   | 0     |
| 0   | 1   | 1     |
| 1   | 0   | 1     |
| 1   | 1   | 0     |

**Important properties:**

1. **XOR is reversible**
```
A ⊕ B = C   →   C ⊕ B = A
```

2. XOR of multiple values:
```
P = A ⊕ B ⊕ C ⊕ D
```
3. If **one disk dies**, you can recover the missing data because:
```
Missing = Parity ⊕ all_other_blocks
```

# 🎯 **RAID-4**

### Structure:

- Data disks: D0, D1, D2, D3
- **Dedicated parity disk**: P
    
Example stripe (each row is a stripe):

| D0  | D1  | D2  | D3  | P           |
| --- | --- | --- | --- | ----------- |
| A0  | A1  | A2  | A3  | A0⊕A1⊕A2⊕A3 |
| B0  | B1  | B2  | B3  | B0⊕B1⊕B2⊕B3 |
## 🔥 **Recovery Example**

Say disk D2 dies (contains A2 and B2).

Row 1 parity:
```
P = A0 ⊕ A1 ⊕ A2 ⊕ A3
```
To recover A2:
```
A2 = P ⊕ A0 ⊕ A1 ⊕ A3
```

This is how XOR saves your ass.

---

## ⚠️ **The RAID-4 Problem: Small Write Bottleneck**

Imagine writing only **A2** changes to **A2'**.

To update parity:

Old parity:
```
P_old = A0 ⊕ A1 ⊕ A2 ⊕ A3
```
New parity:
```
P_new = A0 ⊕ A1 ⊕ A2' ⊕ A3
```
To compute P_new, RAID must:

1. Read old A2
2. Read old parity
3. Compute difference
4. Write new A2
5. Write new parity (to the **same dedicated parity disk**)
   
### 📌 Bottleneck:

**Every write must hit the same parity disk → giant bottleneck.**  
This is why RAID-4 is rarely used in the real world.

---

# ⭐ **RAID-5: Distributed Parity**

RAID-5 solves the RAID-4 bottleneck.

Parity is **rotated** across disks so no single disk is overloaded:

| Stripe | D0       | D1       | D2       | D3       |
| ------ | -------- | -------- | -------- | -------- |
| 1      | A0       | A1       | A2       | **P(A)** |
| 2      | B0       | B1       | **P(B)** | B2       |
| 3      | C0       | **P(C)** | C1       | C2       |
| 4      | **P(D)** | D0       | D1       | D2       |
# 🔥 **How Small Write Works in RAID-5**

**Same XOR logic, but the parity block is in different disks.**

Small write steps:

1. Read old data block
2. Read old parity block
3. Compute new parity
4. Write new data
5. Write new parity

You still have **4 I/O operations** for a small write, called the:

## ❗ **RAID-5 Write Penalty**

Small random write = **4 physical I/Os**

This is often called the _"read-modify-write penalty"_.

---

# ⚠️ **RAID-5 Problems**

### 1. **Write penalty**

Even though parity rotates, every small write still needs:

- read old data
    
- read old parity
    
- write new data
    
- write new parity
    

→ 4 I/O operations → slow.

---

### 2. **Rebuild risk**

If one disk fails, rebuild requires:
```
Read all remaining disks + recompute lost disk using XOR
```

During rebuild:

- Massive read load
    
- Array is degraded
    
- If ANY disk has unreadable sectors → RAID-5 collapses
    

This is why modern arrays prefer RAID-6 or RAID-Z.

---

### 3. **Write Hole Problem**

If power fails during:
- data write
- parity write
    
Parity may become inconsistent → corruption.

Filesystems like ZFS avoid this.

---

# 🎉 **Why RAID-5 Is Popular**

- Faster reads (striping)
    
- Good capacity efficiency (1 disk of parity)
    
- Protection against 1 disk failure
    

But:

- Not ideal for heavy-write workloads
    
- Not safe with very large disks due to URE (unrecoverable read error) risk
    

---

# 🎯 **Summary Table**

|RAID Level|Parity Disk|Performance|Small Write|Failure Tolerance|
|---|---|---|---|---|
|RAID-4|Dedicated|Fast reads, slow writes|**Terrible (parity bottleneck)**|1 disk|
|RAID-5|Distributed|Good|**Write penalty (4 I/Os)**|1 disk|

# 📌 **Quick Intuition: Why XOR Works**

Imagine three disks per stripe:
```
Disk1: 10101100
Disk2: 00011100
Parity = XOR(Disk1, Disk2)
       = 10101100
         00011100
         --------
         10110000
```
If Disk2 dies:
```
Disk2 = Parity ⊕ Disk1
```


# 🧩 **What Is a Stripe?**

A **stripe** = one horizontal "row" of data spread across multiple disks.

Example with **4 disks**:
```
Disk0   Disk1   Disk2   Disk3
-----   -----   -----   -----
 A0      A1      A2      A3       ← this entire row = **one stripe**
 B0      B1      B2      B3       ← this is another stripe
 C0      C1      C2      C3       ← stripe
```


A **stripe** is made of **stripe units** (individual blocks) that live on each disk.

Think:

👉 **Stripe = one chunk of your file split across all disks.**  
👉 **Stripe unit = the piece stored on each disk.**

---

# 🚀 **What Is Striping?**

**Striping** = the technique of splitting data into chunks and distributing them across disks.

Instead of writing whole megabytes to disk 0 → then disk 1 → etc., you break the data like:
```
Write this file:
[A0 A1 A2 A3 A4 A5 A6 A7 ...]

Stripe across 4 disks:

Disk0: A0 | A4 | A8  ...
Disk1: A1 | A5 | A9  ...
Disk2: A2 | A6 | A10 ...
Disk3: A3 | A7 | A11 ...
```

This massively increases read/write speed because **many disks handle the workload in parallel**.

---

# 🎯 In RAID-4 or RAID-5 Context

A **stripe** includes:
- Data blocks on each data disk
- The **parity block** (on parity disk → RAID-4, rotated -> RAID-5)
RAID-4 example:

```
D0     D1     D2     P
----   ----   ----   ----
A0     A1     A2     P(A)     ← Stripe 1
B0     B1     B2     P(B)     ← Stripe 2
C0     C1     C2     P(C)     ← Stripe 3
```
RAID-5 example (parity rotates):
```
D0     D1     D2     D3
----   ----   ----   ----
A0     A1     A2     P(A)     ← Stripe 1
B0     B1     P(B)   B2       ← Stripe 2
C0     P(C)   C1     C2       ← Stripe 3
P(D)   D0     D1     D2       ← Stripe 4
```

# 🧠 So Why Stripe?

**Parallel performance!**

If a file is striped across 4 disks, reading it means:

- All 4 disks work at the same time
    
- You get ~4x read throughput
    
- ~4x write throughput (minus parity overhead for RAID-4/5)
    

---

# 📝 Super Simple Analogy

Imagine you want to carry 400 boxes from the truck into your house.

- 1 person = slow (single disk)
    
- 4 people = each carries 100 boxes → **4x faster** (striping)
    

Each row of 4 boxes they bring in = **a stripe**.

---

# TL;DR Version

- **Stripe:** A horizontal row of blocks (one block per disk).
    
- **Striping:** Splitting data across disks so many disks read/write in parallel.
    

---

# ✅ **A single file _can_ (and usually _will_) be spread across multiple disks.**

When RAID uses **striping**, it **splits the file into blocks** and distributes those blocks across disks like this:

Example file:
```
FILE = [Block0, Block1, Block2, Block3, Block4, Block5, Block6, Block7]
```

With 4 disks:
```
Disk0:  Block0   Block4
Disk1:  Block1   Block5
Disk2:  Block2   Block6
Disk3:  Block3   Block7
```

This entire horizontal row of Block0-Block3 = **one stripe**.

---

# 🧠 **Why do this?**

### 1. **Speed**

Instead of 1 disk reading the whole file slowly…

→ all disks read their blocks **in parallel**.

If each block is 4 KB, RAID reads 16 KB at a time (4 disks x 4 KB).  
So it's ~4× faster.

### 2. **Scalability**

As disks get bigger and faster, parallel reading helps massively.

---

# 🧩 **But what about RAID-4/5 parity?**

Even with parity, the data still stripes across disks.

RAID-5 example:


# 🧠 **Where is the mapping "block X → disk Y and stripe offset Z" stored?**

### 🔸 **Not in the filesystem**

The FS has **no idea RAID even exists**.

### 🔸 **Not in the OS**

The OS only sees one logical device:  
`/dev/md0` (Linux software RAID), or  
`/dev/sda` (hardware RAID)

### 🔸 **It is stored and managed in the RAID layer itself.**

### ✔ If using **hardware RAID**, the controller card stores metadata:

- On the controller’s firmware
    
- And often on the disks themselves (first/last few sectors)
    

### ✔ If using **software RAID** (like Linux mdadm):

- RAID metadata is stored on each disk (superblock)
    
- RAID driver maintains the stripe mapping
    
- This mapping is algorithmic, not per-file
    

---

# 🧩 **How RAID Knows Which Disk Holds What Block**

A RAID controller uses a **mathematical rule** based on:

- Stripe size (e.g., 64 KB)
    
- Number of disks (e.g., 4 disks)
    
- RAID level (0/4/5/6)
---
# 🚀 **Why RAID-5 Is Faster Than RAID-4**

The _only_ reason RAID-5 is faster than RAID-4 is:

> **RAID-5 distributes (rotates) parity across all disks. RAID-4 keeps parity on one dedicated disk.**

This eliminates the **parity disk bottleneck** that kills RAID-4 performance.

Let’s walk through it clearly.

---

# 🧱 **RAID-4 Problem: A Single Parity Disk Gets Hammered**

RAID-4 layout:
```
Disk0   Disk1   Disk2   ParityDisk
----    ----    ----    ----------
A0      A1      A2      P(A)
B0      B1      B2      P(B)
C0      C1      C2      P(C)
```

When you write _any_ block (even a small write), RAID-4 must:

1. Read old data block
    
2. Read parity block **from the parity disk**
    
3. Write new data block
    
4. Write new parity block **to the parity disk**
    

### ⚠️ That means:

Every small write must touch the **same parity disk**.

So if 10 clients are writing, all 10 must hit the same parity disk → **bottleneck**.

It becomes the slowest component of the array.

---

# 🔥 **RAID-5 Fix: Parity Rotates Across All Disks**

RAID-5 layout example:
```
Stripe 1:
D0: A0    D1: A1    D2: A2    D3: P(A)

Stripe 2:
D0: B0    D1: B1    D2: P(B)  D3: B2

Stripe 3:
D0: C0    D1: P(C)  D2: C1    D3: C2

Stripe 4:
D0: P(D)  D1: D0    D2: D1    D3: D2
```

Notice **every disk** takes turns storing parity.

So if 10 write requests come in, the parity writes get spread across multiple disks:
```
Some writes hit disk 0
Some hit disk 1
Some hit disk 2
Some hit disk 3
```


### 🟢 No more single-disk bottleneck

This is the _entire speed advantage_ RAID-5 has over RAID-4.

---

# 📌 **Important: RAID-5 is NOT faster at small random writes**

Both RAID-4 and RAID-5 have the same “write penalty”:

- read old data
    
- read old parity
    
- write new data
    
- write new parity
    

= **4 I/O operations per small write**  
(“read-modify-write penalty”)

So RAID-5 doesn't magically make small writes fast.

👉 **It only makes them _less bad_**  
because the work is _spread across disks_ instead of hammering one disk.

---

# 🧠 **Why RAID-5 Is Actually Faster Overall**

### RAID-4 performance:

- Reads: great
    
- Large writes: great
    
- Small writes: **terrible** (parity disk bottleneck)
    

### RAID-5 performance:

- Reads: great
    
- Large writes: great
    
- Small writes: **still slow but not catastrophic**
    

Because all disks participate in parity writes **in parallel**.