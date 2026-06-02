### Partition


### Create Partitions
- List devices using `lsblk`.
- fdisk is interactive, changes will not be commited until we enter `w` command.
- `fdisk /dev/sdd` to start paritioning.
- `Command (m for help): p`: prints parition.
- `d` delete existing parition, usually there are so we need to get rid of them first.
- `n` to create new parition.
    - Partition type 
        p   primary (0 primary, 0 extended, 4 free) 
        e   extended (container for logical partitions) 
    Select (default p): `p` # p for primary 
    Partition number (1-4, default 1): `1` 
    First sector (2048-8368127, default 2048): `2048`
    Last sector, +sectors or +size{K,M,G,T,P} (2048-8368127, default 8368127): `+200M` # for 200MB size, leave empty if we wanna use it all
    Created a new partition 1 of type 'Linux' and of size 200 MiB.
- `w` to write parition.


# Background

## What is a Partition?

- A **partition** is a subdivision of a physical disk.
- Linux represents partitions by appending a number to the disk device name:
    - `/dev/sda1` → Partition 1 on disk `/dev/sda`
    - `/dev/sdb3` → Partition 3 on disk `/dev/sdb`
- The kernel treats each partition as its own **block device**, just like an entire disk.

### Example

```
Disk: /dev/sda
├── /dev/sda1
├── /dev/sda2
└── /dev/sda3
```

---

## Partition Table (Disk Label)

- Partitions are defined in a special area of the disk called the:
    - **Partition Table**
    - **Disk Label** (another name for the same thing)
- The partition table stores:
    - Partition boundaries
    - Partition sizes
    - Partition types

[alt](./images/01.png)
---

## Why Create Multiple Partitions?

Historically, multiple partitions were common for several reasons:

### 1. Boot Limitations

- Older PCs could only boot from certain regions of a disk.
- Administrators created partitions to ensure boot files were in accessible locations.

### 2. Space Management

- Administrators could reserve disk space for the operating system.
- Prevented users from filling the entire disk and causing critical services to fail.

### Example

```
Disk
├── OS Partition     (20 GB)
├── User Data        (200 GB)
└── Swap             (8 GB)
```

Even today:

- Many Windows systems use multiple partitions.
- Most systems still have a dedicated **swap partition**.

---

## Accessing Entire Disks vs Partitions

Linux allows access to:

1. The entire disk device
2. Individual partitions

Example:

```
Entire disk:     /dev/sda
Partition:       /dev/sda1
```

The kernel allows both to be accessed simultaneously.

### Typical Usage

Normally, you access:

```
/dev/sda1
```

rather than:

```
/dev/sda
```

### Exception

Accessing the entire disk is useful when:

- Cloning disks
- Creating backups
- Copying disk images

Example:

```
dd if=/dev/sda of=disk_backup.img
```

---

## Logical Volume Manager (LVM)

Linux provides the **Logical Volume Manager (LVM)** to make storage management more flexible.

### Benefits

- Resize storage dynamically
- Combine multiple disks into larger logical volumes
- Easier storage administration

### Layer Position

LVM sits between:

- Physical storage devices
- Filesystems

```
Disk
  ↓
Partition
  ↓
LVM
  ↓
Filesystem
```


---

## Filesystems

Above partitions sits the **filesystem**.

### What is a Filesystem?

A filesystem is a database-like structure that stores:

- Files
- Directories
- Metadata

Examples:

- ext4
- XFS
- Btrfs

Users interact primarily with the filesystem rather than directly with disk blocks.

---

## How Linux Finds a File

To access file data:

### Step 1

Use the partition table to locate the correct partition.

### Step 2

Read the filesystem stored on that partition.

### Step 3

Search the filesystem metadata for the requested file.

### Step 4

Locate and read the file's data blocks.

### Visualization

```
Physical Disk
     ↓
Partition Table
     ↓
Partition
     ↓
Filesystem
     ↓
File Metadata
     ↓
File Data Blocks
```
[alt](./images/02.png)

---

## Linux Disk Access Layers

Linux accesses storage through multiple layers.

### Simplified Stack

```
User Program
      ↓
Filesystem
      ↓
Block Device Layer
      ↓
Partition
      ↓
Disk Device
      ↓
SCSI/Storage Subsystem
      ↓
Hardware
```

### Important Observation

Linux provides two ways to access storage:

#### Through the Filesystem

Normal user operations:

```
cat file.txt
cp file1 file2
vim notes.txt
```

Path:

```
Application
    ↓
Filesystem
    ↓
Disk
```

#### Direct Disk Access

Utilities can bypass the filesystem:

```
dd if=/dev/sda1 of=image.img
```

Path:

```
Application
    ↓
Block Device
    ↓
Disk
```

This is useful for:

- Disk imaging
- Recovery tools
- Filesystem creation
- Low-level disk utilities