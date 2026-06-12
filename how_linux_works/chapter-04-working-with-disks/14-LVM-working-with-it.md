# Constructing and Managing an LVM (Logical Volume Manager) System

These notes cover creating physical volumes (PVs), volume groups (VGs), logical volumes (LVs), formatting them with filesystems, mounting them, resizing them, and removing them. All commands and outputs from the source have been included.

---

# Why Use LVM?

LVM provides an abstraction layer between physical disks and filesystems.

Benefits:

- Combine multiple disks into a single storage pool.
- Create logical volumes independent of physical disk boundaries.
- Resize volumes more easily than traditional partitions.
- Allocate storage dynamically.
- Manage storage without requiring identical disk sizes.

Example from the text:

- Disk 1: 5 GB (`/dev/sdb`)
- Disk 2: 15 GB (`/dev/sdc`)
- Combined into volume group:
    - `myvg`
- Divided into two logical volumes:
    - `mylv1` (10 GB)
    - `mylv2` (10 GB)

This would be difficult using traditional partitioning alone.

---

# LVM Components

## Physical Volume (PV)

A block device used by LVM.

Examples:

- `/dev/sdb1`
- `/dev/sdc1`

A PV can be:

- A partition
- An entire disk
- Any block device

---

## Volume Group (VG)

A storage pool created from one or more PVs.

Example:

```
myvg
```

Contains storage from:

```
/dev/sdb1
/dev/sdc1
```

---

## Logical Volume (LV)

Virtual partitions created inside a volume group.

Examples:

```
mylv1
mylv2
```

Logical volumes are what filesystems are created on.

---

# Step 1: Create LVM Partitions

Before creating PVs, create partitions and mark them as LVM partitions (type ID `8e`).
- In fdisk to change partition type `t` and set to `8e`

## Verify `/dev/sdb`

```
# parted /dev/sdb print
```

Output:

```
Model: ATA VBOX HARDDISK (scsi)
Disk /dev/sdb: 5616MB
Sector size (logical/physical): 512B/512B
Partition Table: msdos
Disk Flags:

Number Start End Size Type File system Flags
1 1049kB 5616MB 5615MB primary lvm
```

---

## Verify `/dev/sdc`

```
# parted /dev/sdc print
```

Output:

```
Model: ATA VBOX HARDDISK (scsi)
Disk /dev/sdc: 16.0GB
Sector size (logical/physical): 512B/512B
Partition Table: msdos
Disk Flags:

Number Start End Size Type File system Flags
1 1049kB 16.0GB 16.0GB primary lvm
```

### Important Note

You do **not** have to partition a disk before making it a PV.

Example:

```
/dev/sdb
```

could be used directly.

However, partitioning:

- Makes booting easier.
- Clearly identifies devices intended for LVM.

---

# Step 2: Create a Volume Group

Create a volume group called `myvg` using `/dev/sdb1`.

Command:

```
# vgcreate myvg /dev/sdb1
```

Output:

```
Physical volume "/dev/sdb1" successfully created.
Volume group "myvg" successfully created
```

### Note

`vgcreate` automatically performs the work of `pvcreate` if the partition is not already an LVM physical volume.

---

# Step 3: Verify Volume Group

Command:

```
# vgs
```

Output:

```
VG #PV #LV #SN Attr VSize VFree
myvg 1 0 0 wz--n- <5.23g <5.23g
```

Meaning:

|Column|Meaning|
|---|---|
|VG|Volume Group|
|#PV|Number of Physical Volumes|
|#LV|Number of Logical Volumes|
|VSize|Total Size|
|VFree|Available Space|

### If the VG Doesn't Appear

Run:

```
pvscan
```

Some systems require manual rescanning after LVM changes.

---

# Step 4: Extend the Volume Group

Add the second physical volume.

Command:

```
# vgextend myvg /dev/sdc1
```

Output:

```
Physical volume "/dev/sdc1" successfully created.
Volume group "myvg" successfully extended
```

---

## Verify Again

```
# vgs
```

Output:

```
VG #PV #LV #SN Attr VSize VFree
my-vg 2 0 0 wz--n- <20.16g <20.16g
```

Now:

- 2 PVs
- ~20 GB combined storage

---

# Step 5: Create Logical Volumes

Create two 10 GB logical volumes.

## Create `mylv1`

```
# lvcreate --size 10g --type linear -n mylv1 myvg
```

Output:

```
Logical volume "mylv1" created.
```

---

## Create `mylv2`

```
# lvcreate --size 10g --type linear -n mylv2 myvg
```

Output:

```
Logical volume "mylv2" created.
```

### Notes

`--type linear`

- Simplest LV type.
- Maps extents sequentially.
- Default type, so it can be omitted.

Example:

```
lvcreate --size 10g -n mylv1 myvg
```

would also work.

---

# Verify Logical Volumes

Inspect the VG.

Command:

```
# vgdisplay myvg
```

Output:

```
--- Volume group ---
VG Name myvg
System ID
Format lvm2
Metadata Areas 2
Metadata Sequence No 4
VG Access read/write
VG Status resizable
MAX LV 0
Cur LV 2
Open LV 0
Max PV 0
Cur PV 2
Act PV 2
VG Size 20.16 GiB
PE Size 4.00 MiB
Total PE 5162
Alloc PE / Size 5120 / 20.00 GiB
Free PE / Size 42 / 168.00 MiB
VG UUID 1pHrOe-e5zy-TUtK-5gnN-SpDY-shM8-Cbokf3
```

### Key Concepts

#### PE (Physical Extent)

LVM divides PVs into chunks called extents.

In this example:

```
PE Size = 4 MiB
```

There are:

```
42 free extents
```

because:

```
20.16 GiB
```

cannot be perfectly divided into two exact 10 GB logical volumes.

---

# Step 6: Create a Filesystem

Create an ext4 filesystem on `mylv1`.

Command:

```
# mkfs -t ext4 /dev/mapper/myvg-mylv1
```

Output:

```
mke2fs 1.44.1 (24-Mar-2018)
Creating filesystem with 2621440 4k blocks and 655360 inodes
Filesystem UUID: 83cc4119-625c-49d1-88c4-e2359a15a887

Superblock backups stored on blocks:
32768, 98304, 163840, 229376, 294912, 819200, 884736, 1605632

Allocating group tables: done
Writing inode tables: done
Creating journal (16384 blocks): done
Writing superblocks and filesystem accounting information: done
```

---

# Step 7: Mount the Logical Volume

Mount it:

```
# mount /dev/mapper/myvg-mylv1 /mnt
```

Check capacity:

```
# df /mnt
```

Output:

```
Filesystem 1K-blocks Used Available Use% Mounted on
/dev/mapper/myvg-mylv1 10255636 36888 9678076 1% /mnt
```

This LV now behaves exactly like a normal partition.

---

# Removing a Logical Volume

Suppose:

```
mylv2
```

is no longer needed.

Remove it:

```
# lvremove myvg/mylv2
```

Output:

```
Do you really want to remove and DISCARD active logical volume
myvg/mylv2? [y/n]: y

Logical volume "mylv2" successfully removed
```

---

# Critical Warning About lvremove

Correct syntax:

```
lvremove myvg/mylv2
```

Notice the slash:

```
volume-group/logical-volume
```

A common mistake:

```
lvremove myvg mylv2
```

Using a space can cause `lvremove` to interpret arguments differently and potentially remove more logical volumes than intended.

Always verify the command before pressing Enter.

---

# Resizing a Logical Volume

After deleting `mylv2`, all free extents can be given to `mylv1`.

## Step 1: Resize the LV

Command:

```
# lvresize -l +2602 myvg/mylv1
```

Output:

```
Size of logical volume myvg/mylv1 changed from 10.00 GiB (2560 extents)
to 20.16 GiB (5162 extents).

Logical volume myvg/mylv1 successfully resized.
```

Meaning:

```
+2602 extents
```

were added to the logical volume.

---

# Step 2: Resize the Filesystem

The filesystem must also grow.

Command:

```
# fsadm -v resize /dev/mapper/myvg-mylv1
```

Output:

```
fsadm: "ext4" filesystem found on "/dev/mapper/myvg-mylv1".

fsadm: Device "/dev/mapper/myvg-mylv1" size is 21650997248 bytes

fsadm: Parsing tune2fs -l "/dev/mapper/myvg-mylv1"

fsadm: Resizing filesystem on device "/dev/mapper/myvg-mylv1" to
21650997248 bytes (2621440 -> 5285888 blocks of 4096 bytes)

fsadm: Executing resize2fs /dev/mapper/myvg-mylv1 5285888

resize2fs 1.44.1 (24-Mar-2018)

Filesystem at /dev/mapper/myvg-mylv1 is mounted on /mnt;
on-line resizing required

old_desc_blocks = 2, new_desc_blocks = 3

The filesystem on /dev/mapper/myvg-mylv1 is now
5285888 (4k) blocks long.
```

### Important Observation

`fsadm` is a wrapper script.

Internally it calls filesystem-specific tools such as:

```
resize2fs
```

for ext4 filesystems.

---

# Easier Method: Resize LV and Filesystem Together

Instead of running two commands:

```
lvresize
fsadm
```

Use:

```
# lvresize -r -l +100%FREE myvg/mylv1
```

Meaning:

- `-r` → resize filesystem too
- `+100%FREE` → consume all free extents in the VG

This is the preferred approach.

---

# Growing vs Shrinking Filesystems

## Growing

Ext2/3/4 filesystems can be expanded while mounted.

Example:

```
lvresize -r -l +100%FREE myvg/mylv1
```

can be performed online.

---

## Shrinking

Shrinking is more complicated.

Requirements:

1. Unmount filesystem.
2. Shrink filesystem first.
3. Shrink logical volume second.

Reason:

The filesystem must always fit inside the logical volume.

Using:

```
lvresize -r
```

is safer because LVM coordinates both operations automatically.