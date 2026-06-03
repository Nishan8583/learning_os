# 4.2 Filesystems — Notes & Explanation

## Big Picture

A disk partition by itself is just a collection of blocks:

```
Block 0
Block 1
Block 2
Block 3
...
```

Humans don't want to work with block numbers.

A **filesystem** provides structure on top of those blocks and turns them into:

```
/
├── home
│   ├── nishan
│   │   └── notes.txt
├── etc
├── var
└── usr
```

The book describes a filesystem as:

> A database that organizes blocks into files and directories.

---

# The Linux Storage Stack

From bottom to top:

```
Application
    ↓
Filesystem
    ↓
Block Device
    ↓
Disk / SSD
```

Example:

```
cat myfile.txt
```

The filesystem translates:

```
myfile.txt
```

into:

```
Read blocks:102410251026
```

from the underlying device.

---

# Filesystems Aren't Just For Storage

Historically:

```
Filesystem → Disk
```

Today:

```
Filesystem → Many things
```

Examples:

|Filesystem|Purpose|
|---|---|
|ext4|Real storage|
|xfs|Real storage|
|proc|Process information|
|sysfs|Kernel/device information|
|tmpfs|RAM-backed storage|
|FUSE|User-space filesystem|

---

## Example: /proc

```
cat /proc/cpuinfo
```

looks like reading a file.

But:

```
No actual file exists on disk.
```

The kernel generates the contents dynamically.

---

## Example: /sys

```
cat /sys/block/sda/size
```

Again:

```
Looks like a file
```

but is actually an interface into kernel data structures.

---

# Virtual File System (VFS)

Linux supports many filesystems:

- ext4
- xfs
- btrfs
- ntfs
- fat
- iso9660

Applications should not need to care which one is being used.

The solution is:

## VFS (Virtual File System)

```
Application
      ↓
     VFS
      ↓
 ┌────┼─────┐
 ↓    ↓     ↓
ext4 xfs  ntfs
```

---

Without VFS:

```
ls → ext4 code
ls → xfs code
ls → ntfs code
```

would require separate implementations.

Instead:

```
ls
 ↓
VFS
 ↓
Filesystem-specific code
```

provides one common interface.

---

# FUSE (Filesystem in User Space)

Traditionally:

```
Filesystems run inside kernel space.
```

FUSE allows:

```
Filesystems in user space.
```

Examples:

- SSHFS
- Cloud storage mounts
- Encrypted filesystems

Example:

```bash
sshfs server:/data ~/remote
```

Now:

```
ls ~/remote
```

works like a normal filesystem even though files are coming over SSH.

---

# Common Filesystem Types

---

## ext4

### Fourth Extended Filesystem

Linux default for many years.

Type:

```
ext4
```

Features:

- Stable
- Fast
- Reliable
- Supports large files
- Supports many subdirectories

Example:

```
mkfs.ext4 /dev/sdb1
```

---

### Evolution

```
ext2
  ↓
ext3
  ↓
ext4
```

---

### ext2

Original Linux filesystem.

No journaling.

---

### ext3

Added:

## Journaling

A journal records pending filesystem operations before applying them.

Example:

```
Write file
 ↓
Journal records operation
 ↓
Filesystem updated
```

After power loss:

```
Journal helps recovery.
```

---

### ext4

Improves:

- Larger files
- Larger volumes
- Better performance
- More subdirectories

---

# Compatibility

Allowed:

```
ext2 → ext3
ext2 → ext4
ext3 → ext4
```

Not allowed:

```
ext4 → ext2
ext4 → ext3
```

because ext4 contains newer features older filesystems don't understand.

---

# Btrfs

## B-tree Filesystem

Type:

```
btrfs
```

Designed to replace ext4 eventually.

Advantages:

- Snapshots
- Checksums
- Better scalability
- Advanced volume management

The book notes that Btrfs was becoming the default on some distributions.

---

# FAT Filesystems

Microsoft family.

---

## msdos

Very old FAT format.

Characteristics:

```
8.3 filenames
UPPERCASE
```

Example:

```
README.TXT
```

---

## vfat

Adds:

- Long filenames
- Better compatibility

Common on:

```
USB drives
SD cards
```

Maximum file size:

```
4 GB
```

---

## exfat

Designed for larger files.

Common on:

```
Modern USB drives
SDXC cards
```

Supports files larger than:

```
4 GB
```

---

# NTFS

Windows default filesystem.

Type:

```
ntfs
```

Features:

- Permissions
- Journaling
- Large files

---

# XFS

High-performance filesystem.

Type:

```
xfs
```

Used by many enterprise distributions.

Known for:

- Large storage systems
- High throughput

---

# HFS+

Apple filesystem.

Type:

```
hfsplus
```

Used on older Mac systems.

---

# ISO 9660

Type:

```
iso9660
```

CD-ROM filesystem standard.

Example:

```
mount -t iso9660 image.iso /mnt
```

---

# Creating a Filesystem

After partitioning:

```
Disk
 ↓
Partition
 ↓
Filesystem
```

You must create a filesystem before storing files.

---

## Command

```bash
mkfs -t ext4 /dev/sdf2
```

Meaning:

```
mkfs         = make filesystem
-t ext4      = filesystem type
/dev/sdf2    = partition
```

---

# What Happens Internally?

Before:

```
/dev/sdf2
```

contains random blocks.

After:

```
Superblock
Inode tables
Data blocks
Journal
```

are initialized.

The partition is now usable.

---

# WARNING

Running:

```bash
mkfs.ext4 /dev/sdf2
```

on a partition that contains data will effectively destroy that data.

Think:

```
Formatting a drive
```

---

# Superblock

One of the most important filesystem structures.

Contains metadata such as:

```
Filesystem size
Block size
Filesystem state
```

If the superblock is corrupted:

```
Filesystem may become unmountable.
```

For safety:

```
mkfs
```

creates backup superblocks.

---

# What Is mkfs Really?

The book explains:

```
mkfs -t ext4 /dev/sdf2
```

is a frontend.

Internally:

```
mkfs.ext4 /dev/sdf2
```

is executed.

---

Example:

```
ls -l /sbin/mkfs.*
```

Output:

```
mkfs.ext2 -> mke2fs
mkfs.ext3 -> mke2fs
mkfs.ext4 -> mke2fs
```

Meaning:

```
mkfs.ext4
```

is actually a symbolic link to:

```
mke2fs
```

---

# Mounting a Filesystem

Creating a filesystem is not enough.

Linux must attach it to the directory tree.

This is called:

## Mounting

---

Example:

Filesystem exists:

```
/dev/sdf2
```

but nobody can access it.

Mount it:

```
mount /dev/sdf2 /home/extra
```

Now:

```
/home/extra
```

shows the contents of the filesystem.

---

# Mount Point

A mount point is simply:

```
An empty directory
```

Example:

```
mkdir /home/extra
```

Then:

```
mount /dev/sdf2 /home/extra
```

---

Result:

```
/
├── home
│   └── extra
```

The filesystem becomes visible there.

---

# Mount Command Syntax

```
mount -t type device mountpoint
```

Example:

```
mount -t ext4 /dev/sdf2 /home/extra
```

Components:

```
ext4        = filesystem type
/dev/sdf2   = device
/home/extra = mount point
```

---

# Viewing Mounted Filesystems

Command:

```
mount
```

Example output:

```
/dev/sda1 on / type ext4
proc on /proc type proc
sysfs on /sys type sysfs
```

Format:

```
device on mountpoint type filesystem
```

---

Example:

```
/dev/sda1 on / type ext4
```

means:

```
Device      = /dev/sda1
Mount point = /
Filesystem  = ext4
```

---

# Special Filesystems

Example:

```
proc on /proc type proc
```

Notice:

```
proc
```

isn't a real disk device.

It's a virtual filesystem.

Same for:

```
sysfs
tmpfs
devpts
```

---

# Unmounting

Detach a filesystem:

```
umount /home/extra
```

or:

```
umount /dev/sdf2
```

After unmounting:

```
Filesystem disappears from directory tree.
```

The data remains on disk.

---

# /mnt

Most Linux systems provide:

```
/mnt
```

for temporary mounting.

Example:

```
mount /dev/sdb1 /mnt
```

Useful for:

- Testing
- Recovery
- Examining USB drives

Not usually used for permanent mounts.