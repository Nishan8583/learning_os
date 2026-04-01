# Initial Kernel Tasks (after main())
- for detail go in this folder [folder](./003-first-step-into-the-kernel/)

### 🔹 Copy Boot Parameters (zeropage)

- Kernel copies bootloader-provided data into its own structure:

`boot_params.hdr = hdr;`

Source:

- `hdr from header.S`

Destination:

`boot_params.hdr (in zeropage)`

---

➕ Purpose
- Preserve bootloader data
- Normalize boot protocol format
- Prepare for later kernel use
---

➕ Command Line Fix

If old protocol used:
- Kernel updates command line pointer

### 🔹 memcpy (Real Mode)

Arguments passed via registers:
```
ax → destination  
dx → source  
cx → size  
```

---

🔹 Algorithm
```
Copy 4 bytes at a time:
    cx / 4 → rep movsl
Copy remaining bytes:
    cx % 4 → rep movsb
```


### 🔹 Console Initialization (console_init)

Kernel sets up early output system.

🔹 Parse kernel cmdline

Looks for:

`earlyprintk=`

Examples:
```
serial,0x3f8,115200  
ttyS0,115200  
```

🔹 Serial Port Setup

If found:
- Initialize serial port
- Store base in:
`early_serial_base`

🔹 First Debug Output
```c
if (debug)
    puts("early console in setup code\n");
```
---

🔹 Output System
🔹 puts → putchar loop
for each character:
    putchar(ch)
🔹 putchar behavior
if '\n' → print '\r' first

Print to:
    1. BIOS (screen)
    2. Serial (if enabled)
🔹 BIOS Output
INT 0x10 (video interrupt)

AH = 0x0e → teletype mode  
AL = character  
🔹 Serial Output
Uses outb / inb
Direct hardware I/O

---

🔹 initregs()

Initializes BIOS register structure:

`memset(reg, 0, sizeof)`

Then sets:
- segment registers (ds, es, fs, gs)
- flags
---

🔹 memset (Real Mode)

Arguments:
```
ax → destination  
dx → value  
cx → size  
```
🔹 Algorithm
```
Expand value:
    0x07 → 0x07070707

Fill 4 bytes:
    rep stosl

Fill remainder:
    rep stosb
```

---
### 🔹 Heap Initialization (init_heap)
🔹 Check if heap allowed
if (loadflags & CAN_USE_HEAP)
🔹 Stack boundary
stack_end = esp - STACK_SIZE
🔹 Heap boundary
heap_end = heap_end_ptr + 0x200
🔹 Prevent overlap
if (heap_end > stack_end)
    heap_end = stack_end
🔹 Memory Layout
`[ kernel ][ heap ↑ ][ free ][ stack ↓ ]`

➕ Notes
No malloc yet
Simple linear heap
Uses GET_HEAP later

---
### 🔹 CPU Validation (validate_cpu)
🔹 Check CPU level
if cpu_level < required → fail
🔹 Checks include
CPU generation (386+)
Vendor (Intel/AMD)
Features:
long mode (64-bit)
SSE/SSE2
➕ Purpose

Ensure kernel runs on supported hardware

🔹 BIOS Mode Setup (set_bios_mode)

(Only for x86_64)
---
🔹 BIOS call
```
INT 0x15
AX = 0xec00  
BX = 2 → long mode
```
➕ Purpose

Notify BIOS:
- Kernel will use 64-bit mode

### 🔹 Memory Detection (detect_memory)
🔹 Uses BIOS E820 interface
INT 0x15, AX = 0xE820
🔹 Register setup
AX = 0xE820  
CX = buffer size  
EDX = "SMAP"  
ES:DI = buffer address  
EBX = continuation value  
🔹 Loop
Call BIOS → get entry  
EBX = next pointer  
Repeat until done
🔹 Each entry contains
[start address]  
[size]  
[type]
🔹 Types
usable → free RAM  
reserved → do not use  
🔹 Result

Stored in:

boot_params.e820_map
🔹 Keyboard Initialization (keyboard_init)
🔹 Get keyboard status
INT 0x16  
AH = 0x02
🔹 Set repeat rate
AX = 0x0305  
INT 0x16
🔹 Store result
boot_params.kbd_status
🔹 BIOS Queries
🔹 Intel SpeedStep (query_ist)
Uses INT 0x15
Stores CPU power info
🔹 APM (query_apm_bios)
INT 0x15, AH = 0x53

Checks:

signature = 0x504d ("PM")
carry flag = 0
CX = 0x02 → protected mode supported
🔹 Switch to 32-bit interface
AX = 0x5304  
INT 0x15
🔹 Store result
boot_params.apm_bios_info
🔹 EDD (Disk Info) (query_edd)
🔹 Loop devices
0x80 → 0x8F  (max 16 disks)
🔹 Check support
INT 0x13, AH = 0x41
🔹 Get info
INT 0x13, AH = 0x48
🔹 Store result
boot_params.eddbuf
🔹 Final State Before Protected Mode

At this point:

✔ boot params copied  
✔ console ready  
✔ heap initialized  
✔ CPU validated  
✔ memory map known  
✔ keyboard initialized  
✔ hardware info collected  
🧠 Final Mental Model
Kernel is still in REAL MODE

Uses BIOS for:
    video (0x10)
    disk (0x13)
    system (0x15)
    keyboard (0x16)

Collects ALL hardware info

Prepares memory + CPU

➡ Next step: switch to PROTECTED MODE
