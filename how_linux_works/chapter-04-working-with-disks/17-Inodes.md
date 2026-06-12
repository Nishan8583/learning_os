# 4.5 Looking Forward: Disks and User Space

## User Space vs Kernel Space in Storage

Storage management in Linux is split between:

### Kernel Responsibilities

- Raw block I/O operations.
- Filesystem implementation and access.
- Virtual memory swap management.
- Reading and writing blocks to storage devices.

### User Space Responsibilities

- Disk partitioning.
- Filesystem creation.
- Swap space creation.
- Filesystem administration tools.

### Key Takeaway

Although user-space tools can access block devices directly through device files, normal applications interact with storage through filesystems provided by the kernel.

---

# 4.6 Inside a Traditional Filesystem

A traditional Unix filesystem consists of two major components:

|Component|Purpose|
|---|---|
|Data Blocks|Store actual file contents|
|Inode Database|Stores metadata and locations of files|

The filesystem database revolves around the **inode**.

---

# What is an Inode?

An inode is a data structure that describes a file.

An inode contains:

- File type
- Permissions
- Ownership information
- Link count
- Locations of file data blocks

Most importantly:

> An inode tells the filesystem where the file's data is stored.

Each inode is identified by an inode number in the inode table.

---

# Directories are Files Too

Directories are implemented as special inodes.

A directory inode contains:

- Filenames
- Mappings from filenames to inode numbers

Example:

```
dir_1
 ├── file_1 -> inode 13
 ├── file_2 -> inode 14
 └── file_3 -> inode 15
```

The directory stores names, while the inode stores file metadata.

---

# Example Filesystem Structure

Commands used to create the example:

```
mkdir dir_1
mkdir dir_2

echo a > dir_1/file_1
echo b > dir_1/file_2
echo c > dir_1/file_3

echo d > dir_2/file_4

ln dir_1/file_3 dir_2/file_5
```

### Command Breakdown

|Command|Purpose|
|---|---|
|`mkdir dir_1`|Create directory|
|`mkdir dir_2`|Create directory|
|`echo a > dir_1/file_1`|Create file containing `a`|
|`echo b > dir_1/file_2`|Create file containing `b`|
|`echo c > dir_1/file_3`|Create file containing `c`|
|`echo d > dir_2/file_4`|Create file containing `d`|
|`ln dir_1/file_3 dir_2/file_5`|Create hard link|

---

# Hard Links

The command:

```
ln dir_1/file_3 dir_2/file_5
```

creates a **hard link**.

This means:

```
dir_1/file_3
dir_2/file_5
```

are two names pointing to the same inode.

```
file_3
    \
     -> inode 15
    /
file_5
```

No duplicate file data is created.

---

# Root Inode

In ext2/ext3/ext4:

```
inode 2
```

is always the filesystem root inode.

Important:

This is **NOT** the Linux root filesystem (`/`).

It is simply the top directory inode within a filesystem.

---

# How Path Resolution Works

Example:

```
dir_1/file_2
```

The kernel performs:

### Step 1

Split path:

```
dir_1
file_2
```

### Step 2

Start at:

```
inode 2 (root)
```

### Step 3

Search root directory data for:

```
dir_1
```

Result:

```
inode 12
```

### Step 4

Verify inode 12 is a directory.

### Step 5

Load inode 12 directory contents.

### Step 6

Search for:

```
file_2
```

Result:

```
inode 14
```

### Step 7

Load inode 14.

### Step 8

Follow inode 14's data pointers to access file contents.

This is how every pathname lookup works internally.

---

# Special Directory Entries

Every directory contains:

|Entry|Meaning|
|---|---|
|`.`|Current directory|
|`..`|Parent directory|

Example:

```
.
..
file_1
file_2
```

These entries allow navigation through the filesystem hierarchy.

---

# Viewing Inode Numbers

Use:

```
ls -i
```

Example:

```
12 dir_1
7633 dir_2
```

### Command Purpose

|Command|Purpose|
|---|---|
|`ls -i`|Show inode numbers|

For more detailed inode information:

```
stat
```

|Command|Purpose|
|---|---|
|`stat`|Show detailed inode metadata|

---

# Link Count

Every inode contains a **link count**.

Definition:

> Number of directory entries pointing to that inode.

Example:

```
inode 15
 ├── dir_1/file_3
 └── dir_2/file_5
```

Link count:

```
2
```

because two directory entries point to the same inode.

---

# Removing Files (Unlinking)

Example:

```
rm dir_1/file_2
```

Kernel actions:

1. Locate directory entry.
2. Find corresponding inode.
3. Remove directory entry.
4. Decrement link count.

If link count becomes:

```
0
```

the kernel:

- Deletes the inode.
- Frees associated blocks.

---

# Hard Link Deletion

Example:

```
rm dir_1/file_3
```

Before:

```
inode 15
link count = 2
```

After:

```
inode 15
link count = 1
```

Because:

```
dir_2/file_5
```

still references the inode.

Therefore:

- Inode remains.
- Data remains.

---

# Directory Link Counts

Directories also have link counts.

Example:

```
inode 12 (dir_1)
```

Link count:

```
2
```

Because:

1. Parent directory references it.
2. It contains its own `.` entry.

---

# Adding a Subdirectory

Example:

```
dir_1/dir_3
```

Now inode 12's link count increases.

Reason:

```
dir_3/..
```

points back to inode 12.

Each child directory contributes one additional link count to its parent.

---

# Root Directory Special Case

Root inode:

```
inode 2
```

appears to have one extra link count.

Reason:

The filesystem superblock also references the root inode.

---

# Block Allocation

The filesystem must track which blocks are free.

A common mechanism is the:

## Block Bitmap

Each bit represents one block.

|Bit Value|Meaning|
|---|---|
|0|Free block|
|1|Allocated block|

Example:

```
Bitmap:

0 1 1 0 0 1

Block:

1 2 3 4 5 6
```

Blocks:

```
1,4,5
```

are free.

Allocation and deallocation simply flip bits.

---

# Filesystem Corruption

Problems occur when:

- Link counts are wrong.
- Block bitmap is incorrect.
- Inode table and allocation metadata disagree.

Common cause:

```
Improper shutdown
```

or power loss.

---

# fsck Recovery

The filesystem checker:

```
fsck
```

performs:

1. Walk inode table.
2. Walk directory tree.
3. Rebuild link counts.
4. Rebuild allocation map.
5. Compare with on-disk metadata.

If inconsistencies are found:

- Link counts are fixed.
- Orphaned files are recovered.

---

# lost+found Directory

Files discovered by `fsck` but no longer reachable from any directory are placed in:

```
lost+found
```

These are called:

```
Orphaned files
```

---

# Filesystem Details in User Space

User-space programs can access filesystem metadata through:

```c
stat()
```

The `stat()` system call returns:

- Inode numbers
- Link counts
- Permissions
- Timestamps
- File size

---

# VFS (Virtual Filesystem)

Linux provides a Virtual Filesystem layer (VFS).

Benefits:

- Uniform filesystem API.
- Same system calls work across different filesystems.
- Applications don't need filesystem-specific code.

VFS ensures metadata such as inode numbers and link counts are always returned, even if the underlying filesystem doesn't truly use Unix-style inodes.

---

# Filesystems May Differ

Not all filesystems support Unix concepts.

Example:

```
VFAT
```

does not support hard links.

Therefore:

```
ln file1 file2
```

fails on VFAT.

---

# User-Space Filesystems

Some filesystems operate largely in user space.

In these systems:

- Kernel forwards filesystem requests.
- User-space process implements filesystem logic.

The kernel acts mainly as a conduit for filesystem system calls.