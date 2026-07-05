### UUID and by-id

Excellent question. There are actually **three different kinds of stable names**, and it's easy to confuse them.

|Name|Example|What identifies|
|---|---|---|
|Kernel name|`/dev/sda1`|Current kernel-assigned device|
|`by-id`|`/dev/disk/by-id/scsi-SATA_WDC_WD3200AAJS-_WDWMAV2FU80671-part1`|Physical partition on a specific disk|
|UUID|`/dev/disk/by-uuid/8d9c8e1d-...`|Filesystem stored on that partition|

The important thing is that **UUID and `by-id` solve different problems.**

---

## Option 1: Use `/dev/sda1`

This is **not stable**.

Today:

```
/dev/sda1 = Windows partition
```

Tomorrow:

```
/dev/sdb1 = Windows partition
```

Your configuration breaks.

---

## Option 2: Use `by-id`

Suppose you have:

```
/dev/disk/by-id/scsi-SATA_WDC_WD3200AAJS-_WDWMAV2FU80671-part1    -> /dev/sda1
```

Next boot:

```
/dev/disk/by-id/scsi-SATA_WDC_WD3200AAJS-_WDWMAV2FU80671-part1    -> /dev/sdb1
```

Nothing breaks.

The symlink simply points to the new kernel name.

So **yes**, `by-id` completely solves the "sda becomes sdb" problem.

---

## Then why do people use UUID?

Because usually we don't actually care about **which disk** the filesystem is on.

We care about **which filesystem** we want.

Imagine this:

You clone your disk.

Now you have:

```
Disk A
  Serial = AAA
  ext4 filesystem

Disk B
  Serial = BBB
  ext4 filesystem (exact clone)
```

Each disk has a different `by-id`.

But the filesystem itself has its own UUID stored in its superblock.

For example:

```
Filesystem UUID

4f9b-1234-abcd-5678
```

That UUID belongs to the filesystem, not the disk.

---

## What happens during boot?

Suppose `/etc/fstab` contains:

```
UUID=4f9b-1234-abcd-5678 /home ext4 defaults 0 2
```

During boot:

1. The kernel discovers disks.
2. `udevd` creates:
    - `/dev/sda1`
    - `/dev/disk/by-id/...`
    - `/dev/disk/by-uuid/...`
3. The mount program reads `/etc/fstab`.
4. It looks for:

```
/dev/disk/by-uuid/4f9b-1234-abcd-5678
```

which might be

```
/dev/disk/by-uuid/4f9b...
        │
        ▼
/dev/sdb1
```

Then it mounts `/dev/sdb1`.

---

## Wait... `/dev/disk/by-uuid` is also a symlink?

**Yes!**

If you run:

```
ls -l /dev/disk/by-uuid
```

you'll see something like:

```
4f9b1234-abcd-5678 -> ../../sda1
```

So **UUIDs are implemented using symlinks too.**

---

## Why is UUID usually preferred?

For mounting filesystems, UUID is often the best choice because it identifies the **filesystem**, not the physical disk.

For example:

- If you replace a SATA cable, the UUID doesn't change.
- If the kernel renames `sda` to `sdb`, the UUID doesn't change.
- Even if you move the disk to another computer, the filesystem UUID stays the same.

---

## So when should you use each?

- **`/dev/sda1`**: Temporary manual commands (`fdisk`, `mkfs`, quick inspection).
- **`by-id`**: When you care about a specific physical disk (common in storage servers, RAID, SANs).
- **UUID**: When you care about mounting a particular filesystem (this is why `/etc/fstab` commonly uses UUIDs).

---

### The big picture

```
Physical disk
      │
      ▼
Kernel assigns: sda1        (may change)
      │
      ▼
udev creates:
    by-id   ─────► ../../sda1   (identifies the hardware)
    by-uuid ────► ../../sda1   (identifies the filesystem)
```

So your original understanding was almost correct: we don't rely on `/dev/sda1` for persistent configuration because it can change. Instead, we usually refer to a stable symlink—most commonly `by-uuid` for mounting filesystems, or `by-id` when we specifically want to identify a particular physical disk. Both ultimately resolve to whatever the kernel currently calls the device (`/dev/sda1`, `/dev/sdb1`, etc.).


----


### Discussion

## First boot

The kernel detects:

```
Disk A → /dev/sda
Disk B → /dev/sdb
```

Suppose:

- Disk A contains a filesystem with UUID `1111`
- Disk B contains a filesystem with UUID `2222`

`udevd` creates:

```
/dev/disk/by-uuid/1111 -> ../../sda1
/dev/disk/by-uuid/2222 -> ../../sdb1
```

Everything is consistent.

---

## Second boot

This time the kernel discovers the disks in the opposite order:

```
Disk B → /dev/sda
Disk A → /dev/sdb
```

Now the kernel exports information through `/sys`.

`udevd` reads:

```
This partition has UUID 2222
```

and creates:

```
/dev/disk/by-uuid/2222 -> ../../sda1
```

Then it reads:

```
This partition has UUID 1111
```

and creates:

```
/ dev/disk/by-uuid/1111 -> ../../sdb1
```

Notice what changed:

### First boot

```
UUID1111 -> sda1
UUID2222 -> sdb1
```

### Second boot

```
UUID1111 -> sdb1
UUID2222 -> sda1
```

The **symlink target changes**, but the **UUID name stays the same**.
Since the kernel reads the Filesystem data and reads UUID, the same UUID will refer to the same filesytem in a partition on a disk.

---

## The same is true for `by-id`

Suppose Disk A has serial number `AAA`.

### First boot

```
by-id/AAA -> sda
```

### Second boot

```
by-id/AAA -> sdb
```

Again, the target changes.

---

## How does `udevd` know?

This is the clever part.

The kernel exports device information in `/sys`.

For example:

```
/sys/block/sda
```

contains information about the disk.

For partitions that contain filesystems, the filesystem UUID is also available (typically obtained via helpers such as `blkid`, which `udevd` can invoke through its rules).

So during every boot, `udevd` effectively does something like:

```
For every detected partition:

    Read its filesystem UUID

    If UUID = 1111
        create by-uuid/1111 -> current device
```

Likewise, for disks it reads attributes like serial number from `/sys` to create the `by-id` links.

There is **no database** saying:

```
UUID1111 always equals sda1
```

Instead, every boot it asks:

> "Which partition currently has UUID 1111?"

Then it creates the appropriate symlink.

---

## Visual example

### Boot 1

```
Disk A (UUID1111) ---> /dev/sda1
Disk B (UUID2222) ---> /dev/sdb1

by-uuid/1111 -> sda1
by-uuid/2222 -> sdb1
```

### Boot 2

```
Disk B (UUID2222) ---> /dev/sda1
Disk A (UUID1111) ---> /dev/sdb1

by-uuid/2222 -> sda1
by-uuid/1111 -> sdb1
```

The kernel changed the names, but `udevd` recreated the symlinks to point to the correct current device.