## What Is Swap?

Swap is disk space used as an extension of RAM.

When physical memory (RAM) becomes scarce, Linux can move memory pages between:

- RAM
- Disk

This process is called **swapping**.

---

## Why Swap Exists

Suppose:

```
RAM Full
```

Linux can move inactive memory pages to swap space and free RAM for active processes.

### Trade-off

Benefits:

- Prevents immediate memory exhaustion

Drawbacks:

- Disk access is much slower than RAM
- Excessive swapping severely hurts performance

---

# Viewing Swap Usage

Use:

```
$ free
```

Example output:

```
$ free
total used free
--snip--
Swap: 514072 189804 324268
```

Meaning:

|Field|Meaning|
|---|---|
|Total|Total swap available|
|Used|Currently used swap|
|Free|Remaining swap|

---

# 4.3.1 Using a Disk Partition as Swap Space

## Step 1: Ensure Partition Is Empty

The partition should contain no useful data.

---

## Step 2: Create Swap Signature

```
# mkswap dev
```

Example:

```
# mkswap /dev/sda5
```

### What it Does

Creates a swap signature on the partition, marking it as swap space rather than:

- ext4
- xfs
- FAT32
- another filesystem

---

## Step 3: Enable Swap

```
# swapon dev
```

Example:

```
# swapon /dev/sda5
```

Registers the swap area with the kernel.

---

## Automatically Enable at Boot

Add an entry to:

```
/etc/fstab
```

Example:

```
/ dev/sda5 none swap sw 0 0
```

(Book formatting normalized:)

```
/dev/sda5 none swap sw 0 0
```

### Fields

|Field|Value|
|---|---|
|Device|`/dev/sda5`|
|Mount Point|`none`|
|Type|`swap`|
|Options|`sw`|

---

## UUIDs

Swap partitions also receive UUIDs.

Modern systems often use:

```
UUID=<uuid> none swap sw 0 0
```

instead of raw device names.

---

# 4.3.2 Using a File as Swap Space

Instead of dedicating an entire partition, Linux can use a regular file as swap.

This is useful when:

- Disk repartitioning is inconvenient
- Additional swap is needed quickly

---

## Step 1: Create an Empty File

```
# dd if=/dev/zero of=swap_file bs=1024k count=num_mb
```

### Parameters

|Parameter|Meaning|
|---|---|
|if|Input file|
|/dev/zero|Source of zero bytes|
|of|Output file|
|swap_file|Swap file name|
|bs|Block size|
|count|Number of blocks|

### Example

Create a 2 GB swap file:

```
# dd if=/dev/zero of=/swapfile bs=1024k count=2048
```

---

## Step 2: Initialize as Swap

```
# mkswap swap_file
```

Example:

```
# mkswap /swapfile
```

---

## Step 3: Activate

```
# swapon swap_file
```

Example:

```
# swapon /swapfile
```

---

## Disable Swap

Use:

```
# swapoff
```

Example:

```
# swapoff /swapfile
```

or

```
# swapoff /dev/sda5
```

### Requirement

The system must have enough remaining RAM and swap space to absorb any pages currently stored in the area being removed.

---

# Summary: Creating Swap File

```
dd if=/dev/zero of=/swapfile bs=1024k count=2048
mkswap /swapfile
swapon /swapfile
```

---

# 4.3.3 How Much Swap Do You Need?

## Old Rule

Historically:

```
Swap = 2 × RAM
```

Example:

|RAM|Recommended Swap|
|---|---|
|4 GB|8 GB|
|8 GB|16 GB|

This rule came from multi-user Unix systems where many users were logged in simultaneously.

---

## Why the Rule Is Less Useful Today

Modern systems often have:

- Large amounts of RAM
- Large disks
- Different workloads

You may:

- Never use swap
- Or require significant swap depending on workloads

---

## When Swap Helps

Swap works well when:

- Processes are idle
- Memory pages are rarely accessed

Examples:

- Background applications
- Inactive browser tabs
- Sleeping processes

---

## When Swap Hurts

If many active processes need memory simultaneously:

```
RAM ↔ Disk ↔ RAM ↔ Disk
```

constant swapping occurs.

This is called **thrashing**.

### Result

- Severe performance degradation
- High disk I/O
- Slow applications

---

## Solutions to Excessive Swapping

1. Add more RAM
2. Stop some processes
3. Reduce workload

(The book humorously adds:)

> or complain

---

# Why Some Systems Use No Swap

Sometimes Linux swaps out memory pages to gain additional filesystem cache.

Some administrators dislike this behavior and disable swap entirely.

Common example:

High-performance servers.

Goals:

- Avoid swap latency
- Keep everything in RAM
- Minimize disk access

---

# Danger of Running Without Swap

If a machine exhausts all memory:

```
RAM = FullSwap = Full (or absent)
```

Linux invokes the:

**OOM Killer** (Out Of Memory Killer)

### Purpose

Kill one or more processes to recover memory.

Consequences:

- Applications terminate unexpectedly.
- Unsaved work may be lost.

---

## Why High-Performance Servers Can Risk No Swap

Such systems typically have:

- Monitoring
- Redundancy
- Load balancing
- Capacity planning

to ensure memory exhaustion rarely occurs.

General-purpose desktops usually do **not** have these protections and therefore benefit from having some swap available.