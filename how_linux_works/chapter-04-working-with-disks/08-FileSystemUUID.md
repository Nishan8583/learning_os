# 4.2.4 Filesystem UUID

## Why UUIDs Exist

Mounting filesystems using device names such as:

```bash
/dev/sda1
/dev/sdb1
/dev/sdc1
```

can be unreliable because device names depend on the order in which Linux discovers hardware during boot.

For example:

```
Today:
/dev/sdb1 = USB drive

Tomorrow:
/dev/sdc1 = USB drive
```

If Linux discovers devices in a different order, the device name can change.

To solve this problem, filesystems are assigned a **UUID (Universally Unique Identifier)**.

A UUID acts like a permanent serial number for a filesystem.

Example UUID:

```
b600fe63-d2e9-461c-a5cd-d3b373a5e1d2
```

Unlike device names, UUIDs stay the same even if the device becomes:

```
/dev/sdb1
/dev/sdc1
/dev/sdd1
```

---

## How UUIDs Are Created

When a filesystem is created using tools such as:

```
mke2fs
mkfs.ext4
```

Linux automatically generates a UUID and stores it in the filesystem metadata.

---

## Viewing UUIDs with blkid

Use:

```bash
sudo blkid
```

### Example Output

```
/dev/sdf2: UUID="b600fe63-d2e9-461c-a5cd-d3b373a5e1d2" TYPE="ext4"

/dev/sda1: UUID="17f12d53-c3d7-4ab3-943e-a0a72366c9fa" TYPE="ext4"
PARTUUID="c9a5ebb0-01"

/dev/sda5: UUID="b600fe63-d2e9-461c-a5cd-d3b373a5e1d2" TYPE="swap"
PARTUUID="c9a5ebb0-05"

/dev/sde1: UUID="4859-EFEA" TYPE="vfat"
```

---

## Understanding the Output

### ext4 Filesystem

```
/dev/sda1
UUID="17f12d53-c3d7-4ab3-943e-a0a72366c9fa"
TYPE="ext4"
```

- Linux native filesystem
- Uses a standard UUID

---

### Swap Partition

```
/dev/sda5
TYPE="swap"
```

Not a filesystem.

Instead, it is disk space used as virtual memory.

---

### FAT Filesystem

```
/dev/sde1
UUID="4859-EFEA"
TYPE="vfat"
```

FAT filesystems do not use the same UUID format as ext4.

Instead they use a:

```
Volume Serial Number
```

Example:

```
4859-EFEA
```

---

## UUID vs PARTUUID

### UUID

Identifies the filesystem itself.

```
UUID="17f12d53-c3d7-4ab3-943e-a0a72366c9fa"
```

### PARTUUID

Identifies the partition entry in the partition table.

```
PARTUUID="c9a5ebb0-01"
```

Think:

```
Disk
 └─ Partition
      └─ Filesystem
```

Partition → PARTUUID

Filesystem → UUID

---

## Mounting by UUID

Instead of:

```
sudo mount /dev/sda1 /home/extra
```

You can use:

```
sudo mount UUID=b600fe63-d2e9-461c-a5cd-d3b373a5e1d2 /home/extra
```

Example:

```
sudo mount UUID=b600fe63-d2e9-461c-a5cd-d3b373a5e1d2 /home/extra
```

Result:

```
Filesystem mounted at /home/extra
```

(no output if successful)

---

## Why UUID Mounting Is Useful

### 1. Reliable Booting

In:

```
/etc/fstab
```

Linux commonly uses UUIDs:

```
UUID=17f12d53-c3d7-4ab3-943e-a0a72366c9fa / ext4 defaults 0 1
```

Even if the disk becomes:

```
/dev/sdb1/dev/sdc1
```

the system still finds the correct filesystem.

---

### 2. Removable Media

When a USB drive is inserted:

```
UUID=4859-EFEA
```

Ubuntu might automatically mount it as:

```
/media/user/4859-EFEA
```

The device insertion event is handled by:

```
udevd
```

---

## Changing a UUID

If you clone a filesystem:

```
Original filesystem
       ↓ copy
Clone filesystem
```

Both may have the same UUID.

This can confuse Linux.

For ext2/ext3/ext4 filesystems, UUIDs can be changed using:

```
tune2fs
```

Example:

```
sudo tune2fs -U random /dev/sdb1
```

Generates a new random UUID.

---

# 4.2.5 Disk Buffering, Caching, and Filesystems

## Disk Write Buffering

When a program writes a file:

```
echo hello > file.txt
```

Linux usually does **not** immediately write data to the disk.

Instead:

```
Program
   ↓
Kernel RAM Buffer
   ↓
Disk (later)
```

The kernel waits until an efficient time to perform disk I/O.

Benefits:

- fewer disk operations
- better performance
- faster applications

---

## Why This Matters

Suppose you copy a file:

```
cp huge.iso /mnt/usb
```

The command may finish before all data has actually reached the USB drive.

Some data may still be in memory buffers.

Removing the drive too early can corrupt data.

---

## sync Command

To force Linux to flush buffers to disk:

```
sync
```

Example:

```
sync
```

Output:

```
(no output)
```

The kernel writes buffered data to disk.

---

## Unmount Automatically Syncs

When you unmount:

```
sudo umount /mnt/usb
```

Linux automatically performs a sync operation.

Process:

```
Flush buffers
    ↓
Write to disk
    ↓
Unmount filesystem
```

---

## Safe Removal Rule

Before removing media:

```
sudo umount /mnt/usb
```

If unmounting isn't possible:

```
sync
```

at minimum.

---

# Disk Read Caching

Linux also caches data it reads.

Without cache:

```
Program
   ↓
Disk
   ↓
Program
```

Every read requires disk access.

---

With cache:

```
First read:
Disk → RAM Cache

Future reads:
RAM Cache → Program
```

RAM is much faster than disk.

Result:

- less disk activity
- faster file access
- improved performance

---

## Example

Reading the same file repeatedly:

```
cat large.log
cat large.log
cat large.log
```

The first read may come from disk.

Subsequent reads often come from RAM cache.

---

# 4.2.6 Filesystem Mount Options

Mount options modify mount behavior.

General syntax:

```
mount -o option1,option2 device mountpoint
```

Example:

```
mount -o ro /dev/sdb1 /mnt
```

Mounts read-only.

---

# General vs Filesystem-Specific Options

### General Options

Work on most filesystems.

Examples:

```
-r
-n
-t
ro
rw
```

---

### Filesystem-Specific Options

Only work on particular filesystems.

Examples:

```
uid=1000
gid=1000
utf8
```

(vfat options)

---

# Remount Example

Convert an existing read-only mount to read-write:

```
sudo mount -o remount,rw /mnt
```

Meaning:

```
remount = do not unmount
rw      = make writable
```

---

# Important Short Options

## -r (Read Only)

Mount filesystem read-only.

```
sudo mount -r /dev/sdb1 /mnt
```

Equivalent to:

```
sudo mount -o ro /dev/sdb1 /mnt
```

---

### Use Cases

- Protect data
- Forensics
- Recovery work
- Damaged filesystem inspection

---

## -n (Don't Update /etc/mtab)

```
sudo mount -n /dev/sdb1 /mnt
```

Normally mount updates:

```
/etc/mtab
```

which tracks mounted filesystems.

The `-n` option prevents this update.

Useful during:

- boot process
- rescue mode
- single-user mode

when `/etc/mtab` may not yet be writable.

---

## -t (Filesystem Type)

Specify filesystem type explicitly.

Examples:

```
sudo mount -t ext4 /dev/sda1 /mnt
```

```
sudo mount -t xfs /dev/sdb1 /mnt
```

```
sudo mount -t vfat /dev/sdc1 /mnt
```

---

# Long Options

Long options follow `-o`.

Syntax:

```
mount -o option1,option2,...
```

---

## Example

```
sudo mount -t vfat /dev/sde1 /dos -o ro,uid=1000
```

Meaning:

```
Filesystem type = vfatRead onlyOwner = UID 1000
```

---

## Long Option: exec / noexec

### exec

Allow program execution.

```
sudo mount -o exec /dev/sdb1 /mnt
```

---

### noexec

Block execution.

```
sudo mount -o noexec /dev/sdb1 /mnt
```

Useful for:

- USB drives
- shared storage
- security hardening

---

## Long Option: suid / nosuid

### suid

Allow setuid programs.

```
sudo mount -o suid /dev/sdb1 /mnt
```

---

### nosuid

Ignore setuid bits.

```
sudo mount -o nosuid /dev/sdb1 /mnt
```

Common security measure for removable media.

---

## Long Option: ro

Read-only.

```
sudo mount -o ro /dev/sdb1 /mnt
```

---

## Long Option: rw

Read-write.

```
sudo mount -o rw /dev/sdb1 /mnt
```

Usually the default.

---

# DOS vs Unix Newlines

## Unix Line Ending

Uses:

```
\n
```

ASCII:

```
0x0A
```

Example:

```
hello\nworld\n
```

---

## DOS/Windows Line Ending

Uses:

```
\r\n
```

ASCII:

```
0x0D 0x0A
```

Example:

```
hello\r\nworld\r\n
```

---

## Why It Matters

Files created on Windows may appear strange on Linux tools.

Historically many filesystems tried automatic conversion, but it caused problems.

Modern editors such as:

- vim
- neovim
- VS Code

can detect and preserve the correct newline style automatically.