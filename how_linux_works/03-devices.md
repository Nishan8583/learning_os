# Chapter 3 Notes: Devices in Linux

---

# 3.1 Device Files

## Overview

Linux follows the Unix philosophy of treating many hardware devices as files.

These special files are called:

- **Device files**
- **Device nodes**

Location:

```
/dev
```

This design allows programs to interact with hardware using normal file operations:

- read()
- write()
- open()
- close()

Some devices can even be used directly from the shell with standard commands like:

```
catechodd
```

### Important Limitation

Not every device fits the file abstraction.

Examples:

- Hard disks → work well as device files
- Printers → work well as device files
- Network interfaces → do **not** have device files

Linux provides other APIs for devices where file semantics are unsuitable.

---

## Example: Writing to `/dev/null`

### Command

```
$ echo blah blah > /dev/null
```

### What happens?

1. `echo` writes text to stdout.
2. Output is redirected to `/dev/null`.
3. Kernel detects `/dev/null` is a device.
4. Kernel passes data to the `/dev/null` driver.
5. Driver discards all data.

Result:

```
blah blah
```

is thrown away.

### Uses

Commonly used to suppress output:

```
command > /dev/null
```

---

## Viewing Device Files

Use:

```
$ ls -l
```

### Example Output

```
brw-rw---- 1 root disk 8, 1 Sep 6 08:37 sda1
crw-rw-rw- 1 root root 1, 3 Sep 6 08:37 null
prw-r--r-- 1 root root 0 Mar 3 19:17 fdata
srw-rw-rw- 1 root root 0 Dec 18 07:43 log

```

---

## Device Type Identification

The first character indicates the file type.

|Character|Type|
|---|---|
|b|Block Device|
|c|Character Device|
|p|Named Pipe|
|s|Socket|

---

# Block Devices

Example:

```
/dev/sda1
```

Characteristics:

- Data stored in fixed-size blocks
- Random access supported
- Kernel can jump directly to any block
- Usually storage devices

Examples:

- HDDs
- SSDs
- USB drives

Advantages:

- Fast indexing
- Efficient seeking

---

# Character Devices

Examples:

```
/dev/null
/dev/tty
```

Characteristics:

- Stream-oriented
- No random access
- No fixed size
- Read/write sequentially

Examples:

- Terminals
- Printers
- Serial ports

Important:

Once data passes through the stream:

- Kernel cannot rewind
- Kernel cannot re-read previous data

---

# Pipe Devices

Named pipes (FIFOs).

Characteristics:

- Stream-based
- Another process is on the opposite end

Example:

```
Process A -> Pipe -> Process B
```

---

# Socket Devices

Used for:

- Inter-process communication (IPC)

Often located outside `/dev`.

Unix domain sockets appear as socket files.

Example:

```
/var/run/docker.sock
```

---

# Major and Minor Numbers

Example:

```
brw-rw---- 1 root disk 8,1 ...
```

Here:

```
8  = Major Number
1  = Minor Number
```

### Major Number

Identifies:

```
Device driver
```

### Minor Number

Identifies:

```
Specific device handled by driver
```

Example:

```
sda1
sda2
sdb1
```

may share a major number but have different minor numbers.

---

## Why Network Interfaces Don't Have Device Files

Network devices don't fit naturally into:

- Block device model
- Character device model

While technically possible, it would be awkward.

Therefore Linux provides specialized networking APIs instead.

---

# 3.2 The sysfs Device Path

## Why `/dev` Is Not Enough

Problems with `/dev`:

### Little Information

Example:

```
/dev/sda
```

Doesn't tell you:

- Physical location
- Controller
- Bus
- Hardware details

### Device Names Can Change

Linux discovers hardware during boot.

Device naming depends on discovery order.

Example:

```
Disk A -> /dev/sdaDisk B -> /dev/sdb
```

After hardware changes:

```
Disk B -> /dev/sda
```

This can break scripts and configurations.

---

## sysfs

Linux provides:

```
/sys
```

Device information lives under:

```
/sys/devices
```

---

## Example sysfs Path

```
/sys/devices/pci0000:00/0000:00:17.0/ata3/host0/target0:0:0/0:0:0:0/block/sda
```

Compared with:

```
/dev/sda
```

---

### Purpose Difference

`/dev/sda`

- Used to access the disk

`/sys/.../sda`

- Used to inspect and manage the disk

---

## Example Directory Contents

```
alignment_offset discard_alignment holders removable size
uevent
bdi events inflight ro slaves
capability events_async power sda1 stat
dev events_poll_msecs queue sda2 subsystem
device ext_range range sda5 trace
```

Most files are intended for programs rather than humans.

---

## Example: Device Numbers

Inside the sysfs directory:

```
$ cat dev
```

Output:

```
8:0
```

Meaning:

```
Major: 8Minor: 0
```

which corresponds to:

```
/dev/sda
```

---

## Shortcut Directory

```
/sys/block
```

Contains block devices.

Most entries are symbolic links.

Reveal actual paths:

```
$ ls -l /sys/block
```

---

## Finding a Device's sysfs Path

Use:

```
$ udevadm info --query=all --name=/dev/sda
```

This displays:

- sysfs path
- attributes
- udev information
- device metadata

---

# 3.3 dd and Devices

## What is dd?

`dd` copies data from:

- files
- devices
- streams

to:

- files
- devices
- streams

It is extremely useful when working with:

- disks
- partitions
- device files
- binary data

---

## Warning

`dd` can easily destroy data.

A typo in:

```
of=
```

can overwrite an entire disk.

Always verify:

```
if=of=
```

before pressing Enter.

---

## Example: Creating a File From `/dev/zero`

### Command

```
$ dd if=/dev/zero of=new_file bs=1024 count=1
```

### Breakdown

Input:

```
if=/dev/zero
```

Continuous stream of zero bytes.

Output:

```
of=new_file
```

Block size:

```
bs=1024
```

Copy:

```
count=1
```

one block.

---

### Result

Creates:

```
new_file
```

containing:

```
1024 zero bytes
```

---

## dd Syntax

Unlike most Unix commands:

```
command -option
```

`dd` uses:

```
option=value
```

Example:

```
dd if=input of=output
```

---

## Important dd Options

### Input File

```
if=file
```

Example:

```
if=/dev/zero
```

---

### Output File

```
of=file
```

Example:

```
of=image.bin
```

---

### Block Size

```
bs=size
```

Examples:

```
bs=1024
```

```
bs=1k
```

Abbreviations:

|Suffix|Size|
|---|---|
|b|512 bytes|
|k|1024 bytes|

---

### Separate Input and Output Sizes

```
ibs=sizeobs=size
```

Example:

```
ibs=512 obs=4096
```

---

### Count

```
count=num
```

Copies only specified blocks.

Example:

```
count=10
```

copies 10 blocks.

---

### Skip

```
skip=num
```

Skip initial blocks.

Example:

```
skip=100
```

starts reading at block 101.

Useful for extracting part of a large disk image.

---

# 3.4 Device Name Summary

Ways to identify devices:

---

## 1. Use udevadm (Best Method)

```
$ udevadm info --query=all --name=/dev/sda
```

Most reliable.

---

## 2. Explore sysfs

```
/sys
```

---

## 3. Check Kernel Messages

```
$ journalctl -k
```

Displays:

- hardware detection
- driver messages
- device discovery

---

## 4. Check Mounted Devices

```
$ mount
```

Shows mounted filesystems and backing devices.

---

## 5. View Registered Drivers

```
$ cat /proc/devices
```

Displays:

- block device drivers
- character device drivers
- major numbers

---

# 3.4.1 Hard Disks — `/dev/sd*`

Examples:

```
/dev/sda
/dev/sdb
/dev/sdc
```

Partitions:

```
/dev/sda1
/dev/sda2
/dev/sda3
```

---

## Why "sd"?

Stands for:

```
SCSI Disk
```

Even modern devices often use SCSI protocols internally:

- USB drives
- SATA drives

---

## Listing SCSI Devices

### Command

```
$ lsscsi
```

### Example Output

```
[0:0:0:0]1 disk2 ATA WDC WD3200AAJS-2 01.0 /dev/sda3
[2:0:0:0] disk FLASH Drive UT_USB20 0.00 /dev/sdb
```

### Columns

First column:

```
[0:0:0:0]
```

Device address.

Second column:

```
disk
```

Device type.

Last column:

```
/dev/sda
```

Device file.

---

## Device Renaming Problem

Example:

Before:

```
/dev/sda
/dev/sdb
/dev/sdc
```

Disk `/dev/sdb` fails.

After reboot:

```
/dev/sda
/dev/sdb
```

Old:

```
/dev/sdc
```

becomes:

```
/dev/sdb
```

This can break:

```
fstab
scripts
mount configs
```

Solutions:

- UUIDs
- LVM mappings

---

# 3.4.2 Virtual Disks

Prefixes:

```
/dev/xvd*
/dev/vd*
```

Common in:

- Xen
- AWS
- VirtualBox
- Other virtualized environments

---

# 3.4.3 NVMe Devices

Prefix:

```
/dev/nvme*
```

Used by:

```
NVMe SSDs
```

List devices:

```
$ nvme list
```

---

# 3.4.4 Device Mapper

Prefixes:

```
/dev/dm-*
```

and

```
/dev/mapper/*
```

Typically indicates:

```
LVM
```

or device-mapper usage.

---

# 3.4.5 CD/DVD Drives

Prefix:

```
/dev/sr*
```

Examples:

```
/dev/sr0
/dev/sr1
```

Characteristics:

- Read-only access
- Used for reading discs

Writing often uses:

```
/dev/sg0
```

(Generic SCSI device)

---

# 3.4.6 PATA Disks

Prefixes:

```
/dev/hda
/dev/hdb
/dev/hdc
/dev/hdd
```

Older hardware.

If a SATA disk appears here:

- Controller may be in compatibility mode.
- Performance may be reduced.

Check BIOS settings.

---

# 3.4.7 Terminals

Prefixes:

```
/dev/tty*
/dev/pts/*
```

---

## Virtual Console

Example:

```
/dev/tty1
```

First virtual console.

---

## Pseudoterminal

Example:

```
/dev/pts/0
```

Terminal emulator window.

Examples:

- GNOME Terminal
- xterm
- Konsole
- Alacritty

---

## `/dev/tty`

Special device:

```
Current process's controlling terminal
```

Example:

If a shell is attached to:

```
/dev/pts/0
```

then:

```
/ dev/tty
```

refers to:

```
/ dev/pts/0
```

for that process.

---

# Key Exam / Interview Takeaways

- Linux exposes many devices through files under `/dev`.
- Device types: block (`b`), character (`c`), pipe (`p`), socket (`s`).
- Block devices support random access; character devices are stream-based.
- `/dev/null` discards all written data.
- Major numbers identify drivers; minor numbers identify specific devices.
- `/sys` provides detailed hardware information via sysfs.
- `udevadm info --query=all --name=<device>` is the most reliable way to inspect a device.
- `dd` is the primary low-level tool for copying raw device data.
- Modern disks usually appear as `/dev/sd*`.
- Virtual disks often use `/dev/xvd*` or `/dev/vd*`.
- NVMe SSDs appear as `/dev/nvme*`.
- LVM/device mapper devices appear as `/dev/dm-*` and `/dev/mapper/*`.
- Terminal devices appear as `/dev/tty*` and `/dev/pts/*`.
- Device names can change between boots; UUIDs and LVM mappings help avoid this problem.