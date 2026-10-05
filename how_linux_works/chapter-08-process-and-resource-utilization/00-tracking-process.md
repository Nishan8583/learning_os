# Linux Process Tracking & Tracing

## 1. Tracking Processes

### `ps` vs `top`

`ps` provides a **snapshot** of processes and their resource usage at a particular point in time. It is less useful for determining how resource consumption changes over time.

`top` provides an **interactive, continuously updating** view of process activity. By default, it updates every second and places the most CPU-active processes near the top.

### Common `top` keystrokes

|Key|Function|
|---|---|
|`Space`|Immediately refresh the display|
|`M`|Sort by current resident memory usage|
|`T`|Sort by cumulative CPU usage|
|`P`|Sort by current CPU usage — default|
|`u`|Show processes belonging to a specific user|
|`f`|Select which statistics/fields are displayed|
|`?`|Display help/summary of `top` commands|

**Important:** `top` commands are **case-sensitive**.

### Alternatives

- `atop` — enhanced process/system monitoring.
- `htop` — enhanced interactive process viewer.
- `htop` provides functionality overlapping with other tools, including some capabilities associated with `lsof`.

---

# 2. Finding Open Files with `lsof`

`lsof` (**list open files**) shows:

- Open files
- Processes using those files
- Network resources
- Dynamic libraries
- Pipes
- Other file-like resources

Because Unix exposes many resources through the file abstraction, `lsof` is useful for identifying process/resource problems.

## `lsof` output

Example:

```
COMMAND  PID   USER   FD    TYPE    DEVICE  SIZE/OFF    NODE NAME
systemd    1   root  cwd     DIR    8,1      4096          2 /
systemd    1   root  rtd     DIR    8,1      4096          2 /
systemd    1   root  txt     REG    8,1   1595792    9961784 /lib/systemd/systemd
vi      1994   juser   3u    REG    8,1     12288     786440 /tmp/.ff.swp
```

### Important fields

|Field|Meaning|
|---|---|
|`COMMAND`|Process/command holding the file descriptor|
|`PID`|Process ID|
|`USER`|User running the process|
|`FD`|File descriptor or purpose of the open resource|
|`TYPE`|Resource type, e.g. regular file, directory, socket|
|`DEVICE`|Major/minor device number|
|`SIZE/OFF`|File size or offset|
|`NODE`|Inode number|
|`NAME`|Filename/resource name|

### Useful `FD` values

`FD` can describe either the purpose of the resource or its numeric file descriptor.

For example:

- `cwd` → process's current working directory
- `rtd` → root directory
- `txt` → executable text/image
- `mem` → memory-mapped file/library
- `3u` → file descriptor `3`, opened for read/write

The source specifically demonstrates `cwd` entries as the process's current working directories.

### Permissions

`lsof` can be executed as a normal user or root, but **root generally provides more information**.

---

# 3. Narrowing `lsof` Output

`lsof` can generate a very large amount of output. Two approaches are:

1. List everything and pipe through tools such as `less`.
2. Narrow the output using `lsof` options.

### Search a directory tree

```
lsof +D /usr
```

Shows open files under `/usr` and its subdirectories.

### Inspect a specific process

```
lsof -p <PID>
```

Lists open files/resources associated with the specified process.

### Display help

```
lsof -h
```

Shows a summary of `lsof` options.

**Note:** `lsof` relies heavily on kernel information. After updating both the kernel and `lsof`, the updated version may not work correctly until rebooting into the new kernel.

---

# 4. Tracing Program Execution

Process-monitoring tools such as `ps`, `top`, and `lsof` primarily examine **active processes/resources**.

For programs that:

- Start and immediately terminate
- Fail before you can inspect them
- Perform unexpected operations
- Produce unclear errors

use:

- `strace` → traces **system calls**
- `ltrace` → traces **shared library calls**

Both can generate very large amounts of output, so filtering is important.

---

# 5. `strace`

`strace` traces the **system calls** made by a process.

A system call is a privileged operation where a userspace process requests an operation from the kernel, such as opening or reading a file.

### Basic usage

```
strace cat /dev/null
```

This traces the system calls made while executing `cat /dev/null`.

### Save output

By default, `strace` writes its output to **stderr**.

Use:

```
strace -o trace.log cat /dev/null
```

to save the trace to a file.

Alternatively:

```
strace cat /dev/null 2> trace.log
```

but this also captures the program's normal stderr output.

---

## Understanding `strace` process startup

When a process starts another program, the typical sequence involves:

```
fork()
  ↓
exec()
```

`strace` begins tracing the new process after the `fork()` operation, so startup output commonly includes an `execve()` call followed by memory initialization such as `brk()`.

Example:

```
execve("/bin/cat", ["cat", "/dev/null"], ...) = 0
brk(NULL) = 0x561e83127000
```

The `execve()` return value of `0` indicates successful execution.

---

# 6. Reading System Calls in `strace`

A typical file-access sequence might look like:

```
openat(..., "/dev/null", O_RDONLY|O_CLOEXEC) = 3
fstat(3, ...) = 0
read(3, "", 131072) = 0
close(3) = 0
```

Conceptually:

```
open file
   ↓
get file information
   ↓
read from file
   ↓
close file
```

The value returned by `openat()` (`3` in this example) is the **file descriptor** assigned by the kernel. Subsequent calls use that descriptor to operate on the file.

Common calls worth recognizing:

|System call|What it indicates|
|---|---|
|`execve()`|Execute a program|
|`openat()`|Open a file|
|`read()`|Read data|
|`write()`|Write data|
|`close()`|Close a file descriptor|
|`mmap()`|Map memory/file into process address space|
|`munmap()`|Remove a memory mapping|
|`fstat()`|Retrieve file metadata|
|`brk()`|Modify process heap boundary|
|`exit_group()`|Terminate the process|

---

# 7. Using `strace` for Errors

`strace` is particularly useful when a program fails because of filesystem or system-call errors.

Example:

```
strace cat not_a_file
```

Relevant output:

```
openat(AT_FDCWD, "not_a_file", O_RDONLY) = -1 ENOENT
```

Interpretation:

```
-1       → system call failed
ENOENT   → file/directory does not exist
```

`strace` reports both the failure code and a short description of the error.

This makes it useful when normal application logs don't explain **which filesystem operation failed**.

---

# 8. Tracing Forking/Daemon Processes

For programs that spawn children or detach themselves, use:

```
strace -o crummyd_strace -ff crummyd
```

`-ff` causes tracing output for child processes to be written separately.

The resulting files use the PID of each child, e.g.:

```
crummyd_strace.<pid>
```

This is particularly useful when the process you're interested in isn't the original process you launched.

---

# 9. `ltrace`

`ltrace` traces **shared library calls** rather than kernel system calls.

Conceptually:

```
Application
    │
    ├── ltrace → library calls
    │
    └── strace → system calls → kernel
```

`ltrace` output resembles `strace`, but it operates at the library-call level rather than the kernel level.

### Important limitation

`ltrace` does **not work with statically linked binaries** because they don't depend on external shared libraries in the same way.

---

# Quick Troubleshooting Workflow

```
High CPU/memory?
      ↓
     top
      ↓
Identify PID
      ↓
     lsof
      ↓
What files/resources does it use?
      ↓
Program starts then immediately fails?
      ↓
    strace
      ↓
Which system call fails?
      ↓
Need library-level behavior?
      ↓
    ltrace
```**