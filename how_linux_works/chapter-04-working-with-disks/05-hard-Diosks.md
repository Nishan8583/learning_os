# 4.1.4 Navigating Disk and Partition Geometry — Notes & Explanation

## Big Picture

The author is explaining that although Linux treats a hard disk as a simple collection of numbered blocks, traditional hard disks have physical moving parts that affect performance.

Historically, understanding the physical layout of a disk was important when creating partitions. Modern disks hide most of this complexity through **Logical Block Addressing (LBA)**, so concepts like cylinders and heads are mostly historical.

---

# How a Traditional Hard Disk Works

A traditional HDD contains:

1. **Platter**
    - Circular magnetic disk that stores data.
    - Spins continuously.
2. **Spindle**
    - The axle that the platter rotates around.
3. **Read/Write Head**
    - Reads and writes magnetic data.
4. **Arm**
    - Moves the head across the platter.

### Simplified View

```
          Head
            |
            V
     -----------------
    /                 \
   /      Platter      \
   \                   /
    \                 /
     -----------------
            ^
            |
         Spindle
```

As the platter spins, the head reads data beneath it.

---

# Cylinder

When the arm is positioned at a particular distance from the center:

```
      -------------------
     /                   \
    /      ========       \
   |      Cylinder         |
    \      ========       /
     \                   /
      -------------------
```

The head can only access a circular path.

This circular path is called a **track**.

With multiple platters stacked together:

```
Top platter     =====
Middle platter  =====
Bottom platter  =====
```

The set of tracks directly above each other forms a:

## Cylinder

Think of a cylinder as:

> "All tracks at the same radius across all platters."

---

# Track vs Cylinder

The note says:

> A track is the part of a cylinder that a single head accesses.

### Track

One circular ring on one platter.

### Cylinder

Collection of tracks at the same radius across all platters.

Example:

```
Platter 1: Track 50
Platter 2: Track 50
Platter 3: Track 50
```

Together:

```
Cylinder 50
```

---

# Sector

A track can be divided into smaller pieces.

```
         /|
       /  |
     /    |
    -------
```

Each slice is called a:

## Sector

Historically:

- Sector = smallest addressable storage unit
- Often 512 bytes

Example:

```
Track
 ├─ Sector 1
 ├─ Sector 2
 ├─ Sector 3
 └─ Sector N
```

---

# CHS Addressing

Older systems located data using:

## CHS = Cylinder + Head + Sector

Example:

```
Cylinder: 200
Head: 3
Sector: 15
```

This uniquely identified a location on the disk.

Think of it like:

```
Building = Cylinder
Floor    = Head
Room     = Sector
```

---

# Why CHS Became a Problem

As disks became larger:

- More platters
- More tracks
- More sectors

CHS became complicated and limited.

Modern disks also have:

### Variable Density

Outer tracks are larger:

```
Outer track  ---> more sectors
Inner track  ---> fewer sectors
```

Because the outer circumference is larger.

Example:

```
Outer track:
-------------------------
lots of data

Inner track:
-----------
less data
```

CHS cannot accurately describe this.

---

# Logical Block Addressing (LBA)

Modern disks use:

## LBA = Logical Block Addressing

Instead of:

```
Cylinder 200
Head 3
Sector 15
```

The OS sees:

```
Block 0
Block 1
Block 2
Block 3
...
Block N
```

The disk controller translates:

```
LBA 500000
```

into whatever physical location is required.

This greatly simplifies disk access.

---

## Example

Instead of:

```
CHS:
Cylinder 200
Head 3
Sector 15
```

Modern systems use:

```
LBA:
500000
```

Just a block number.

Linux mostly works this way.

---

# Why MBR Still Contains CHS

The text mentions:

> MBR contains CHS information as well as LBA equivalents.

Historical compatibility.

An MBR partition entry stores:

```
Start CHS
End CHS

Start LBA
Size
```

Modern systems use:

```
Start LBA
Size
```

The CHS values are mostly ignored.

---

# Why the Book Calls CHS Values "Fiction"

Modern drives:

- Use LBA internally
- Remap bad sectors
- Use caching
- Use translation layers

Therefore:

```
Reported cylinders
Reported heads
Reported sectors
```

are often fake values invented for compatibility.

Example:

A drive might report:

```
255 heads
63 sectors
```

even though its real physical geometry is completely different.

The numbers are essentially a compatibility lie.

---

# Are Cylinder Boundaries Important?

## Historically: YES

Suppose a file occupies one cylinder.

```
Cylinder A
```

The head stays still while the platter spins.

Very efficient.

---

Suppose the file spans many nearby cylinders.

```
Cylinder 10
Cylinder 11
Cylinder 12
Cylinder 13
```

The arm moves only slightly.

Still efficient.

---

## Why Partition Alignment Mattered

Old partitioning tools tried to place partitions on cylinder boundaries:

```
Partition 1
|-----------|

Partition 2
            |-----------|
```

This reduced unnecessary head movement.

---

# Today: Mostly No

Modern disks:

- Hide physical geometry
- Use LBA
- Use internal optimization

Therefore:

### Ignore warnings like:

> "Partition does not start on cylinder boundary"

Those warnings are relics from older systems.

Modern partitioning tools:

- `fdisk`
- `parted`
- `gparted`

generally create sensible layouts automatically.

---

# Important Note About SSDs

Everything discussed here applies primarily to:

- HDDs (Hard Disk Drives)

SSDs have:

- No platters
- No heads
- No cylinders
- No seek time

They also expose storage through LBA, but internally use flash memory and a translation layer.