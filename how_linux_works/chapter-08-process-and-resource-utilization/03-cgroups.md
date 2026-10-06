# 8.6 Control Groups (cgroups)

## Overview

**Control groups (cgroups)** are a Linux kernel feature used to **manage and limit resource consumption for groups of processes**.

Instead of controlling individual processes, cgroups allow multiple processes to be managed together.

Common resources that can be controlled include:

- CPU time
- Memory
- Number of processes/threads
- I/O and other resources through controllers

For example, a cgroup can limit the **total memory consumption of several processes combined**.

> **Important:** `systemd` makes extensive use of cgroups, but cgroups are a **kernel feature** and do not depend on systemd.

---

## cgroups v1 vs v2

Linux currently has two cgroup versions:

### cgroups v1

- Each controller has its **own cgroup hierarchy**.
- A process can belong to **one cgroup per controller**.
- Therefore, a process can simultaneously belong to multiple cgroups.

Example:

```
Process
 ├── CPU cgroup
 └── Memory cgroup
```

This allows independent CPU and memory group assignments.

### cgroups v2

- A process belongs to **only one cgroup**.
- Multiple controllers can be configured for that single cgroup.

Example:

```
cgroup A
 ├── CPU controller
 └── Memory controller
```

This simplifies the hierarchy compared with v1.

### Key difference

|Feature|cgroups v1|cgroups v2|
|---|---|---|
|Process membership|One cgroup per controller|One cgroup total|
|Controllers|Separate hierarchies|Multiple controllers per cgroup|
|Structure|More complex|Simpler|
|Current direction|Being phased out|Focus of modern Linux|

The source focuses on **cgroups v2** because v1 is being phased out.

---

# Identifying a Process's cgroups

Every process has a:

```
/proc/<pid>/cgroup
```

file.

For the current shell/process:

```
cat /proc/self/cgroup
```

Example:

```
12:rdma:/
11:net_cls,net_prio:/
10:perf_event:/
9:cpuset:/
8:cpu,cpuacct:/user.slice
7:blkio:/user.slice
6:memory:/user.slice
5:pids:/user.slice/user-1000.slice/session-2.scope
...
0::/user.slice/user-1000.slice/session-2.scope
```

### Interpreting the output

- **Numbers 2–12** → cgroups v1 in this example.
- **Number 1** → v1 management cgroup without a controller.
- **Number 0** → cgroups v2.
- The controller names appear after the first `:`.
- The final field represents the cgroup's hierarchical path.

For example:

```
0::/user.slice/user-1000.slice/session-2.scope
```

means the process is in the cgroups v2 hierarchy at:

```
/user.slice/user-1000.slice/session-2.scope
```

---

# systemd and cgroup Hierarchy

On systems using systemd:

```
/user.slice/
```

contains user-related cgroups.

For example:

```
/user.slice/user-1000.slice/session-2.scope
```

represents a login session managed by systemd.

System services are generally located under:

```
/system.slice/
```

This is useful when investigating **which systemd service or user session owns a process**.

---

# cgroup Filesystem

Unlike traditional Unix interfaces, cgroups are accessed through a **filesystem interface**.

For cgroups v2, the filesystem is normally mounted at:

```
/sys/fs/cgroup
```

If cgroups v1 is also present, the unified v2 hierarchy may instead be under:

```
/sys/fs/cgroup/unified
```

You can correlate `/proc/self/cgroup` with the corresponding directory:

```
cat /proc/self/cgroup
```

Example:

```
0::/user.slice/user-1000.slice/session-2.scope
```

Then:

```
cd /sys/fs/cgroup/user.slice/user-1000.slice/session-2.scope/
ls
```

---

# Important cgroup Interface Files

Cgroup directories contain files that act as the interface for managing and inspecting the cgroup.

## `cgroup.procs`

Lists the **process IDs** belonging to the cgroup.

```
cat cgroup.procs
```

## `cgroup.threads`

Similar to `cgroup.procs`, but also includes **threads**.

```
cat cgroup.threads
```

## `cgroup.controllers`

Shows the controllers currently available/used for the cgroup.

```
cat cgroup.controllers
```

Example:

```
memory pids
```

This means the cgroup has the **memory** and **pids** controllers available.

---

# Controllers

Controllers implement resource-management functionality.

Examples:

|Controller|Purpose|
|---|---|
|`cpu`|CPU resource management|
|`memory`|Memory management/limits|
|`pids`|Limit/manage number of processes|
|`io`|I/O resource management|

The exact controllers available depend on the system and hierarchy.

---

# Inspecting Resource Limits

Controller-specific files use the controller name as a prefix.

### Process count

```
cat pids.current
```

Example:

```
4
```

This shows the current number of processes/threads represented by the `pids` controller.

### Memory limit

```
cat memory.max
```

Example:

```
max
```

`max` means the cgroup has **no specific memory limit** at that level.

However, because cgroups are hierarchical, a parent/ancestor cgroup can still impose a restriction.

---

# Moving Processes Between cgroups

A process can be placed into a cgroup by writing its PID to `cgroup.procs`.

As root:

```
echo <PID> > cgroup.procs
```

For example:

```
echo 1234 > cgroup.procs
```

This moves PID `1234` into the corresponding cgroup.

---

# Setting Resource Limits

For example, to limit a cgroup to **3,000 PIDs**:

```
echo 3000 > pids.max
```

This demonstrates an important cgroup concept:

> **Many cgroup settings are controlled by writing values directly into cgroup interface files.**

---

# Creating a cgroup

Creating a cgroup can be as simple as creating a directory inside the cgroup hierarchy:

```
mkdir my-cgroup
```

The kernel automatically creates the appropriate cgroup interface files.

If the cgroup contains no processes, it can be removed with:

```
rmdir my-cgroup
```

However, there are important hierarchy rules.

### cgroup hierarchy rules

1. Processes can normally only be placed into **leaf cgroups**.
2. A child cannot use a controller that isn't available in its parent.
3. Controllers must explicitly be enabled for child cgroups through `cgroup.subtree_control`.

For example:

```
echo +cpu +pids > cgroup.subtree_control
```

enables the CPU and PIDs controllers for child cgroups.

### Root cgroup

The root cgroup is an exception to some of these rules.

Processes can be placed directly in the root cgroup. One possible use is detaching a process from systemd's control.

---

# Viewing Resource Utilization

cgroups can also be used for **resource accounting**, not just limiting.

## CPU

```
cat cpu.stat
```

Example:

```
usage_usec 4617481
user_usec 2170266
system_usec 2447215
```

These values represent accumulated CPU usage over the **entire lifetime of the cgroup**.

This is particularly useful for services that spawn multiple subprocesses:

```
Service
 ├── Process A
 ├── Process B
 ├── Process C
 └── Process D
```

Even if some subprocesses terminate, their CPU usage remains reflected in the cgroup's accumulated statistics.

---

## Memory

When the memory controller is enabled:

```
cat memory.current
```

shows current memory consumption.

```
cat memory.stat
```

provides more detailed memory statistics for the cgroup.

---

# Threat Hunting / Process Analysis Relevance

For process investigation, cgroups provide another way to understand **process ownership and resource context**.

A useful investigation flow is:

```
PID
 │
 ├── /proc/<PID>/cgroup
 │
 └── cgroup hierarchy
       │
       ├── system.slice
       ├── user.slice
       └── session/application scope
```

This can help correlate a process with:

- A systemd service
- A user session
- An application scope
- Resource limits
- Other processes managed within the same cgroup

The source also notes that understanding cgroups becomes important later when studying **containers**, where cgroups are used for a different purpose.