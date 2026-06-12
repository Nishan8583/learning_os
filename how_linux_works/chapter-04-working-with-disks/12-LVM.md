# 4.4 The Logical Volume Manager (LVM) — Notes

## Core Idea

Traditional disk management uses **partitions directly on physical disks**, e.g.:

```
/dev/sda1 → partition → filesystem
```

This approach is rigid because resizing or changing disks often requires:

- repartitioning
- reinstalling filesystems
- moving data manually
- multiple reboots

---

## Problem with Traditional Partitioning

Example upgrade scenario:

```
Old disk → new disk migration
```

Steps usually required:

- install new disk
- create partitions
- format filesystems
- update bootloader
- copy data
- reboot multiple times

### Adding a new disk is also inconvenient:

- new mount points must be chosen
- data must be manually distributed
- no easy resizing

---

# Solution: Logical Volume Manager (LVM)

LVM introduces an **abstraction layer between hardware and filesystem**.

## Architecture

```
Physical Volumes (PV)
        ↓
Volume Group (VG)
        ↓
Logical Volumes (LV)
        ↓
Filesystems / swap
```

---

## Key Concepts

### 1. Physical Volume (PV)

A physical storage device:

- disk partition
- whole disk

Example:

```
/dev/sda1
/dev/sdb1
```

---

### 2. Volume Group (VG)

A storage pool made from PVs.

Think:

```
VG = big virtual disk created from multiple real disks
```

---

### 3. Logical Volume (LV)

Virtual block devices carved from the VG.

They behave like partitions:

- contain filesystems
- contain swap
- mountable devices

---

# Advantages of LVM

LVM allows:

### 1. Add storage dynamically

```
Add new disk → expand VG → extend LV
```

---

### 2. Remove disks (if space allows)

```
Remove PV only if VG still has enough space
```

---

### 3. Resize logical volumes online

- no reboot required
- often no unmount required

Tool:

```
fsadm
```

---

### 4. Cloud-friendly flexibility

- attach new disks dynamically
- resize storage without downtime

---

# 4.4.2 Working with LVM

LVM provides CLI tools:

- `vgs` → volume groups
- `lvs` → logical volumes
- `vgdisplay` → detailed VG info
- `lvdisplay` → detailed LV info
- `lvm` → interactive shell

Many commands are symlinks to `lvm`.

---

# Listing Volume Groups

## Command

```
vgs
```

## Example Output

```
VG         #PV #LV #SN Attr   VSize   VFree
ubuntu-vg   1   2   0  wz--n- <10.00g 36.00m
```

---

## Column Meaning

| Column | Meaning                                  |
| ------ | ---------------------------------------- |
| VG     | Volume group name                        |
| `#PV`  | Number of physical volumes               |
| `#LV`  | Number of logical volumes                |
| `#SN`  | Snapshots                                |
| Attr   | Status flags (writable, resizable, etc.) |
| VSize  | Total size of VG                         |
| VFree  | Free space available                     |

---

# Detailed Volume Group Info

## Command

```
vgdisplay
```

## Example Output

```
--- Volume group ---
VG Name               ubuntu-vg
Format                lvm2
Metadata Areas        1
VG Access             read/write
VG Status             resizable
Cur LV                2
Open LV               2
Cur PV                1
VG Size               <10.00 GiB
PE Size               4.00 MiB
Total PE              2559
Alloc PE / Size       2550 / 9.96 GiB
Free PE / Size        9 / 36.00 MiB
VG UUID               0zs0TV-wnT5-laOy-vJ0h-rUae-YPdv-pPwaAs
```

---

## Important Concepts

### Open LV

```
Logical volumes currently in use
```

---

### Physical Extent (PE)

Small allocation unit inside VG:

```
PE size = 4MB
```

Think:

```
Disk → blocks → partitions → PE (LVM level)
```

---

### UUID

Each VG has a unique identifier:

```
VG UUID = unique volume group identity
```

Used when names conflict.

---

# Listing Logical Volumes

## Command

```
lvs
```

## Example Output

```
LV     VG         Attr       LSize
root   ubuntu-vg  -wi-ao---- <9.01g
swap_1 ubuntu-vg  -wi-ao---- 976.00m
```

---

## Column Meaning

|Column|Meaning|
|---|---|
|LV|Logical volume name|
|VG|Volume group name|
|Attr|Status flags|
|LSize|Size of LV|

---

# Detailed Logical Volume Info

## Command

```
lvdisplay /dev/ubuntu-vg/root
```

## Example Output

```
--- Logical volume ---
LV Path                /dev/ubuntu-vg/root
LV Name                root
VG Name                ubuntu-vg
LV UUID                CELZaz-PWr3-tr3z-dA3P-syC7-KWsT-4YiUW2
LV Status              available
# open                 1
LV Size                <9.01 GiB
Current LE             2306
Segments               1
Block device           253:0
```

---

## Key Fields

### LV Path

```
/dev/ubuntu-vg/root
```

Represents the logical device path used by the system.

---

### Device Mapping Reality

This path is often a symlink:

```
ls -l /dev/ubuntu-vg/root
```

Example output:

```
lrwxrwxrwx 1 root root 7 Nov 14 06:58 /dev/ubuntu-vg/root -> ../dm-0
```

---

# Device Mapper Layer

LVM uses kernel **device-mapper**, exposing:

```
/dev/dm-0
/dev/dm-1
```

These are the real block devices.

---

## Why symbolic links exist

Because `/dev/dm-0` is:

- unstable (can change on reboot)
- hard to identify

So LVM creates stable links:

### 1. Traditional path

```
/dev/ubuntu-vg/root
```

### 2. Mapper path

```
/dev/mapper/ubuntu--vg-root
```

Note:

```
- single dash becomes double dash in VG name
- VG-LV separation uses single dash
```

---

# Using Logical Volumes

Logical volumes behave like normal block devices:

```
mkfs.ext4 /dev/ubuntu-vg/root
mount /dev/ubuntu-vg/root /mnt
```

or swap:

```
mkswap /dev/ubuntu-vg/swap_1swapon /dev/ubuntu-vg/swap_1
```

---

# Summary Table

|Layer|Example|Description|
|---|---|---|
|PV|`/dev/sda1`|Physical storage|
|VG|`ubuntu-vg`|Storage pool|
|LV|`root`, `swap_1`|Virtual partitions|
|Device|`/dev/dm-0`|Kernel mapping layer|
|Symlink|`/dev/ubuntu-vg/root`|Stable access path|

---

# Key Takeaways

- LVM adds abstraction over physical disks
- You no longer manage fixed partitions directly
- Storage becomes flexible and resizable
- Changes can be done mostly without rebooting
- Logical volumes behave like real block devices
- Device mapper (`dm-*`) is the kernel backend


---

# My question
```
Explain the mapper layer to me, Are the physical device exposed to us via layer like /dev/dm-o u mean ?
```
Yes — you’re very close, but the key idea is slightly different:

### Short answer

No, `/dev/dm-*` is **not the physical disk exposed directly to you**.

It is a **virtual block device created by the kernel’s device-mapper layer**, which sits _on top of_ real disks (like `/dev/sda1`).

---

# 🧠 Mental Model: Where `/dev/dm-*` fits

Think of storage as layers:

```
Filesystem (ext4, xfs)
        ↓
Logical Volume (LVM)
        ↓
Device Mapper (/dev/dm-0)
        ↓
Physical Volumes (/dev/sda1, /dev/sdb1)
        ↓
Actual disk hardware
```

So:

- `/dev/sda1` → real partition
- `/dev/dm-0` → **virtual device created by the kernel**
- LVM → decides how `/dev/dm-0` is built from real disks

---

# 🔧 What is the device-mapper?

The **device-mapper (DM)** is a Linux kernel subsystem that lets the kernel:

> Create virtual block devices by mapping requests to one or more real block devices.

So instead of “disk → filesystem”, you get:

```
disk(s) → mapping rules → virtual disk (/dev/dm-0)
```

---

# 📦 What `/dev/dm-0` actually is

When LVM creates a logical volume:

```
/dev/ubuntu-vg/root
```

This is really just a symlink to:

```
/dev/dm-0
```

Example:

```
ls -l /dev/ubuntu-vg/root
```

Output:

```
/dev/ubuntu-vg/root -> ../dm-0
```

---

# ⚙️ So is `/dev/dm-0` a physical device?

❌ No.

It is:

- not a disk
- not a partition
- not hardware-backed directly

✔ It is:

- a **virtual block device**
- backed by real disks underneath
- managed by kernel mappings

---

# 🧱 How a read/write works (important)

Let’s say you do:

```
cat /dev/ubuntu-vg/root/file.txt
```

Flow:

```
User process
    ↓
Filesystem (ext4)
    ↓
/dev/ubuntu-vg/root (LV)
    ↓
Device Mapper (/dev/dm-0)
    ↓
LVM mapping table
    ↓
/dev/sda1 + /dev/sdb1 (physical storage)
    ↓
Disk hardware
```

So `/dev/dm-0` is just the **translation layer executor**.

---

# 🧩 What device-mapper actually stores

The kernel keeps a **mapping table** like:

```
dm-0:
    start 0-5000  → /dev/sda1
    start 5000-9000 → /dev/sdb1
```

So `/dev/dm-0` is basically:

> “A virtual disk composed of these chunks from these real devices”

---

# 🔁 Why LVM uses device-mapper

Because it enables powerful features:

### 1. Resizing

You can grow a logical volume without touching partitions.

### 2. Combining disks

One LV can span multiple physical disks.

### 3. Snapshots

You can create copy-on-write views of a disk.

### 4. Flexibility

Everything is abstracted into mappings instead of fixed geometry.

---

# 📁 Why you also see `/dev/mapper/*`

Besides `/dev/dm-0`, you’ll see:

```
/dev/mapper/ubuntu--vg-root
```

This is just:

- a **human-friendly name**
- pointing to the same `/dev/dm-0`

So you typically interact with:

✔ preferred:

```
/dev/mapper/ubuntu--vg-root
```

✔ also valid:

```
/dev/ubuntu-vg/root
```

❌ low-level:

```
/dev/dm-0
```

---

# 🧠 Key takeaway

### Think of it like this:

|Layer|Meaning|
|---|---|
|`/dev/sda1`|Real disk partition|
|device-mapper|Kernel “virtual disk builder”|
|`/dev/dm-0`|Virtual disk created by mappings|
|LVM LV|Named interface to `/dev/dm-0`|

---

# 🚨 Common misconception

> “Is dm-0 exposing the physical device?”

❌ No.

It is the opposite:

> It hides physical devices behind a virtual abstraction.

---

# 👍 Simple analogy

Imagine:

- `/dev/sda1` = bricks
- device-mapper = construction blueprint
- `/dev/dm-0` = a house built from bricks
- LVM = architect naming the house (“root”, “swap”)