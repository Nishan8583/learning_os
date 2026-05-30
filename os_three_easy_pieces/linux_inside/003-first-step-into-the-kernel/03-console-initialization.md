# 🧠 Console Initialization

After copying boot params, the kernel does:

> “Let me setup a way to PRINT debug output”

Because at this stage:

- No filesystem ❌
- No drivers ❌
- No full kernel ❌

👉 Only very primitive I/O:

- BIOS interrupts
- Serial port (if enabled)

---

# 🔥 Step 1: `console_init()`

Purpose:

> Initialize early console so kernel can print debug messages

---

## 🔍 What it does

It looks for this kernel parameter:

`earlyprintk=`

Examples:
```
serial,0x3f8,115200  
serial,ttyS0,115200  
ttyS0,115200

```
---

## 🧠 Meaning

|Part|Meaning|
|---|---|
|serial|use serial port|
|0x3f8|I/O port (COM1)|
|115200|baud rate|

---

## ✅ Result

If found:

- Initialize serial port
- Store base address in:

early_serial_base

---

# 🔥 First Debug Output
```c
if (cmdline_find_option_bool("debug"))  
    puts("early console in setup code\n");

```
👉 If `debug` flag is present → print message

---

# 🧠 puts() → putchar()

`puts()` just loops:

```c
for each char:  
    putchar(char)

```
---

# 🔥 putchar() (VERY IMPORTANT)

```c

void putchar(int ch)  
{  
    if (ch == '\n')  
        putchar('\r');  
  
    bios_putchar(ch);  
  
    if (early_serial_base != 0)  
        serial_putchar(ch);  
}

```
---

## 🧠 Key Idea

It prints to **TWO places**:

### 1. VGA (via BIOS)

### 2. Serial port (if enabled)

---

## 🔹 Why `\n → \r\n`?

Old systems expect:

newline = carriage return + line feed

So:

- `\n` → move down
- `\r` → move to start of line

---

# 🔥 bios_putchar()

This is how characters go to screen:
```c
intcall(0x10, &ireg, NULL);
```

👉 BIOS interrupt **0x10 = video services**

---

## 🔹 Registers setup

ireg.ah = 0x0e;   // teletype output  
ireg.al = ch;     // character

---

## 🧠 What happens

CPU does:

INT 0x10 → BIOS → print character on screen

---

# 🔥 initregs()

Before calling BIOS:

memset(reg, 0, sizeof *reg);

👉 Clean register state

Then sets:

- segment registers (ds, es, fs, gs)
- flags

---

# 🔧 memset() (Assembly Deep Dive)

This is VERY similar to memcpy — but instead of copying, it **fills memory**.

---

## 🔹 Calling convention

Same as before:

|Register|Meaning|
|---|---|
|ax|destination|
|dx|value|
|cx|size|

---

## 🔹 Code

GLOBAL(memset)  
    pushw %di  
    movw %ax, %di  
    movzbl %dl, %eax  
    imull $0x01010101,%eax  
    pushw %cx  
    shrw $2, %cx  
    rep; stosl  
    popw %cx  
    andw $3, %cx  
    rep; stosb  
    popw %di  
    retl  
ENDPROC(memset)

---

# 🔍 Step-by-Step

---

## 🔹 1. Setup destination

movw %ax, %di

👉 DI = destination pointer

---

## 🔹 2. Prepare fill value

movzbl %dl, %eax

👉 Example:

dl = 0x07 → eax = 0x00000007

---

## 🔹 3. Expand to 4 bytes

imull $0x01010101, %eax

👉 Magic trick:

0x00000007 → 0x07070707

So we can fill 4 bytes at once

---

## 🔹 4. Fast fill (4 bytes)

shrw $2, %cx  
rep; stosl

👉 Fill memory 4 bytes at a time

---

## 🔹 5. Remaining bytes

andw $3, %cx  
rep; stosb

👉 Fill leftover 1–3 bytes

---

# 🧠 Visual

memset(ptr, 0x07, 10 bytes)  
  
→ [07070707] [07070707] [07][07]

---

# 🔥 serial_putchar()

If serial initialized:

- Uses `outb` / `inb`
- Writes to hardware I/O ports

👉 This bypasses BIOS → direct hardware access

---

# 🧠 Important Concepts You Just Learned

---

## 🔥 1. Early Debugging Infrastructure

Before kernel is ready:

- BIOS output
- Serial output

👉 This is how kernel devs debug boot issues

---

## 🔥 2. BIOS vs Hardware I/O

|Method|How|
|---|---|
|BIOS|interrupt (int 0x10)|
|Serial|port I/O (inb/outb)|

---

## 🔥 3. inittext section

__attribute__((section(".inittext")))

👉 Means:

- Used only during boot
- Freed later

---

## 🔥 4. Pattern: Optimized memory ops

Both:

- `memcpy`
- `memset`

Use:

- 4-byte chunks (fast)
- then leftovers

---

# 🧾 Clean Notes (Add this)

---

## 🔹 console_init()

- Parses `earlyprintk` from cmdline
- Initializes serial port if present

---

## 🔹 Early Output

puts → putchar

---

## 🔹 putchar()

- Converts `\n → \r\n`
- Calls:
    - `bios_putchar` (screen)
    - `serial_putchar` (if enabled)

---

## 🔹 BIOS Output

- Uses interrupt `0x10`
- AH = 0x0e → teletype mode

---

## 🔹 memset()

### Calling convention:

- ax → dest
- dx → value
- cx → size

### Algorithm:

1. Expand byte → 4 bytes
2. Fill 4-byte chunks
3. Fill remainder

---

## 🔹 Key Instructions

|Instruction|Meaning|
|---|---|
|stosl|store 4 bytes|
|stosb|store 1 byte|
|rep|repeat|

---

# 🧠 SUPER IMPORTANT INSIGHT (for you)

This stage shows:

👉 Kernel is still in a **weird hybrid world**:

- Using BIOS (real-mode style)
- But preparing for protected mode

---

Also VERY relevant for:

- 🧠 Reverse engineering bootkits
- 🧠 OSWE-style low-level bugs
- 🧠 Kernel exploitation (early boot assumptions)
