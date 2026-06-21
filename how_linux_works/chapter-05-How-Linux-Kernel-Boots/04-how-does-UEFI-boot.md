# How does UEFI boot 
**UEFI does not use the MBR boot mechanism at all.** It does **not** read the first 512 bytes of the disk and execute them like BIOS does.

### BIOS

When booting:

```
Power On
   ↓
BIOS
   ↓
Read sector 0 (512-byte MBR)
   ↓
Execute boot code found there
```

The first sector is critical because that's where the boot process starts.

---

### UEFI

When booting:

```
Power On
   ↓
UEFI Firmware
   ↓
Read UEFI boot entries from NVRAM
   ↓
Locate EFI System Partition (ESP)
   ↓
Load .efi file
   ↓
Execute it
```

Example:

```
EFI System Partition (FAT32)

EFI/
├── Microsoft/
│   └── Boot/
│       └── bootmgfw.efi
└── ubuntu/
    ├── shimx64.efi
    └── grubx64.efi
```

UEFI loads something like:

```
EFI/ubuntu/grubx64.efi
```

directly as a file.

---

### Why the ESP Exists

UEFI expects a special partition called the:

**EFI System Partition (ESP)**

Entity: EFI System Partition

Characteristics:

- Usually FAT32
- Typically 100–500 MB
- Contains `.efi` executables
- Mounted in Linux at:

```
/boot/efi
```

or sometimes:

```
/efi
```

---

### BIOS vs UEFI Side-by-Side

#### BIOS

```
Disk
├── MBR (512 bytes)
│   └── GRUB Stage 1
├── GRUB Core
├── /boot
└── /
```

BIOS must start from the MBR sector.

---

#### UEFI

```
Disk
├── ESP (FAT32)
│   └── EFI/ubuntu/grubx64.efi
├── /boot
└── /
```

No MBR boot code required.

---

### Does an MBR-Partitioned UEFI Disk Still Have Sector 0?

Yes.

Every disk still has:

```
LBA 0
```

(the first 512-byte sector).

If the disk uses MBR partitioning, that sector contains:

```
Boot code area
Partition table
Signature
```

But during a UEFI boot:

```
UEFI ignores the boot code
```

It uses the ESP and `.efi` files instead.

So on a UEFI system with an MBR-partitioned disk:

```
Sector 0 exists
        ↓
Contains MBR
        ↓
But UEFI normally does not execute it
```

That's the key difference:

- **BIOS boot** → starts from the MBR sector.
- **UEFI boot** → starts from an `.efi` file in the ESP.

---
# How does UEFI know where the ESP partition is ?

## BIOS

BIOS doesn't know about partitions or filesystems in any meaningful way.

It simply does:

```
Read sector 0
Execute it
```

That's it.

---

## UEFI

UEFI is much smarter.

UEFI contains code that can:

- Read partition tables (GPT and usually MBR)
- Read FAT filesystems
- Read files and directories
- Store boot entries in NVRAM

So when the machine starts:

```
UEFI
   ↓
Read partition table
   ↓
Find EFI System Partition (ESP)
   ↓
Mount FAT filesystem
   ↓
Load .efi file
```

---

## How does it find the ESP?

### GPT disks

GPT partitions have a **partition type GUID**.

The ESP is marked with a special type:

```
EFI System Partition
GUID:
C12A7328-F81F-11D2-BA4B-00A0C93EC93B
```

UEFI scans the GPT table:

```
Partition 1 -> ESP GUID
Partition 2 -> Linux root
Partition 3 -> Home
```

and immediately knows:

```
Partition 1 is the ESP
```

---

### MBR disks

There is no GUID.

Instead the partition is usually marked with a special partition type:

```
0xEF
```

which means:

```
EFI System Partition
```

UEFI can recognize that as well.

---

## But how does it know WHICH .efi file to run?

This is where **UEFI NVRAM** comes in.

UEFI stores boot entries in motherboard flash memory.

You can see them on Linux:

```
efibootmgr -v
```

Example:

```
Boot0000 Ubuntu
HD(1,GPT,...)
/EFI/ubuntu/shimx64.efi

Boot0001 Windows
HD(1,GPT,...)
/EFI/Microsoft/Boot/bootmgfw.efi
```

Notice that the firmware remembers:

```
Partition
+
Path to .efi file
```

---

## Complete UEFI Boot Example

Suppose your disk looks like:

```
Disk
├── Partition 1 (ESP)
├── Partition 2 (Linux)
└── Partition 3 (Home)
```

ESP contents:

```
EFI/
├── ubuntu/
│   └── grubx64.efi
└── Microsoft/
    └── bootmgfw.efi
```

NVRAM contains:

```
BootOrder:
  Ubuntu
  Windows
```

Boot process:

```
Power On
   ↓
UEFI
   ↓
Read NVRAM
   ↓
Ubuntu entry selected
   ↓
Go to Partition 1 (ESP)
   ↓
Open EFI/ubuntu/grubx64.efi
   ↓
Execute GRUB
```

No MBR code involved.

---

## What if NVRAM is missing?

UEFI has a fallback path.

Most firmware will try:

```
\EFI\BOOT\BOOTX64.EFI
```

on the ESP.

That's why many Linux installation USBs contain:

```
EFI/
└── BOOT/
    └── BOOTX64.EFI
```

Even if the firmware has no boot entries configured, it can still find the bootloader.

---

### The short answer

**UEFI knows where the ESP is because it can read the disk's partition table (GPT or MBR). It knows which `.efi` file to execute because the path is stored in UEFI NVRAM boot entries (or it falls back to `EFI/BOOT/BOOTX64.EFI`).**