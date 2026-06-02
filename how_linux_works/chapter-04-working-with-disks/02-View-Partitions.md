# Viewing a Partition Table

## Viewing Partition Tables with `parted`

To display all partition tables on the system:

```
# parted -l
```

### Example Output

```bash
# parted -l

Model: ATA KINGSTON SM2280S (scsi)
Disk /dev/sda: 240GB
Sector size (logical/physical): 512B/512B
Partition Table: msdos
Disk Flags:

Number  Start   End     Size    Type      File system       Flags
1       1049kB  223GB   223GB   primary   ext4              boot
2       223GB   240GB   17.0GB  extended
5       223GB   240GB   17.0GB  logical   linux-swap(v1)

Model: Generic Flash Disk (scsi)
Disk /dev/sdf: 4284MB
Sector size (logical/physical): 512B/512B
Partition Table: gpt
Disk Flags:

Number  Start   End      Size     File system  Name      Flags
1       1049kB  1050MB   1049MB                myfirst
2       1050MB  4284MB   3235MB                mysecond
```

---

# Understanding the Output

The example shows two disks:

|Device|Partition Table Type|
|---|---|
|`/dev/sda`|MBR (`msdos`)|
|`/dev/sdf`|GPT|

Notice:

```
Partition Table: msdos
```

means:

```
MBR partition table
```

while:

```
Partition Table: gpt
```

means:

```
GUID Partition Table (GPT)
```

---

# MBR vs GPT Output Differences

The partition tables store different metadata.

### MBR Example

```
Number  Start   End     Size    Type      File system
1       ...     ...     ...     primary   ext4
```

### GPT Example

```
Number  Start   End     Size     Name
1       ...     ...     ...      myfirst
```

Notice GPT contains a:

```
Name
```

column.

MBR does not.

### Why?

GPT supports:

- Partition names
- GUIDs
- Additional metadata

MBR does not.

Example GPT partition names:

```
myfirstmysecond
```

These names were arbitrarily assigned by the author.

---

# Be Careful with Units

## `parted`

Displays human-friendly sizes:

```
223GB
17.0GB
1049MB
```

These are approximations.

---

## `fdisk -l`

Displays exact sizes, often in sectors.

Example:

```
Start      End
2048   468860927
```

Since sectors are often:

```
512 bytes
```

the numbers can appear much larger than expected.

### Important

Always check:

```
Sector size (logical/physical): 512B/512B
```

when interpreting partition sizes.

---

# MBR Partition Types

The example MBR disk contains:

```
/dev/sda1
/dev/sda2
/dev/sda5
```

These represent three different concepts.

---

## Primary Partition

A normal partition directly described in the MBR.

Example:

```
/dev/sda1
```

```
Disk
├── Primary Partition
```

---

## MBR Limitation

Traditional MBR supports only:

```
4 primary partitions
```

maximum.

---

## Extended Partition

To overcome the 4-partition limit, one primary partition can be designated as:

```
Extended Partition
```

Example:

```
/dev/sda2
```

The extended partition is essentially a container.

```
Disk
├── Primary
├── Extended
```

---

## Logical Partition

Logical partitions live inside the extended partition.

Example:

```
/dev/sda5
```

Visualization:

```
Disk
├── /dev/sda1 (Primary)
└── /dev/sda2 (Extended)
      └── /dev/sda5 (Logical)
```

Linux uses logical partitions just like normal partitions.

---

# MBR Partition Numbering

You may wonder:

```
Why sda5 and not sda3?
```

Historically:

```
sda1 - sda4
```

are reserved for primary/extended partitions.

Logical partitions begin at:

```
sda5
```

even if:

```
sda3
sda4
```

do not exist.

Example:

```
sda1  Primary
sda2  Extended
sda5  Logical
```

This is completely normal.

---

# Filesystem Type vs MBR System ID

The book points out an important distinction.

## What `parted` Shows

```
linux-swap(v1)
ext4
```

These are filesystem types that `parted` detects by examining the partition contents.

---

## What MBR Actually Stores

MBR stores a numeric partition type called the:

```
System ID
```

Examples:

|ID|Meaning|
|---|---|
|83|Linux partition|
|82|Linux swap|
|07|NTFS|
|0f|Extended partition|

The filesystem displayed by `parted` is not necessarily the same thing as the stored MBR type code.

---

## To View Actual MBR Type IDs

Use:

```
fdisk -l
```

instead of:

```
parted -l
```

---

# LVM Partitions (Preview)

If your system uses LVM, you may encounter:

### LVM Partition Type

```
8e
```

(MBR type code)

or:

```
lvm
```

flag in `parted`.

Example:

```
Number  Start   End     Size     Type     Flags
1       1049kB  10.7GB  10.7GB   primary  boot,lvm
```

---

# Device Mapper Devices

LVM creates virtual block devices.

Examples:

```
/dev/dm-0
/dev/dm-1
```

or

```
/dev/mapper/ubuntu--vg-root
/dev/mapper/ubuntu--vg-swap_1
```

These are not physical disks.

They are virtual devices created by the Linux Device Mapper.

---

## Example LVM Output

### Root Filesystem

```
Disk /dev/mapper/ubuntu--vg-root: 9672MB

1 0.00B 9672MB ext4
```

### Swap Volume

```
Disk /dev/mapper/ubuntu--vg-swap_1: 1023MB

1 0.00B 1023MB linux-swap(v1)
```

---

# Simple Mental Model of LVM

Traditional Partitioning:

```
Disk
 └── Partition
      └── Filesystem
```

LVM:

```
Disk
 └── LVM Partition
       └── Logical Volume
             └── Filesystem
```

The book's simplified explanation:

> The partitions appear to have been separated from the partition table.

The details are covered later in the LVM chapter.

---

# Viewing Kernel Partition Detection

When Linux boots, the kernel reads partition tables and logs what it discovers.

View kernel messages:

```
journalctl -k
```

Example output:

```
sda: sda1 sda2 < sda5 >
```

---

# Interpreting the Kernel Output

```
sda
```

Physical disk.

```
sda1
```

Primary partition.

```
sda2
```

Extended partition.

```
<sda5>
```

Logical partition inside the extended partition.

Visualization:

```
sda
├── sda1
└── sda2 (Extended)
      └── sda5
```

The angle brackets indicate:

```
Logical partitions contained inside the extended partition
```

---

# Key Takeaways

- Use `parted -l` to view all partition tables.
- `msdos` in `parted` means an MBR partition table.
- GPT supports partition names; MBR does not.
- `parted` displays human-readable sizes; `fdisk -l` often displays exact sector counts.
- MBR supports:
    - Up to 4 primary partitions.
    - Additional partitions through an extended partition.
- Logical partitions reside inside an extended partition and begin numbering at `5`.
- The filesystem type shown by `parted` is not necessarily the MBR system ID.
- Use `fdisk -l` to see actual MBR type codes.
- Systems using LVM often contain:
    - LVM partition type `8e`
    - `/dev/dm-*`
    - `/dev/mapper/*`
- Kernel partition discovery messages can be viewed with:

```
journalctl -k
```

and output such as:

```
sda: sda1 sda2 < sda5 >
```

indicates an extended partition (`sda2`) containing logical partition (`sda5`).