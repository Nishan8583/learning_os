# 🧠 Rest of Stuffs

After console init, kernel does:

1. 🧱 Initialize heap
2. 🧠 Validate CPU
3. 🧬 Tell BIOS about future mode
4. 🧠 Detect memory
5. ⌨️ Init keyboard
6. 🔍 Query BIOS features

👉 This is the **last preparation phase before protected mode**

---

# 🔥 1. Heap Initialization (`init_heap`)

---

## 🧠 Why heap is needed?

At this stage:

- Stack exists ✅
- BSS exists ✅
- But dynamic memory ❌

👉 Kernel needs heap for:

- structures
- buffers
- future allocations

---

## 🔹 Step 1: Check if heap allowed

`if (boot_params.hdr.loadflags & CAN_USE_HEAP)`

👉 Bootloader tells kernel:

> “You are allowed to use heap”

---

## 🔹 Step 2: Calculate stack end

`stack_end = esp - STACK_SIZE;`

👉 Important idea:

- Stack grows **down**
- Heap grows **up**

So kernel defines a boundary:
```
[ heap ↑       ]   
[ free space   ]  
[ stack ↓      ]

```
---

## 🔹 Step 3: Calculate heap end

heap_end = heap_end_ptr + 0x200;

👉 Why +512?

- Safety margin
- Historical alignment buffer

---

## 🔹 Step 4: Prevent collision

```c
if (heap_end > stack_end)  
    heap_end = stack_end;

```
👉 CRITICAL:

> “Heap must NOT overwrite stack”

---

## 🧠 Final result

Heap range is:

heap_start → heap_end

---

## 🔥 Key Insight

This is a **manual memory allocator setup**

No `malloc`, no paging, no VM.

---

# 🔥 2. CPU Validation (`validate_cpu`)

---

## 🧠 Purpose

> “Can this CPU even run this kernel?”

---

## 🔹 Core check

```

if (cpu_level < req_level)  
    fail

```
---

## 🔍 What it checks

- CPU type (386, 486, etc.)
- Vendor (Intel / AMD)
- Features:
    - Long mode (64-bit)
    - SSE/SSE2
- Flags

---

## 🧠 Why important?

👉 Kernel may rely on:

- 64-bit instructions
- specific CPU features

---

# 🔥 3. set_bios_mode()

---

## 🧠 Purpose

Tell BIOS:

> “Hey, I’m about to switch to 64-bit mode”

---

## 🔹 Code
```

ireg.ax = 0xec00;  
ireg.bx = 2;  
intcall(0x15, ...)

```
---

## 🔹 BIOS interrupt

INT 0x15 → system services

---

## 🧠 Meaning

- `bx = 2` → long mode (64-bit)

---

## 🔥 Insight

👉 BIOS must know because:

- It may behave differently
- Prepares hardware compatibility

---

# 🔥 4. Memory Detection (`detect_memory`)

---

## 🧠 Purpose

> “What RAM do I actually have?”

---

## 🔹 Best method: E820

BIOS interrupt:

`INT 0x15, AX = 0xE820`

---

## 🔹 Setup registers

ireg.ax  = 0xe820  
ireg.cx  = sizeof buf  
ireg.edx = SMAP  
ireg.di  = &buf

---

## 🔹 Important values

|Register|Meaning|
|---|---|
|AX|function (E820)|
|CX|buffer size|
|EDX|"SMAP" magic|
|ES:DI|buffer address|
|EBX|continuation value|

---

## 🔁 Loop behavior

call BIOS  
store result  
ebx = next pointer  
repeat until done

---

## 🧠 Output

Each entry:
```

[start address]  
[size]  
[type]

```
---

## 🔹 Types

|Type|Meaning|
|---|---|
|usable|RAM|
|reserved|DO NOT USE|

---

## 🧠 Example (dmesg)

```

[mem 0x0 - 0x9fbff] usable  
[mem 0x9fc00 - ...] reserved

```
---

## 🔥 Insight

👉 This becomes:

- Kernel memory map
- Used for paging later

---

# 🔥 5. Keyboard Initialization

---

## 🧠 Purpose

> “Check keyboard and configure it”

---

## 🔹 Get keyboard status

```

ireg.ah = 0x02  
intcall(0x16)

```
👉 BIOS interrupt:

INT 0x16 → keyboard services

---

## 🔹 Set repeat rate

```

ireg.ax = 0x0305  
intcall(0x16)

```
---

## 🧠 Result

Stored in:

boot_params.kbd_status

---

# 🔥 6. BIOS Queries (Extra Info)

These are **optional but useful hardware info**

---

## 🔹 (1) SpeedStep (`query_ist`)

- Intel CPU power scaling
- Uses `INT 0x15`

---

## 🔹 (2) APM (`query_apm_bios`)

Power management:

INT 0x15, AH = 0x53

Checks:

- Signature = `0x504d` ("PM")
- Carry flag
- Mode support

Then switches to:

- 32-bit interface

---

## 🔹 (3) EDD (`query_edd`)

Disk info via BIOS

---

### 🔍 Loop

for devno = 0x80 → 0x8F

👉 0x80 = first hard disk

---

### 🔹 Check EDD support

INT 0x13, AH = 0x41

---

### 🔹 Get disk info

INT 0x13, AH = 0x48

---

## 🧠 Output

Stored in:

boot_params.eddbuf

---

# 🧠 SUPER IMPORTANT INSIGHT (for YOU)

This entire phase shows:

---

## 🔥 1. Kernel is still BIOS-dependent

Everything uses:

- `int 0x10` (video)
- `int 0x13` (disk)
- `int 0x15` (system)
- `int 0x16` (keyboard)

👉 After protected mode → BIOS is gone

---

## 🔥 2. This is hardware reconnaissance

Kernel is basically doing:

CPU? ✔  
RAM? ✔  
Disk? ✔  
Input? ✔  
Heap? ✔

---

## 🔥 3. Critical for security / RE

This phase is:

- Trusted boundary
- Attack surface for bootkits
- Source of hardware assumptions
