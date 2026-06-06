# 4.2.12 Special-Purpose Filesystems

## Key Idea

Not all filesystems store data on physical disks. Some filesystems act as **interfaces to the kernel and operating system**, exposing system information through files and directories.

This follows the Unix philosophy that **"everything is a file."**

Examples include:

- Process information
- Hardware information
- Kernel settings
- Temporary in-memory storage
- Container filesystem layers

The idea evolved from:

- `/dev` device files
- `/proc` process filesystem
- Research from Bell Labs and the **Plan 9** operating system

---

# Common Special-Purpose Filesystems

## 1. proc Filesystem (`/proc`)

### Purpose

Provides information about:

- Running processes
- Kernel state
- Hardware information

Mounted at:

```
/proc
```

### Process Directories

Each numbered directory corresponds to a process ID (PID).

Example:

```
/proc/1234
```

represents process `1234`.

### Current Process

```
/proc/self
```

always points to the current process accessing it.

### Hardware Information Example

```
/proc/cpuinfo
```

contains CPU information.

### Important Note

Historically, Linux placed lots of system information in `/proc`.

Modern kernel guidelines recommend moving non-process-related information into:

```
/sys
```

Therefore, `/proc` is not always the preferred interface for newer system information.

---

## 2. sysfs Filesystem (`/sys`)

### Purpose

Provides structured kernel and device information.

Mounted at:

```
/sys
```

### Characteristics

- Represents devices
- Represents drivers
- Represents kernel subsystems
- Preferred location for modern kernel information

---

## 3. tmpfs Filesystem

### Purpose

Provides temporary storage using:

- RAM
- Swap space

Common mount point:

```
/run
```

### Characteristics

- Very fast
- Data disappears after reboot
- Uses memory rather than disk

### Size Control

When mounting tmpfs, size can be controlled using:

- `size`
- `nr_blocks`

mount options.

### Warning

Because tmpfs uses memory:

- Filling it excessively consumes RAM.
- Eventually the system may run out of memory.
- Programs can crash.

---

## 4. squashfs Filesystem

### Purpose

A compressed, read-only filesystem.

### Characteristics

- Files stored in compressed form
- Data extracted on demand
- Frequently accessed through loopback devices

### Example Use

The Snap package system mounts packages under:

```
/snap
```

using SquashFS.

### Advantages

- Saves disk space
- Read-only
- Good for packaged software distribution

---

## 5. overlay Filesystem

### Purpose

Combines multiple directories into a single unified filesystem view.

### Characteristics

- Creates a composite filesystem
- Commonly used by containers
- Allows writable layers on top of read-only layers

### Common Usage

Container technologies such as Docker frequently use OverlayFS.

---

# Summary Table: Special-Purpose Filesystems

| Filesystem | Mount Point        | Purpose                                   |
| ---------- | ------------------ | ----------------------------------------- |
| proc       | `/proc`            | Process and kernel information            |
| sysfs      | `/sys`             | Device and kernel subsystem information   |
| tmpfs      | `/run`             | RAM-backed temporary storage              |
| squashfs   | `/snap` (commonly) | Compressed read-only filesystem           |
| overlay    | Various            | Merges multiple directories into one view |
