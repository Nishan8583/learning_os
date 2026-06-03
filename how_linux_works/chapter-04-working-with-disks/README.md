### Create Partitions
- List devices using `lsblk`.
- View parition using `parted -l` or `fdisk -l`.
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
- It has not created any Filesystem yet, so we need to create it.
- `sudo mkfs.vfat -F 32 /dev/sdb1` or `sudo mkfs.fat -F 32 /dev/sdb1` it creates FAT32 FileSytem on `/dev/sdb1`, choose the one we just paritioned.


|Command / Input|Description|
|---|---|
|`lsblk`|List block devices and partitions.|
|`parted -l`|Display partition tables for all disks.|
|`fdisk -l`|Display partition tables for all disks.|
|`fdisk /dev/sdd`|Start interactive partitioning on `/dev/sdd`.|
|**`(fdisk) p`**|Print the current partition table.|
|**`(fdisk) d`**|Delete an existing partition.|
|**`(fdisk) n`**|Create a new partition.|
|**`(fdisk) p`**|Choose a primary partition.|
|**`(fdisk) e`**|Choose an extended partition.|
|**`(fdisk) 1`**|Select partition number 1.|
|**`(fdisk) 2048`**|Select first sector (usually the default).|
|**`(fdisk) +200M`**|Create a partition of size 200 MB.|
|**`(fdisk) w`**|Write changes to disk and exit.|
|`sudo mkfs.vfat -F 32 /dev/sdb1`|Create a FAT32 filesystem on `/dev/sdb1`.|
|`sudo mkfs.fat -F 32 /dev/sdb1`|Alternative command to create a FAT32 filesystem on `/dev/sdb1`.|


### Filesystem
- Make new FS ext4 in a newly partitioned disk (don't care about the data because making a new FS will practically make old data useless) `mkfs.ext4 /dev/sdb1`.
- `mkfs -t ext4 /dev/sdf2`.
- Mount the filesystem `mount /dev/sdf2 /home/extra`.
- Mount syntax `mount -t type device mountpoint` type using `-t` might not be needed, mkfs will figure it out, but there might be confusion in FS that are similiar, so better do it i guess ?.
- `mount -t ext4 /dev/sdf2 /home/extra`.
- View mounted fs `mount`.
- `sshfs server:/data ~/remote` because of FUSE (Filesystem in User Space), over ssh, can treat it as normal filesystem.
- `ls ~/remote` now treat it like any other normal directory.
- To unmount `umount /home/extra` or `umount /dev/sdf2`
- For temprorary can use `/mnt` mount point`mount /dev/sdb1 /mnt`.
- List UUIDs of devices using `sudo blkid`.
- Mount by UUID `sudo mount UUID=b600fe63-d2e9-461c-a5cd-d3b373a5e1d2 /home/extra`.
- If u clone a device, change the UUID using `sudo tune2fs -U random /dev/sdb1`, if same UUID will confuse linux.
- `sync` flush buffers to disk.
- `sudo umount /mnt/usb`.
- Mount options `mount -o ro /dev/sdb1 /mnt`. Read only.
- Remount without unmounting `sudo mount -o remount,rw /mnt`.
```
remount = do not unmount
rw      = make writable
```
- Allow program execution with exec option `sudo mount -o exec /dev/sdb1 /mnt`. Fix Kali CD ROM issue.
- Check disk usage `df`, for only current directory `df .`, `df -h` for human readable output.
- Filesystem checker `fsck`, interactive.
- `fsck -p /dev/sdb1` auto fix problem if normal problems, if serious exists.
- `fsck -y /dev/sdb1` auto yes to everything, be careful.
- Worst-Case Recovery
    - Copy entire disk

    ```
    dd if=/dev/sdb of=/dev/sdc
    ```
- Mount Read-Only
```
mount -o ro /dev/sdb1 /mnt/recovery
```

Salvage files manually.
- debugfs

- Open filesystem directly:

```
debugfs /dev/sdb1
```

Useful when normal mounting fails.




|Command|Description|
|---|---|
|`mkfs.ext4 /dev/sdb1`|Create a new ext4 filesystem on `/dev/sdb1`. Existing data becomes inaccessible.|
|`mkfs -t ext4 /dev/sdf2`|Create an ext4 filesystem using the generic `mkfs` command.|
|`mount /dev/sdf2 /home/extra`|Mount `/dev/sdf2` at `/home/extra`.|
|`mount -t type device mountpoint`|General mount syntax.|
|`mount -t ext4 /dev/sdf2 /home/extra`|Mount an ext4 filesystem explicitly specifying the type.|
|`mount`|Display all currently mounted filesystems.|
|`sshfs server:/data ~/remote`|Mount a remote directory over SSH using FUSE.|
|`ls ~/remote`|Access the SSH-mounted directory like a normal filesystem.|
|`umount /home/extra`|Unmount filesystem by mount point.|
|`umount /dev/sdf2`|Unmount filesystem by device name.|
|`mount /dev/sdb1 /mnt`|Temporarily mount a filesystem at `/mnt`.|
|`sudo blkid`|List devices, filesystems, and UUIDs.|
|`sudo mount UUID=b600fe63-d2e9-461c-a5cd-d3b373a5e1d2 /home/extra`|Mount a filesystem by UUID.|
|`sudo tune2fs -U random /dev/sdb1`|Generate a new random UUID for an ext2/ext3/ext4 filesystem. Useful after cloning disks.|
|`sync`|Flush filesystem buffers to disk.|
|`sudo umount /mnt/usb`|Safely unmount a USB device.|
|`mount -o ro /dev/sdb1 /mnt`|Mount filesystem in read-only mode.|
|`sudo mount -o remount,rw /mnt`|Remount an already mounted filesystem as read-write without unmounting.|
|`sudo mount -o exec /dev/sdb1 /mnt`|Allow execution of programs from the mounted filesystem.|
|`df`|Show filesystem capacity and usage.|
|`df .`|Show capacity information for the filesystem containing the current directory.|
|`df -h`|Show filesystem usage in human-readable units.|
|`fsck`|Interactive filesystem check and repair.|
|`fsck -p /dev/sdb1`|Automatically fix ordinary filesystem problems; stop on serious errors.|
|`fsck -y /dev/sdb1`|Automatically answer "yes" to all repair prompts. Use carefully.|
|`dd if=/dev/sdb of=/dev/sdc`|Copy an entire disk device to another disk (disaster recovery).|
|`mount -o ro /dev/sdb1 /mnt/recovery`|Mount a damaged filesystem read-only for data recovery.|
|`debugfs /dev/sdb1`|Open an ext filesystem directly for low-level inspection and recovery.|



|Command|Description|
|---|---|
|`mkfs.ext4 /dev/sdb1`|Create a new ext4 filesystem on `/dev/sdb1`. Existing data becomes inaccessible.|
|`mkfs -t ext4 /dev/sdf2`|Create an ext4 filesystem using the generic `mkfs` command.|
|`mount /dev/sdf2 /home/extra`|Mount `/dev/sdf2` at `/home/extra`.|
|`mount -t type device mountpoint`|General mount syntax.|
|`mount -t ext4 /dev/sdf2 /home/extra`|Mount an ext4 filesystem explicitly specifying the type.|
|`mount`|Display all currently mounted filesystems.|
|`sshfs server:/data ~/remote`|Mount a remote directory over SSH using FUSE.|
|`ls ~/remote`|Access the SSH-mounted directory like a normal filesystem.|
|`umount /home/extra`|Unmount filesystem by mount point.|
|`umount /dev/sdf2`|Unmount filesystem by device name.|
|`mount /dev/sdb1 /mnt`|Temporarily mount a filesystem at `/mnt`.|
|`sudo blkid`|List devices, filesystems, and UUIDs.|
|`sudo mount UUID=b600fe63-d2e9-461c-a5cd-d3b373a5e1d2 /home/extra`|Mount a filesystem by UUID.|
|`sudo tune2fs -U random /dev/sdb1`|Generate a new random UUID for an ext2/ext3/ext4 filesystem. Useful after cloning disks.|
|`sync`|Flush filesystem buffers to disk.|
|`sudo umount /mnt/usb`|Safely unmount a USB device.|
|`mount -o ro /dev/sdb1 /mnt`|Mount filesystem in read-only mode.|
|`sudo mount -o remount,rw /mnt`|Remount an already mounted filesystem as read-write without unmounting.|
|`sudo mount -o exec /dev/sdb1 /mnt`|Allow execution of programs from the mounted filesystem.|
|`df`|Show filesystem capacity and usage.|
|`df .`|Show capacity information for the filesystem containing the current directory.|
|`df -h`|Show filesystem usage in human-readable units.|
|`fsck`|Interactive filesystem check and repair.|
|`fsck -p /dev/sdb1`|Automatically fix ordinary filesystem problems; stop on serious errors.|
|`fsck -y /dev/sdb1`|Automatically answer "yes" to all repair prompts. Use carefully.|
|`dd if=/dev/sdb of=/dev/sdc`|Copy an entire disk device to another disk (disaster recovery).|
|`mount -o ro /dev/sdb1 /mnt/recovery`|Mount a damaged filesystem read-only for data recovery.|
|`debugfs /dev/sdb1`|Open an ext filesystem directly for low-level inspection and recovery.|



### Example `/etc/fstab`
- Linux will attempt to mount it during boot (unless you use options like noauto).
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


- FOR sshfs 
- `sudo apt install sshfs`
- `sshfs#nishan@server.example.com:/data  /mnt/remote  fuse.sshfs  defaults,_netdev,user,idmap=user  0  0`

|Option|Meaning|
|---|---|
|`defaults`|Standard mount options|
|`_netdev`|Wait for networking before mounting|
|`user`|Allow non-root user to mount|
|`idmap=user`|Map remote file ownership to local user|
|`noauto`|Don't mount automatically at boot|
- Common for latptop `sshfs#nishan@server.example.com:/data  /mnt/remote  fuse.sshfs  noauto,user,_netdev  0  0`
- `sudo mount -a` to make changes into affect.

# `/etc/fstab` Example (Real Entries)

|Filesystem|Mount Point|Type|Options|Dump|Fsck|
|---|---|---|---|---|---|
|`UUID=70ccd6e7-6ae6-44f6-812c-51aab8036d29`|`/`|`ext4`|`errors=remount-ro`|`0`|`1`|
|`UUID=592dcfd1-58da-4769-9ea8-5f412a896980`|`none`|`swap`|`sw`|`0`|`0`|
|`/dev/sr0`|`/cdrom`|`iso9660`|`ro,user,nosuid,noauto`|`0`|`0`|
|`sshfs#nishan@server.example.com:/data`|`/mnt/remote`|`fuse.sshfs`|`defaults,_netdev,user,idmap=user`|`0`|`0`|
|`sshfs#nishan@server.example.com:/data`|`/mnt/remote`|`fuse.sshfs`|`noauto,user,_netdev`|`0`|`0`|

---

# `/etc/fstab` Structure Explained

|Field|Meaning|Example|
|---|---|---|
|Device / UUID|Identifies filesystem (prefer UUID over `/dev/sdX`)|`UUID=70ccd6e7-...`|
|Mount Point|Where filesystem appears in directory tree|`/`, `/home`, `/cdrom`|
|Filesystem Type|Filesystem format|`ext4`, `xfs`, `vfat`, `swap`, `iso9660`|
|Options|Mount behavior settings|`defaults`, `ro`, `user`, `noauto`|
|Dump|Legacy backup flag (rarely used)|`0`|
|Fsck Order|Boot-time filesystem check order|`0`, `1`, `2`|

---

# SSHFS `/etc/fstab` Options

|Option|Meaning|
|---|---|
|`defaults`|Standard mount behavior|
|`_netdev`|Requires network (wait for network before mounting)|
|`user`|Allow non-root users to mount|
|`idmap=user`|Map remote ownership to local user|
|`noauto`|Do not mount at boot|

---

# Common SSHFS Patterns

|Use Case|fstab Entry|
|---|---|
|Auto-mount at boot (if network available)|`sshfs#user@host:/path /mnt/remote fuse.sshfs defaults,_netdev,user,idmap=user 0 0`|
|Laptop / safer setup (manual mount)|`sshfs#user@host:/path /mnt/remote fuse.sshfs noauto,user,_netdev 0 0`|

---

# Apply Changes

|Action|Command|
|---|---|
|Test `/etc/fstab` changes|`sudo mount -a`|
|Check mounted filesystems|`mount` or `df -h`|
|Manually mount SSHFS|`mount /mnt/remote`|