# 8.5 Resource Monitoring

Resource monitoring in Linux focuses on understanding how the kernel distributes system resources among processes:

- **CPU time**
- **Memory**
- **Disk I/O**
- **Per-process resource utilization**
- **System-wide resource utilization**

The goal is not necessarily to tune the kernel, but to observe how processes consume and compete for resources.

---

## 1. Measuring CPU Time

### Monitoring specific processes with `top`

Use `top -p` to monitor one or more specific PIDs:

```
top -p PID
top -p PID1 -p PID2
```

This allows you to focus on particular processes rather than the entire process list.

### Measuring command execution time

Use `time` to measure how long a command takes:

```
time ls
```

Typical output:

```
real    0m0.442s
user    0m0.052s
sys     0m0.091s
```

#### `user`

CPU time spent executing the program's own code in user space.

#### `sys`

CPU time spent by the kernel performing work on behalf of the process, such as filesystem operations.

#### `real`

Actual elapsed wall-clock time from process start to termination.

Conceptually:

```
real ≈ user + sys + waiting
```

The difference between `real` and `user + sys` can provide an indication of time spent waiting for external resources such as network responses.

> **Important:** Linux shells commonly provide a built-in `time`, while `/usr/bin/time` is a separate utility with more detailed statistics.

---

# 2. Process Priorities

Linux scheduling assigns processes a scheduling priority.

The priority range described here is:

```
-20 ───────── 0 ───────── +20
 ↑                         ↑
Highest priority           Lower priority
```

The confusing part is that **lower numerical priority values represent higher priority**.

### `top` priority fields

Example:

```
PID    PR   NI   ...
28883  20    0   chromium
```

- **PR** — current scheduling priority
- **NI** — nice value

The kernel can adjust scheduling behavior during execution, so priority alone does not completely determine how much CPU time a process receives.

### Nice value

The default nice value is:

```
NI = 0
```

Increasing the nice value makes a process less favored relative to other processes.

For example:

```
renice 20 PID
```

This makes the process "nicer" to other processes by reducing its CPU scheduling preference.

Negative nice values increase priority and require superuser privileges. The text notes that doing this is generally a bad idea because system processes may receive insufficient CPU time.

---

# 3. CPU Performance and Load Average

The **load average** represents the average number of processes that are ready to run.

It includes processes:

- Currently running
- Waiting for an opportunity to use the CPU

Processes waiting for normal input, such as keyboard, mouse, or network input, generally don't contribute to load because they aren't ready to run.

## `uptime`

```
uptime
```

Example:

```
load average: 0.08, 0.03, 0.01
```

The three values represent:

|Value|Period|
|---|---|
|First|1 minute|
|Second|5 minutes|
|Third|15 minutes|

### Interpreting load average

Load must be considered relative to the number of CPU cores.

For a 2-core system:

```
load = 1  → roughly one core worth of runnable work
load = 2  → roughly two cores worth of runnable work
```

A high load average does **not automatically mean a problem**. A system may handle a high load perfectly well if it has sufficient CPU, memory, and I/O resources.

### High load + poor responsiveness

High load combined with system slowdown can indicate memory pressure.

When memory becomes scarce:

```
Memory pressure
      ↓
Swapping
      ↓
Processes wait for memory pages
      ↓
More processes remain runnable/blocked
      ↓
Load average increases
```

Severe swapping can result in **thrashing**, where the kernel spends significant time moving memory pages between RAM and disk instead of doing useful work.

---

# 4. Memory Monitoring

Two simple ways to inspect system memory:

```
free
```

and:

```
cat /proc/meminfo
```

`/proc/meminfo` exposes detailed kernel memory statistics, including memory used for caches and buffers.

---

# 5. Linux Memory and Paging

The CPU's **MMU (Memory Management Unit)** works with the kernel to translate virtual memory addresses into physical memory addresses.

The kernel divides process memory into fixed-size **pages** and maintains **page tables** mapping virtual pages to physical pages.

```
Process virtual address
        ↓
    Page table
        ↓
Physical memory
```

Linux uses **demand paging**: pages don't necessarily have to be loaded into physical memory before a process needs them. Pages are loaded/allocated as required.

### Page size

```
getconf PAGE_SIZE
```

Example:

```
4096
```

The value is in bytes; 4096 bytes (4 KiB) is typical.

---

# 6. Page Faults

A **page fault** occurs when a process accesses a memory page that isn't immediately available through the current MMU mapping.

There are two important types:

## Minor page fault

The page is already in physical memory, but the MMU does not currently have the required mapping.

The kernel establishes the mapping and allows execution to continue.

Minor page faults are normal and occur frequently.

## Major page fault

The requested page isn't currently in physical memory.

The kernel must retrieve it from disk or another slower storage mechanism.

```
Major page fault
      ↓
Disk/storage access
      ↓
Page loaded into RAM
      ↓
Process resumes
```

Many major page faults can significantly hurt performance because storage access is much slower than RAM access.

### Thrashing

When memory becomes severely constrained, the kernel may repeatedly move pages between RAM and disk:

```
RAM → Disk → RAM → Disk → ...
```

This is **thrashing** and can severely degrade system performance.

---

# 7. Measuring Page Faults

Use the system version of `time`:

```
/usr/bin/time command
```

Example:

```
/usr/bin/time cal > /dev/null
```

Example output:

```
2 major + 254 minor page faults
```

The `time` output provides counts for:

- Major page faults
- Minor page faults
- Swaps
- Input/output
- CPU time

The first execution of a program may generate major page faults because its code needs to be loaded from disk. Subsequent executions may generate fewer major faults because the relevant pages can already be cached.

---

## Monitoring page faults with `top`

In `top`:

1. Press `f`
2. Select `nMaj` to display total major page faults.
3. `vMj` can show major page faults since the last update.

`vMj` is useful for identifying a process that is actively generating major page faults.

## Monitoring page faults with `ps`

```
ps -o pid,min_flt,maj_flt PID
```

Example:

```
ps -o pid,min_flt,maj_flt 20365
```

Output:

```
PID    MINFL    MAJFL
20365  834182   23
```

- `MINFL` → minor page faults
- `MAJFL` → major page faults

---

# 8. `vmstat` — CPU, Memory, Swap and I/O

`vmstat` provides a high-level overview of system resource activity with relatively little overhead.

It can show:

- Runnable/blocked processes
- Memory
- Swap
- Disk I/O
- Kernel activity
- CPU utilization

Example:

```
vmstat 2
```

This updates statistics every 2 seconds.

### Important `vmstat` columns

|Section|Columns|Meaning|
|---|---|---|
|Processes|`r`|Runnable processes|
|Processes|`b`|Blocked processes|
|Memory|`swpd`|Memory swapped out|
|Memory|`free`|Free memory|
|Memory|`buff`|Buffer memory|
|Memory|`cache`|Cache memory|
|Swap|`si`|Swap-in|
|Swap|`so`|Swap-out|
|I/O|`bi`|Blocks read|
|I/O|`bo`|Blocks written|
|System|`in`|Interrupts|
|System|`cs`|Context switches|
|CPU|`us`|User CPU time|
|CPU|`sy`|System/kernel CPU time|
|CPU|`id`|Idle CPU|
|CPU|`wa`|CPU waiting for I/O|

The first `vmstat` output represents averages since system startup; subsequent lines provide current interval statistics.

### Identifying memory pressure

Watch for:

```
so > 0
```

indicating pages are being swapped out.

Increasing:

```
si
so
```

combined with decreasing free memory and increased blocked processes can indicate significant memory pressure.

---

# 9. I/O Monitoring with `iostat`

`vmstat` provides general I/O statistics, while `iostat` focuses more specifically on disk/device I/O.

```
iostat
```

Important device statistics:

|Field|Meaning|
|---|---|
|`tps`|Average transfers per second|
|`kB_read/s`|KB read per second|
|`kB_wrtn/s`|KB written per second|
|`kB_read`|Total KB read|
|`kB_wrtn`|Total KB written|

### Continuous monitoring

```
iostat 2
```

Update every 2 seconds.

For device-only statistics:

```
iostat -d 2
```

### Show partition information

```
iostat -p ALL
```

This displays statistics for disks and their partitions. Note that partition values can overlap with their parent disk's values, so simply adding all partition values does not necessarily equal the disk total.

---

# 10. Per-Process I/O with `iotop`

`iotop` provides continuously updated information about processes performing disk I/O.

```
iotop
```

Example fields:

```
TID  PRIO  USER  DISK READ  DISK WRITE  SWAPIN  IO>  COMMAND
```

Unlike most process monitoring tools, `iotop` uses a **TID** column rather than PID and can display threads.

### I/O priority

`PRIO` represents I/O scheduling priority.

Example:

```
be/3
be/4
```

The first component is the scheduling class and the second is the priority level.

Lower priority numbers receive more favorable scheduling within the relevant class.

### I/O scheduling classes

|Class|Meaning|
|---|---|
|`be`|Best effort; normal/fair I/O scheduling|
|`rt`|Real-time; scheduled before other I/O classes|
|`idle`|I/O only when no other I/O needs servicing|

I/O priority can be inspected/changed with:

```
ionice
```

---

# 11. Per-Process Monitoring with `pidstat`

`pidstat` monitors resource consumption over time without continuously overwriting previous observations like `top`.

Example:

```
pidstat -p 1329 1
```

This monitors PID `1329` every second.

Default output includes:

- `%usr` — user CPU time
- `%system` — system/kernel CPU time
- `%guest` — time spent running guest/VM code
- `%CPU` — overall CPU utilization
- `CPU` — CPU/core on which the process ran

### Memory and disk monitoring

```
pidstat -r -p PID 1
```

Monitor memory-related statistics.

```
pidstat -d -p PID 1
```

Monitor disk I/O.

`pidstat` also supports monitoring threads, context switching, and other resource statistics.

---

# Quick Troubleshooting Mental Model

For Linux resource issues, correlate multiple metrics rather than relying on one number:

```
              System slowdown
                    │
        ┌───────────┼───────────┐
        ↓           ↓           ↓
       CPU        Memory        I/O
        │           │           │
      top       free/meminfo   iostat
     uptime       vmstat       iotop
    pidstat      page faults
                    │
                 vmstat
                 si / so
                    │
                Thrashing?
```

Useful correlations:

- **High load + high CPU utilization** → CPU contention.
- **High load + low CPU utilization + high `wa`** → investigate I/O.
- **High load + low memory + increasing `si`/`so`** → memory pressure/swapping.
- **High major page faults** → investigate memory/storage pressure.
- **High disk activity from one process** → use `iotop`/`pidstat -d`.
- **Specific process consuming CPU** → `top -p PID` / `pidstat -p PID`.