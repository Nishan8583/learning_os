# 🧠 Big Picture (What is Protected Mode?)

Think of it like this:

|Mode|Analogy|
|---|---|
|Real Mode|Bare-metal, no rules, tiny memory|
|Protected Mode|OS-ready, structured, controlled memory|

👉 Real mode is:

- Limited (~1MB memory)
- No protection
- Simple segmentation

👉 Protected mode introduces:

- **Up to 4GB memory**
- **Memory protection**
- **Privilege levels (kernel vs user)**
- **Better segmentation**
- Foundation for paging

---

# 🔥 WHY we switch to Protected Mode

Real mode address:

Physical = Segment * 16 + Offset

👉 Problems:

- Only **20-bit address → 1MB max**
- No isolation (any program can overwrite anything)

👉 Protected mode fixes this:

- 32-bit addressing → **4GB memory**
- Controlled access via descriptors

---

# 🧠 Core Idea: Segmentation is Redesigned

In Real Mode:

- Segment = fixed 64KB

In Protected Mode:

- Segment = **fully configurable**
- Defined using a **Segment Descriptor**

---

# 🧱 GDT (Global Descriptor Table)

👉 Think of GDT as:

> “A table that describes ALL memory segments”

Each entry = **Segment Descriptor**

---

## 🔹 Where is GDT stored?

- Somewhere in memory
- CPU stores its location in **GDTR register**

Load it with:

lgdt gdt

---

## 🔹 GDTR structure

- 16-bit → size
- 32-bit → address

---

# 🧩 Segment Descriptor (Important)

Each segment is described by a **64-bit structure**.

It defines:

| Field     | Meaning              |
| --------- | -------------------- |
| Base      | Where segment starts |
| Limit     | Size of segment      |
| Type      | Code/Data            |
| Privilege | Who can access       |
| Flags     | Behavior             |
```

 63         56         51   48    45           39        32 
------------------------------------------------------------
|             | |B| |A|       | |   | |0|E|W|A|            |
| BASE 31:24  |G|/|L|V| LIMIT |P|DPL|S|  TYPE | BASE 23:16 |
|             | |D| |L| 19:16 | |   | |1|C|R|A|            |
------------------------------------------------------------

 31                         16 15                         0 
------------------------------------------------------------
|                             |                            |
|        BASE 15:0            |       LIMIT 15:0           |
|                             |                            |
------------------------------------------------------------

```

---

## 🔥 Base Address (32-bit)

Split across multiple parts, but logically:

Base = starting address of segment

---

## 🔥 Limit (20-bit)

Defines size:

Segment size = Limit + 1

BUT depends on **Granularity (G)**

---

## 🔥 Granularity (G bit)

|G|Unit|
|---|---|
|0|Bytes|
|1|4KB pages|

👉 If G = 1:

Limit << 12  (multiply by 4096)

---

### 🧠 Key Insight

|Mode|Max Segment Size|
|---|---|
|G=0|1 MB|
|G=1|4 GB|

---

# 🧠 Segment Types

## 🔹 Data Segment

- Read-only or read/write
- Can grow up or down

## 🔹 Code Segment

- Executable
- Optional read
- NEVER writable

---

## 🔹 Important Bits

|Bit|Meaning|
|---|---|
|E|Expand direction|
|W|Writable|
|R|Readable|
|A|Accessed|
|C|Conforming (code sharing across privilege)|

---

# 🔐 Privilege Levels (VERY IMPORTANT)

DPL (2 bits):

|Level|Meaning|
|---|---|
|0|Kernel (highest privilege)|
|3|User (lowest privilege)|

👉 This is the foundation of:

- **user space vs kernel space**

---

# 🧠 Segment Registers (Changed!)

In Real Mode:

- Segment register = actual segment base

In Protected Mode:

- Segment register = **selector (NOT address)**

---

# 🔑 Segment Selector (16-bit)

| Index | TI | RPL |

|Field|Meaning|
|---|---|
|Index|Entry in GDT|
|TI|GDT or LDT|
|RPL|Requested privilege|

---

# 🔥 Hidden Magic (CRITICAL)

Each segment register has:

|Part|Description|
|---|---|
|Visible|Selector|
|Hidden|Cached descriptor|

👉 CPU does:

1. You load selector
2. CPU looks up GDT
3. Loads descriptor into hidden part

---

# 🧠 Address Translation (Protected Mode)

Steps:

1. Load selector into segment register
2. CPU finds descriptor in GDT
3. Get **Base**
4. Add Offset

Linear Address = Base + Offset

👉 (Paging comes AFTER this)

---

# 🔥 Switching to Protected Mode

Steps:

1. Disable interrupts
2. Load GDT (`lgdt`)
3. Set PE bit in CR0
4. Jump to new code

---

# 🧠 Kernel Prep (Linux Boot)

Before switching:

- Initialize stuff (keyboard, heap, etc.)
- Copy boot parameters

---

# 🔧 copy_boot_params()

This copies bootloader info into kernel structure.

boot_params.hdr = hdr

---

# 🧠 Assembly memcpy Explanation

### Registers used:

|Register|Meaning|
|---|---|
|ax|destination|
|dx|source|
|cx|size|

---

### What memcpy does:

1. Save registers (si, di)
2. Copy in 4-byte chunks (`movsl`)
3. Copy remaining bytes (`movsb`)
4. Restore registers

---

# 🧾 YOUR CLEAN NOTES (EXAM STYLE)

---

## 🔹 Protected Mode Overview

- Introduced in 80286
- Replaces Real Mode
- Supports:
    - 4GB memory
    - Protection
    - Privilege levels

---

## 🔹 Memory Model

Two parts:

- Segmentation
- Paging (later)

---

## 🔹 GDT

- Stores segment descriptors
- Loaded using `lgdt`
- Stored in GDTR

---

## 🔹 Segment Descriptor (64-bit)

Contains:

- Base (32-bit)
- Limit (20-bit)
- Type
- Privilege (DPL)
- Flags (G, D/B, L, etc.)

---

## 🔹 Granularity (G)

- 0 → bytes
- 1 → 4KB pages

---

## 🔹 Segment Types

### Data

- Read / Write

### Code

- Execute / Read
- Not writable

---

## 🔹 Privilege Levels

- 0 → Kernel
- 3 → User

---

## 🔹 Segment Selector

16-bit:

- Index (GDT entry)
- TI (GDT/LDT)
- RPL

---

## 🔹 Address Translation

Linear Address = Base + Offset

---

## 🔹 Switch to Protected Mode

1. Disable interrupts
2. Load GDT
3. Set CR0.PE
4. Jump

---

## 🔹 memcpy (boot code)

- Uses registers:
    - ax (dest)
    - dx (src)
    - cx (size)
- Copies:
    - 4 bytes at a time
    - Then remaining bytes

---

# 🧠 SUPER IMPORTANT INSIGHT (for you)

Since you’ve read _Operating Systems: Three Easy Pieces_:

👉 Protected mode segmentation = **almost obsolete in modern systems**

Modern OS:

- Use **paging for real memory management**
- Use segmentation minimally (flat model)

👉 Linux typically sets:

Base = 0  
Limit = 4GB

So:

Linear = Virtual Address

👉 Segmentation becomes “invisible”

---
