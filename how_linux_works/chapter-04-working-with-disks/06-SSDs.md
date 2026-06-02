# Why SSDs Are Different

## HDD Access

To read data:

1. Move head to correct track.
2. Wait for platter to rotate.
3. Read data.

```
Seek Time + Rotation Time + Read Time
```

This makes random access expensive.

---

## SSD Access

An SSD stores data in flash memory chips.

```
Flash Cell
Flash Cell
Flash Cell
Flash Cell
```

To access data:

```
Controller → Memory Cell
```

No mechanical movement is required.

Therefore:

```
Random Access ≈ Fast
```

---

# SSDs Read Data in Pages

Unlike HDDs, SSDs do not read arbitrary byte ranges efficiently.

Instead, they read fixed-size chunks called:

## Pages

Common page sizes:

```
4096 bytes   (4 KB)
8192 bytes   (8 KB)
16384 bytes  (16 KB)
```

The SSD controller reads one page at a time.

---

# What Is Partition Alignment?

Suppose an SSD uses:

```
Page Size = 4096 bytes
```

The SSD wants reads to start on:

```
0
4096
8192
12288
16384
...
```

These are page boundaries.

---

## Good Alignment

Example:

```
Partition Start = 8192 bytes
```

Reading data:

```
|----Page 1----|
8192         12288
```

Only one page read is needed.

---

## Bad Alignment

Suppose partition starts at:

```
6144 bytes
```

Now a file might span two pages:

```
Page A      Page B
|-----------|-----------|
      ^
   Partition Start
```

To read a small amount of data:

```
Read Page A
Read Page B
```

instead of:

```
Read Page A only
```

Result:

- More work
- More latency
- Lower performance

---

# Example from the Book

The book says:

```
Page size = 4096 bytes
```

Linux internally often talks about:

```
512-byte sectors
```

Even on SSDs.

---

## Convert Page Size to Sectors

Given:

```
4096 bytes/page
512 bytes/sector
```

Calculation:

```
4096 ÷ 512 = 8
```

A page contains:

```
8 sectors
```

---

# Modern Solution: 1 MB Alignment

Modern partitioning tools avoid alignment problems by starting partitions at:

```
1 MB boundary
```

which is:

```
1048576 bytes
```

or:

```
2048 sectors
```

because:

```
2048 × 512 = 1048576
```

---

## Why 1 MB?

Because it aligns with many possible page sizes.

Examples:

```
1048576 ÷ 4096 = 256
1048576 ÷ 8192 = 128
1048576 ÷ 16384 = 64
```

All divide evenly.

Therefore:

```
1 MB alignment works for most SSDs.
```

This is why modern tools don't try to calculate SSD geometry.

They simply:

```
Start partition at sector 2048
```

which is safe.

---

# Checking Partition Alignment

Linux exposes partition information through:

```
/sys/block
```

Example:

```
cat /sys/block/sdf/sdf2/start
```

Output:

```
1953126
```

This means:

```
Partition starts at sector 1953126
```

Remember:

```
1 sector = 512 bytes
```

---

# Determining Alignment

Suppose:

```
SSD Page Size = 4096 bytes
```

One page contains:

```
4096 / 512 = 8 sectors
```

Therefore:

```
Partition start must be divisible by 8
```

---

## Check the Example

```
1953126 ÷ 8
```

Result:

```
244140 remainder 6
```

Not evenly divisible.

Therefore:

```
Partition is NOT aligned on a 4096-byte page boundary.
```

Performance may be reduced.

---

# Why Linux Calls Them Sectors

The book notes something confusing:

Historically:

```
Sector = physical sector on disk
```

Often:

```
512 bytes
```

Modern Linux still reports offsets using:

```
512-byte units
```

and still calls them:

```
sectors
```

even when they are no longer physical sectors.

So:

```
cat /sys/block/sdf/sdf2/start
```

returns:

```
Number of 512-byte blocks
```

not necessarily real hardware sectors.

---

# What Happens Today?

In practice:

If you create partitions using:

```
fdisk
parted
gparted
```

on a modern Linux system:

```
Alignment is usually correct automatically.
```

You generally only need to worry about this when:

- Investigating performance issues
- Recovering disks
- Working with old partition tables
- Manually creating unusual partitions

---

# HDD vs SSD Alignment

|HDD|SSD|
|---|---|
|Concern is head movement|Concern is page alignment|
|Uses tracks and cylinders|Uses flash pages|
|Seek time matters|No seek time|
|Cylinder boundaries historically important|Page boundaries important|
|Random access slower|Random access fast|
