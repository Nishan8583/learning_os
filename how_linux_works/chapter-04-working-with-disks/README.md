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

