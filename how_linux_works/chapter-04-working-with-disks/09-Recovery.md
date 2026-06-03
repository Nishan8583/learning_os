# 4.2.7 Remounting a Filesystem

## What is Remounting?

A filesystem may already be mounted, but sometimes you need to change its mount options without unmounting it.

Common situation:

```
Filesystem is mounted read-only
↓
Need to make it writable
↓
Remount it
```

This often happens during:

- Crash recovery
- Single-user mode maintenance
- Repairing a damaged system

---

## Remount Root Filesystem as Read-Write

Command:

```
mount -n -o remount /
```

### Meaning

|Option|Meaning|
|---|---|
|`-n`|Do not update `/etc/mtab`|
|`-o remount`|Reattach existing filesystem with new options|
|`/`|Root filesystem|

---

### Example

Root currently:

```
/ mounted read-only
```

Run:

```
# mount -n -o remount /
```

Output:

```
(no output)
```

Result:

```
/ mounted read-write
```

---

## Why Use `-n`?

Normally mount updates:

```
/etc/mtab
```

However:

```
Root filesystem
    ↓
Read-only
    ↓
Cannot update /etc/mtab
```

Therefore:

```
mount -n -o remount /
```

prevents mount from attempting to write to `/etc/mtab`.

---

## Important

This command assumes the root filesystem entry exists in:

```
/etc/fstab
```

If it doesn't, you must specify the device explicitly.

Example:

```
mount -n -o remount,rw /dev/sda1 /
```

---

# 4.2.8 The /etc/fstab Filesystem Table

## Purpose

Linux stores permanent filesystem configuration in:

```
/etc/fstab
```

This file determines:

- What filesystems exist
- Where they mount
- Mount options
- Boot-time mounting behavior

---

## Example `/etc/fstab`

```
UUID=70ccd6e7-6ae6-44f6-812c-51aab8036d29 / ext4 errors=remount-ro 0 1

UUID=592dcfd1-58da-4769-9ea8-5f412a896980 none swap sw 0 0

/dev/sr0 /cdrom iso9660 ro,user,nosuid,noauto 0 0
```

---

# Structure of an fstab Entry

Each line contains six fields:

```
Device   MountPoint   Type   Options   Dump   Fsck
```

---

## Field 1: Device or UUID

Examples:

```
UUID=70ccd6e7-6ae6-44f6-812c-51aab8036d29
/dev/sr0
```

Modern systems usually prefer UUIDs.

---

## Field 2: Mount Point

Examples:

```
/
/home
/cdrom
```

Where the filesystem will appear.

---

## Field 3: Filesystem Type

Examples:

```
ext4
xfs
vfat
swap
iso9660
```

---

## Field 4: Mount Options

Examples:

```
defaults
ro
nosuid
user
```

Multiple options:

```
ro,user,nosuid,noauto
```

---

## Field 5: Dump Field

Historical backup utility:

```
dump
```

Almost never used today.

Always:

```
0
```

---

## Field 6: fsck Order

Controls boot-time filesystem checking.

### Root filesystem

```
1
```

Example:

```
UUID=abc123 / ext4 defaults 0 1
```

Checked first.

---

### Other Local Filesystems

```
2
```

Example:

```
UUID=xyz456 /home ext4 defaults 0 2
```

Checked after root.

---

### No Checking

```
0
```

Used for:

- Swap
- CD-ROM
- USB media
- proc
- sysfs

---

# Mount Shortcuts Using fstab

Suppose fstab contains:

```
/dev/sr0 /cdrom iso9660 ro,user,nosuid,noauto 0 0
```

Instead of:

```
mount /dev/sr0 /cdrom
```

you can simply run:

```
mount /cdrom
```

Linux looks up the entry in `/etc/fstab`.

---

# Mount Everything in fstab

Command:

```
mount -a
```

Meaning:

```
Mount every entryexcept those marked noauto
```

Example:

```
# mount -a
```

Output:

```
(no output if successful)
```

---

# Important fstab Options

---

## defaults

Use normal mount defaults.

Example:

```
UUID=abc123 /home ext4 defaults 0 2
```

Includes:

```
rw
exec
suid
dev
auto
```

Use when no special behavior is required.

---

## errors=

Ext2/3/4 specific.

Controls behavior when filesystem errors occur.

---

### errors=continue

```
errors=continue
```

Kernel:

```
Report errorKeep running
```

Default behavior.

---

### errors=remount-ro

```
errors=remount-ro
```

Kernel:

```
Error detected↓Remount filesystem read-only
```

Common on Ubuntu.

Example:

```
UUID=abc123 / ext4 errors=remount-ro 0 1
```

---

### errors=panic

```
errors=panic
```

Kernel:

```
Filesystem error
↓
Kernel panic
↓
System halts
```

Used on critical systems.

---

## noauto

Example:

```
/dev/sr0 /cdrom iso9660 noauto 0 0
```

Prevents:

```
mount -a
```

from mounting the device.

Useful for:

- USB drives
- CD-ROMs
- External disks

---

## user

Allows normal users to mount a filesystem.

Example:

```
/dev/sr0 /cdrom iso9660 user 0 0
```

Now:

```
mount /cdrom
```

can be executed by a non-root user.

---

### Security Effects of user

Automatically implies:

```
nosuid
noexec
nodev
```

to prevent privilege escalation.

---

# 4.2.9 Alternatives to /etc/fstab

## Method 1: /etc/fstab.d

Instead of one large file:

```
/etc/fstab
```

Use:

```
/etc/fstab.d/
```

Example:

```
/etc/fstab.d/root.conf
/etc/fstab.d/home.conf
/etc/fstab.d/backup.conf
```

One filesystem per file.

---

## Method 2: systemd Mount Units

Systemd can manage mounts directly.

Examples:

```
home.mount
var.mount
data.mount
```

Often generated from `/etc/fstab`.

---

# 4.2.10 Filesystem Capacity

## Viewing Disk Usage

Command:

```
df
```

Example:

```
$ df
```

Output:

```
Filesystem     1K-blocks     Used Available Use% Mounted on
/dev/sda1      214234312 127989560 75339204 63% /
/dev/sdd2        3043836      4632   2864872 1% /media/user/uuid
```

---

## Understanding the Fields

|Field|Meaning|
|---|---|
|Filesystem|Device|
|1K-blocks|Total capacity|
|Used|Space used|
|Available|Free space|
|Use%|Percentage used|
|Mounted on|Mount point|

---

## Check Current Filesystem Only

Very useful:

```
df .
```

Output:

```
Filesystem     1K-blocks     Used Available Use% Mounted on
/dev/sda1      214234312 127989560 75339204 63% /
/dev/sdd2        3043836      4632   2864872 1% /media/user/uuid
```

`.` means current directory.

---

## Human Readable Output

```
df -h
```

Example:

```
Filesystem      Size Used Avail Use% Mounted on
/dev/sda1       204G 122G 72G 63% /
```

---

## MB Units

```
df -m
```

---

## Reserved Blocks

Notice:

```
Used + Available ≠ Total
```

Example:

```
127989560 + 75339204
≠
214234312
```

Why?

Linux reserves about:

```
5%
```

for root.

Benefits:

- Prevent total filesystem exhaustion
- Allow system services to continue working

---

# Finding Large Files and Directories

## du Command

Shows disk usage.

---

### Current Directory Tree

```
du
```

Example:

```
4 ./docs
8 ./images
120 ./videos
132 .
```

---

### Summary Only

```
du -s
```

Example:

```
132 .
```

---

### Everything in Current Directory

```
du -s *
```

Example:

```
12 docs104 videos8 images
```

Useful for finding space hogs.

---

### Human Readable

```
du -sh *
```

Example:

```
12 docs
104 videos
8 images
```

---

# 4.2.11 Checking and Repairing Filesystems

## Why Filesystems Become Corrupted

Common causes:

- Power failures
- Pulling power cord
- Hardware failure
- Bad disks
- Bad cables
- Kernel crashes

---

## Filesystem Checker

Command:

```
fsck
```

Example:

```
# fsck /dev/sdb1
```

For ext filesystems:

```
fsck
 ↓
e2fsck
```

automatically.

---

# CRITICAL WARNING

Never run fsck on a mounted filesystem.

Bad:

```
fsck /dev/sda1
```

while mounted.

Can cause:

- corruption
- crashes
- data loss

---

## Exception

Root mounted read-only in single-user mode.

---

## Example Successful fsck

```
Pass 1: Checking inodes, blocks, and sizes
Pass 2: Checking directory structure
Pass 3: Checking directory connectivity
Pass 4: Checking reference counts
Pass 5: Checking group summary information

/dev/sdb1: 11/1976 files (0.0% non-contiguous),
265/7891 blocks
```

---

# Automatic Repair

```
fsck -p /dev/sdb1
```

or

```
fsck -a /dev/sdb1
```

Meaning:

```
Fix ordinary problems automatically
Stop if serious problems found
```

Linux uses this mode during boot.

---

# Read-Only Check

```
fsck -n /dev/sdb1
```

Meaning:

```
Check onlyDo not modify anything
```

Useful during disaster recovery.

---

# Answer Yes to Everything

```
fsck -y /dev/sdb1
```

Automatically answers:

```
yes
yes
yes
yes
```

to every repair question.

Use only as a last resort.

---

# Recovering a Corrupted Superblock

Use alternate superblock:

```
fsck -b block_number /dev/sdb1
```

Example:

```
fsck -b 32768 /dev/sdb1
```

---

## Finding Backup Superblocks

Safe command:

```
mkfs -n /dev/sdb1
```

Output may include:

```
Superblock backups stored on blocks:
32768
98304
163840
```

The `-n` is critical.

Without it:

```
Filesystem destroyed
```

---

# Ext3 / Ext4 Journal Recovery

Flush journal:

```
e2fsck -fy /dev/sdb1
```

Options:

|Option|Meaning|
|---|---|
|`-f`|Force check|
|`-y`|Answer yes|

---

# Worst-Case Recovery

Options:

### Copy entire disk

```
dd if=/dev/sdb of=/dev/sdc
```

---

### Mount Read-Only

```
mount -o ro /dev/sdb1 /mnt/recovery
```

Salvage files manually.

---

### debugfs

Open filesystem directly:

```
debugfs /dev/sdb1
```

Useful when normal mounting fails.

---

# 4.2.12 Special-Purpose Filesystems

These filesystems do not store normal disk data.

They provide kernel interfaces.

---

# proc Filesystem

Mounted at:

```
/proc
```

Contains process and kernel information.

Examples:

```
ls /proc
```

Output:

```
1
20
500
self
cpuinfo
meminfo
```

---

## CPU Information

```
cat /proc/cpuinfo
```

Example:

```
processor : 0
vendor_id : GenuineIntel
```

---

## Current Process

```
/proc/self
```

Always refers to the current process.

---

# sysfs

Mounted at:

```
/sys
```

Contains:

- Devices
- Drivers
- Kernel objects

Example:

```
eth0
lo
wlan0
```

Output:

```
eth0lowlan0
```

---

# tmpfs

Memory-backed filesystem.

Often mounted on:

```
/run
```

or

```
/dev/shm
```

Characteristics:

```
RAM + Swap
```

Storage disappears after reboot.

---

### Create a tmpfs

```
mount -t tmpfs tmpfs /mnt/ramdisk
```

Example:

```
mount -t tmpfs -o size=512M tmpfs /mnt/ramdisk
```

Creates:

```
512 MB RAM disk
```

---

# squashfs

Compressed read-only filesystem.

Used heavily by:

- Live CDs
- Snap packages

Example mount location:

```
/snap
```

Benefits:

- Compression
- Read-only
- Space efficient

---

# overlay Filesystem

Combines multiple directories into one view.

Concept:

```
Lower Layer (read-only)
          +
Upper Layer (writable)
          ↓
Merged View
```

Widely used by:

- Docker
- Podman
- Containers