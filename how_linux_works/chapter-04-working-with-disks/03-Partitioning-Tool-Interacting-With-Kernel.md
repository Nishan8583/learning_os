# How Partitioning Tools Interact with the Kernel

An important design principle:

> Partition tables are ordinary data stored on a block device.

Because of this:

```
Kernel support is not requiredto edit a partition table.
```

User-space programs can:

- Read the disk
- Modify the partition table
- Write the updated table back

directly.

---

## Why the Kernel Still Matters

After modifying the partition table:

```
Disk
      ↓
Partition Table Changed
```

the kernel must reread it so that it can create or remove partition devices.

Example:

```
/dev/sdf1
/dev/sdf2
```

cannot exist until the kernel knows about them.

---

# How fdisk Updates the Kernel

After writing the partition table:

```
fdisk
      ↓
System Call
      ↓
Kernel rereads partition table
```

The kernel then discovers the new partitions.

### Example Kernel Message

```
sdf: sdf1 sdf2
```

Meaning:

```
Disk: /dev/sdf

Partitions found:
  /dev/sdf1
  /dev/sdf2
```

---

## Viewing Kernel Messages

Use:

```
journalctl -k
```

Example output:

```
sdf: sdf1 sdf2
```

This confirms the kernel detected the new partitions.

---

# How parted Updates the Kernel

`parted` behaves differently.

Instead of issuing a single:

```
Reread entire partition table
```

request,

it notifies the kernel about individual partition changes as they occur.

### Workflow

```
Modify Partition
      ↓
Notify Kernel
      ↓
Kernel Updates Partition
```

Because changes happen incrementally:

```
journalctl -k
```

typically does **not** show the simple:

```
sdf: sdf1 sdf2
```

message seen with `fdisk`.

---

# How to Observe Partition Changes

Several methods exist.

---

## 1. Monitor udev Events

Linux generates device events whenever partitions are added or removed.

### Command

```
udevadm monitor --kernel
```

### Example Output

```
KERNEL[1234]: remove /devices/.../sdf1
KERNEL[1235]: add    /devices/.../sdf1
```

Useful for watching partition changes in real time.

---

## 2. Check `/proc/partitions`

View the kernel's current partition information.

### Command

```
cat /proc/partitions
```

Example:

```
major minor  blocks  name

8        0  41943040 sda
8        1    524288 sda1
8        2  41418752 sda2
```

---

## 3. Check `/sys/block`

Linux exposes block-device information through sysfs.

Example:

```
ls /sys/block
```

Output:

```
sda
sdb
loop0
```

Inspect a device:

```
ls /sys/block/sda
```

---

## 4. Check `/dev`

Device files appear here.

Example:

```
ls /dev/sd*
```

Output:

```
/dev/sda
/dev/sda1
/dev/sda2
```

New partitions appear here after the kernel recognizes them.

---

# Forcing a Partition Table Reload

Sometimes the kernel does not immediately reread the partition table.

You can force it manually.

### Command

```
blockdev --rereadpt /dev/sdf
```

### Meaning

```
blockdev
    ↓
Issue partition-table reread request
    ↓
Kernel reloads partition table
```

This performs the same style of operation that `fdisk` normally requests automatically.

---

# Partition Modification Flow

## fdisk

```
User
  ↓
fdisk
  ↓
Modify in Memory
  ↓
Write Changes
  ↓
Kernel Rereads Entire Table
  ↓
Partition Devices Created
```

---

## parted

```
User
  ↓
parted
  ↓
Modify Partition
  ↓
Immediate Disk Update
  ↓
Kernel Notified
  ↓
Partition Updated
```

---

# Key Takeaways

- Modifying partition tables can make filesystems inaccessible.
- Always back up important data before repartitioning.
- Ensure target partitions are not mounted or in use.
- `fdisk` stages changes and writes them only when you confirm with `w`.
- `parted` applies changes immediately as commands are entered.
- Partition tables are edited entirely from user space.
- The kernel must reread the partition table before new partition devices become available.
- `fdisk` typically triggers a full partition-table reread.
- `parted` updates partitions individually.
- Useful commands:

```bash
journalctl -k
udevadm monitor --kernel
cat /proc/partitions
ls /sys/block
ls /dev/sd*
blockdev --rereadpt /dev/sdf
```

- To manually force the kernel to reload a partition table:

```
blockdev --rereadpt /dev/sdf
```