1. Power button -> supplies Power good signal (Proper amount of electricity to CPU)
2. CPU resets all leftover data in its registers and sets predefined values for each of them.
3. In real mode, PhysicalAddress = Segment Selector * 16 + Offset
4. CS register consists of two parts: the visible segment selector and the hidden base address.
CS register is loaded with 0xf000 and the base address is loaded with 0xffff0000
```
>>> 0xffff0000 + 0xfff0
'0xfffffff0'
```
- 0xfffffff0, which is 16 bytes below 4GB. This point is called the reset vector. It's the memory location at which the CPU expects to find the first instruction to execute after reset. It contains a jump (jmp) instruction that usually points to the BIOS 
5. BIOS does its initial POST.
6. BIOS tries to find boot sector, on MBR partition layout boot sector is stored in the first 446 bytes of the first sector, where each sector is 512 bytes.
7. final two bytes of the first sector are `0x55` and `0xaa`, which designates to the BIOS that this device is bootable.
8. At the beginning of this post, I wrote that the first instruction executed by the CPU is located at address 0xFFFFFFF0, which is much larger than 0xFFFFF (1MB). How can the CPU access this address in real mode? The answer is in the coreboot documentation:
`0xFFFE_0000 - 0xFFFF_FFFF: 128 kilobyte ROM mapped into address space`.
9. Bootloader does its thing, sets up necessary headers for kernel.
10. Bootloader loads the Linux kernel from disk into memory at a known address (commonly 0x10000).
```
```
0x00007C00 → bootloader (initial)
0x00010000 → kernel (real mode part)
```
```
11. Bootloader sets up CPU state (segment registers):
```

```
DS = ES = FS = GS = SS = 0x1000
CS = 0x1020   // execution starts at offset inside kernel
```

```
- CS is different because execution starts at an offset (not beginning of kernel)
12. Bootloader passes boot parameters to kernel via a header structure in memory:
boot_flag = 0xAA55
loadflags (contains CAN_USE_HEAP)
heap_end_ptr
kernel command line pointer
video mode, root device, etc
Bootloader enables A20 line to allow access to memory above 1MB (prevents wraparound behavior of real mode).

Bootloader transfers control to kernel entry point:

jmp 0x1020:0x0000
This jumps to _start in arch/x86/boot/header.S
🔹 Stack Setup (Kernel)

Kernel verifies whether stack is valid:

if (SS == DS)
    stack might be valid
else
    stack is unsafe → rebuild
Case 1: Stack is valid

Kernel aligns stack pointer (SP) to 4 bytes:

SP = SP & ~3

If SP becomes 0 after alignment:

SP = 0xfffc
This is the last safe aligned address in a 64KB segment

Kernel sets stack segment and pointer:

SS = 0x1000
SP = aligned value
Case 2: Stack is invalid

Kernel uses _end (end of kernel setup code) as safe memory location:

stack starts from _end

Kernel checks CAN_USE_HEAP flag from bootloader:

loadflags & (1 << 7)
Case 2A: Heap allowed

Stack is placed after heap:

SP = heap_end_ptr + 1024   // STACK_SIZE

Memory layout:

[ kernel ][ heap ][ stack ]
Case 2B: Heap not allowed

Minimal stack is created:

SP = _end + 1024

Memory layout:

[ kernel ][ stack ]

Kernel aligns SP and sets:

SS = 0x1000
SP = aligned value
🔹 Heap (Early Stage)
Heap is not fully initialized yet.

Bootloader only provides:

heap_end_ptr
Kernel uses heap only if CAN_USE_HEAP is set
No malloc or allocator exists yet
🔹 BSS Setup

BSS section contains uninitialized global/static variables:

int x;  // must be 0

Kernel zeros BSS manually:

di = __bss_start
cx = _end + 3
cx = cx - di
cx = cx / 4
eax = 0
rep stosl

Equivalent C logic:

memset(__bss_start, 0, _end - __bss_start);

Result:

All uninitialized globals are now zero
🔹 Transfer to Kernel C Code

After stack and BSS are ready, kernel calls:

calll main
This:
pushes return address onto stack

transfers execution to:

arch/x86/boot/main.c

At this point:

Segment registers are aligned
Stack is valid
BSS is zeroed
Execution continues in C code → actual kernel initialization begins
