# udev
- Adding more comelexiity in kernel makes it unstable, thats why device file management was given to user process `udev`.
- kernel (detects new device) -> (notification) -> `udevd` (user-space process).
- Problem: Device files are requried early in boot process, so udevd must also start early, udevd can not depend on device it is supposed to create.
- Solution is `devtmpfs`.

### devtmpfs
- Its a in memory File System.
- Similar to `devfs`, but simplified.
```
Kernel creates device file

        | (notifieds udevd new device is available)

udevd (does not create device file)
    - performs device initalization.
    - sets permissions.
    - Notifies other system processes.
    - Creates symbolic links in "/dev" to further identify devices (Examples in /dev/disk/by-id)
```
- Sample output:
```
$ ls -l /dev/disk/by-id 
lrwxrwxrwx 1 root root  9 Jul 26 10:23 scsi-SATA_WDC_WD3200AAJS-_WDWMAV2FU80671 -> ../../sda 
lrwxrwxrwx 1 root root 10 Jul 26 10:23 scsi-SATA_WDC_WD3200AAJS-_WDWMAV2FU80671-part1 -> ../../sda1 
lrwxrwxrwx 1 root root 10 Jul 26 10:23 scsi-SATA_WDC_WD3200AAJS-_WDWMAV2FU80671-part2 -> ../../sda2 
lrwxrwxrwx 1 root root 10 Jul 26 10:23 scsi-SATA_WDC_WD3200AAJS-_WDWMAV2FU80671-part5 -> ../../sda5
```
- `udevd` process names the link by `[interface-type]-[manufacturer]-[model_information]-[serial_number]-[partition(if applicable)]`


---


# Flow
When the kernel detects a disk, it creates device nodes like:

```
/dev/sda
/dev/sda1
/dev/sda2
```

The problem is that these names are **not stable**. If you unplug a drive or add another one, today's `/dev/sda` could become `/dev/sdb` after reboot.

`udevd` solves this by creating **persistent names** based on hardware properties.

For example:

```
/dev/disk/by-id/scsi-SATA_WDC_WD3200AAJS-_WDWMAV2FU80671        -> ../../sda
```

This symlink points to whatever kernel device represents that physical disk.

Similarly,

```
scsi-SATA_WDC_WD3200AAJS-_WDWMAV2FU80671-part1        -> ../../sda1
```

is a persistent name for partition 1.

---
### Are these used by `fdisk` or `gparted`?

Yes, but with some nuance.

Programs like `fdisk`, `parted`, and graphical tools such as `gparted` ultimately operate on the **block device**.

For example, these all refer to the same disk:

```
fdisk /dev/sda

fdisk /dev/disk/by-id/scsi-SATA_WDC_WD3200AAJS-_WDWMAV2FU80671

fdisk /dev/disk/by-path/pci-0000:00:17.0-ata-1
```

Since the `by-id` entry is just a symlink to `/dev/sda`, the kernel receives exactly the same block device.

Many GUI tools actually **display** the stable names internally because they're much less likely to change than `/dev/sda`.

---
### What happens when a new partition is created?

Suppose the disk initially has

```
sda
sda1
sda2
```

You create another partition.

The sequence is roughly:

1. `fdisk` or `parted` writes the new partition table.
2. The kernel rereads the partition table.
3. The kernel creates a new block device:

```
/dev/sda3
```

4. The kernel sends a `**uevent**` announcing the new partition.
5. `udevd` receives that event.
6. `udevd` creates new symlinks such as

```
/dev/disk/by-id/scsi-SATA_WDC_WD3200AAJS-_WDWMAV2FU80671-part3
        -> ../../sda3
```

So yes, `udevd` automatically updates these directories whenever disks or partitions are added or removed.

### Overall Flow
```
Physical disk
      │
      ▼
Kernel discovers device
      │
      ▼
Creates internal block device (sda)
      │
      ├── Exports information in /sys/block/sda
      ├── Creates /dev/sda via devtmpfs
      └── Sends uevent
               │
               ▼
          udevd receives event
               │
               ├── Sets permissions/ownership
               ├── Creates /dev/disk/by-id/...
               ├── Creates /dev/disk/by-path/...
               └── Runs any matching udev rules
```

---

## Does the kernel store devices as `/dev/sda` or as `scsi-SATA_...`?

Neither, actually.

Inside the kernel, devices are represented by kernel data structures, not by filenames.

For example, when the kernel discovers a SATA disk, it creates an internal block device object containing information like:

- Major/minor numbers
- Device size
- Partition table
- Bus type (SATA, NVMe, USB, etc.)
- Vendor
- Model
- Serial number (if available)

The kernel does **not** internally think of the disk as "`/dev/sda`".

Instead, it assigns a kernel name like:

```
sda
```

This is simply the kernel's name for that block device.

Later, userspace creates the file

```
/dev/sda
```

that refers to the kernel device.

So:

```
Kernel object
      │
      ├── kernel name: sda
      └── serial: WDWMAV2FU80671
```

The long name

```
scsi-SATA_WDC_WD3200AAJS-_WDWMAV2FU80671
```

is **not** the kernel's name.

That is generated later by `udevd`.

---

## During early boot

The sequence looks like this:

```
Kernel starts
      │
      ▼
Detect SATA controller
      │
      ▼
Detect disk
      │
      ▼
Create block device named "sda"
      │
      ▼
Send uevent
```

The kernel tells userspace something like:

> "A block device named `sda` appeared."

It also provides attributes through the sysfs filesystem under `/sys`, such as vendor, model, serial number, and other metadata.

---

## Where does `udevd` get the long name?

It reads information exported by the kernel through `/sys`.

For example:

```
/sys/block/sda/
```

contains attributes like

```
vendor
model
serial
wwn
```

`udevd` reads those attributes and applies rules like:

```
If device has serial number
    create a symlink under /dev/disk/by-id
```

That's how it generates

```
scsi-SATA_WDC_WD3200AAJS-_WDWMAV2FU80671
```

The kernel never invents that filename.

---

## After `udevd` creates the symlink, does the kernel use it?

No.

This is one of the biggest misconceptions people have.

The kernel never uses

```
/dev/sda
```

or

```
/dev/disk/by-id/...
```

Those are **userspace filenames**.

The kernel already has the device internally.

When a program does

```
fdisk /dev/disk/by-id/scsi-SATA_WDC_WD3200AAJS-_WDWMAV2FU80671
```

the following happens:

```
fdisk
    │
    ▼
open("/dev/disk/by-id/...")
    │
Filesystem resolves symlink
    │
    ▼
/dev/sda
    │
Kernel looks up device node
    │
    ▼
Internal block device
```

The symlink is resolved by the filesystem before the kernel accesses the underlying block device. The kernel itself is not looking up `by-id` names to find hardware.

---

## Does `udevd` keep persistent storage?

Not in the way you might imagine.

It does **not** keep a database saying:

```
This serial→ always maps to sda
```

Instead, on every boot:

1. Kernel discovers the hardware.
2. Kernel exposes device attributes in `/sys`.
3. `udevd` reads those attributes.
4. `udevd` recreates the symlinks.

Suppose today:

```
Kernel assignssda = WD disk
```

`udevd` creates

```
by-id/scsi-SATA_WDC... -> ../../sda
```

Tomorrow, because you plugged in another disk first:

```
Kernel assignssdb = WD disk
```

`udevd` reads the serial again and now creates

```
by-id/scsi-SATA_WDC... -> ../../sdb
```

Notice that the **symlink name stays the same**, but its **target changes** depending on the kernel's assignment.

So the persistence comes from the **hardware identifier** (serial number, WWN, etc.), not because `udevd` remembered anything from a previous boot.

---

### Reading a file

## Step 1: You mount the filesystem

Suppose you run:

```
mount /dev/sda1 /home
```

During the mount, the kernel associates the filesystem with that block device.

Conceptually, it stores something like:

```
Mount point: /home
Filesystem: ext4
Backing block device: sda1
```

This information is kept **in kernel memory**, not on the disk.

---

## Step 2: You read a file

Now you do:

```
cat /home/user/lol.txt
```

The kernel sees:

- `/home` is a mount point.
- The filesystem mounted there is ext4.
- That ext4 filesystem is backed by the block device `sda1`.

So the flow is:

```
cat
   │
   ▼
open("/home/user/lol.txt")
   │
   ▼
VFS finds mount point (/home)
   │
   ▼
ext4 filesystem
   │
   ▼
inode #12345
   │
   ▼
inode says:
data blocks = 105,106,107
   │
   ▼
ext4 asks kernel:
"Read block 105"
   │
   ▼
Kernel knows ext4 is on sda1
   │
   ▼
Read from sda1
```

Notice that **`lol.txt` never contains "/dev/sda1".**

---

## Where are the inodes stored?

The inode is stored **on the filesystem itself**, which lives on the partition.

For example:

```
sda1
│
├── Superblock
├── Block groups
├── Inode tables
├── Data blocks
```

The inode contains things like:

- owner
- permissions
- timestamps
- file size
- block numbers

For example:

```
Inode 12345

size = 2048 bytes

blocks:
105
106
107
```

Notice it **doesn't say**:

```
stored on /dev/sda1
```

because the inode is already **inside** the filesystem on `sda1`.