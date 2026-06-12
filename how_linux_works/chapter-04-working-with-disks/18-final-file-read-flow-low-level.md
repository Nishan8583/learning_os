# 0. Starting point in user space (C program)

```c
int fd = open("file.txt", O_RDONLY);
read(fd, buf, 4096);
```

At this moment:

- `fd` → points to a **struct file** in kernel
- `struct file` → references a **dentry**
- dentry → references an **inode**

So before `read()` even runs:

### Key kernel objects already set up:

- `file descriptor table (fd table)`
- `struct file`
- `struct dentry` (directory entry cache)
- `struct inode` (metadata: file size, block pointers, permissions)

---

# 1. System call entry (CPU + interrupt/trap)

When `read()` is called:

### User → Kernel transition:

- CPU executes `syscall` instruction (x86_64)
- Switches from:
    - Ring 3 (user mode)
    - → Ring 0 (kernel mode)

### CPU actions:

- Saves registers to kernel stack
- Jumps to syscall entry point:
    - `entry_SYSCALL_64` (Linux x86_64)

---

# 2. Syscall dispatch

Kernel does:

```c
sys_read()
```

Flow:

```
sys_read
 └── ksys_read
     └── vfs_read
```

At this point Linux enters **VFS (Virtual File System layer)**.

---

# 3. VFS layer (core abstraction layer)

## Function:

```c
vfs_read(file, buf, count)
```

VFS does:

### 3.1 Permission checks

- checks `file->f_mode`
- checks credentials (UID/GID)
- checks LSM hooks (SELinux/AppArmor)

### 3.2 File position

- uses `file->f_pos`

### 3.3 Calls filesystem-specific read

```c
file->f_op->read_iter()
```

So now control goes to the actual filesystem driver.

---

# 4. Filesystem layer (ext4 / xfs / btrfs)

Assume **ext4** for simplicity.

---

## 4.1 inode lookup (important part)

If file was not cached:

### Path resolution (from open time already done):

```
/home/user/file.txt
```

Steps:

### Directory traversal:

1. root inode `/`
2. lookup `"home"`
3. lookup `"user"`
4. lookup `"file.txt"`

Each lookup uses:

- directory inode
- directory data blocks
- hash tree (HTREE in ext4)

---

## 4.2 inode structure

An inode contains:

- file size
- permissions
- timestamps
- **block mapping info**

For ext4:

### block mapping types:

- direct blocks
- indirect blocks
- extent tree (modern ext4)

Example:

```
logical file block → physical disk block
```

So filesystem resolves:

```
read(offset 0–4096)
→ logical block 0
→ physical block 0x1A3F
```

---

## 4.3 page cache check (VERY important)

Before disk access:

### Kernel checks Page Cache:

```
address_space (inode cache)
 └── radix tree / xarray lookup
```

### If cache hit:

- data returned immediately (NO disk I/O)

### If cache miss:

- triggers block I/O request

---

# 5. Block layer (generic I/O layer)

Filesystem now calls:

```c
submit_bio()
```

BIO = Block I/O structure:

Contains:

- sector numbers
- read/write flag
- pages to fill
- completion callback

---

## 5.1 BIO submission path

```
submit_bio
 └── generic_make_request
     └── blk_mq_make_request
```

At this point Linux sends request into block subsystem.

---

# 6. DEVICE MAPPER LAYER (ONLY if LVM exists)

Now we split into two flows.

---

# A. WITHOUT LVM (direct disk path)

## 6A.1 Request goes directly to block device

```
block layer
 → /dev/sda2 (partition)
 → scsi/ata driver
```

### request queue:

- merges adjacent IO
- schedules request

---

# 7A. Disk driver layer

Example: NVMe or SATA

### NVMe path:

```
nvme_make_request → NVMe submission queue
```

Driver:

- converts BIO → NVMe command
- places it in submission queue

---

# 8A. Hardware (disk)

### NVMe SSD:

- controller DMA reads data
- fetches from NAND flash
- writes into RAM buffer (DMA memory)

### Then:

- interrupt generated (MSI-X interrupt)

---

# 9A. Interrupt handling

CPU receives interrupt:

```
nvme_irq_handler
```

Steps:

- identify completed request
- mark BIO as complete
- trigger callback

---

# 10A. Back up stack (completion path)

```
bio_end_io
 └── end_bio
     └── vfs callback
```

---

# 11A. Return to user space

- data now in page cache
- copied to user buffer (`copy_to_user`)
- `read()` returns

---

# B. WITH LVM (Device Mapper in between)

Now we insert **LVM layer (dm-crypt / dm-linear / dm-thin)**

---

# 6B. Device Mapper (LVM core)

Instead of:

```
block layer → /dev/sda2
```

We have:

```
block layer → /dev/mapper/vg-lv
```

---

## 6B.1 dm (device mapper target)

BIO goes into:

```
dm_make_request
```

Then:

```
device-mapper core
```

---

## 6B.2 Logical Volume mapping

LVM maintains:

### mapping table:

```
Logical block → Physical Extent (PE)
```

Example:

```
LV block 0 → VG extent 1000 → /dev/sda2 sector 500000
```

So LVM does:

1. takes BIO request
2. translates logical sector
3. rewrites BIO target device

---

## 6B.3 After mapping

Now request becomes:

```
BIO → /dev/sda2 (or multiple PVs)
```

Then continues exactly like **section 7A onward**

---

# 7B. Block device driver (same as non-LVM)

Now identical:

- request scheduler
- NVMe/SATA driver
- DMA transfer

---

# 8B–10B same as normal disk path

Hardware → interrupt → completion → page cache → user space

---

# KEY DIFFERENCE SUMMARY (VERY IMPORTANT)

## Without LVM:

```
VFS
 → filesystem (ext4)
 → block layer
 → disk driver
 → hardware
```

---

## With LVM:

```
VFS
 → filesystem (ext4)
 → block layer
 → DEVICE MAPPER (LVM)
 → block layer again
 → disk driver
 → hardware
```

---

# IMPORTANT INTERNAL CONCEPTS (what matters most)

## 1. inode lookup

- maps filename → metadata
- contains extent tree (file → disk mapping)

---

## 2. page cache

- avoids disk reads entirely if data is cached

---

## 3. BIO abstraction

- unit of I/O in Linux block layer

---

## 4. device mapper (LVM)

- translates logical → physical block addresses
- completely transparent to filesystem

---

## 5. interrupt-driven completion

- disk never polled (usually)
- completion via IRQ → softirq → callback

---

# ONE-LINE MENTAL MODEL

> `read()` = syscall → VFS → inode → page cache → BIO → (LVM mapping if present) → driver → disk DMA → interrupt → page cache → user buffer