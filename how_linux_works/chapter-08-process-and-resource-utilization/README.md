## Command Reference

|Command|Meaning|
|---|---|
|`top`|Interactive, continuously updating process/resource monitor|
|`atop`|Enhanced system/process monitoring utility|
|`htop`|Enhanced interactive process viewer|
|`lsof`|List open files/resources and the processes using them|
|`lsof +D /usr`|Show open files under `/usr` and its subdirectories|
|`lsof -p <PID>`|Show resources/files opened by a specific process|
|`lsof -h`|Display `lsof` help/options|
|`strace <command>`|Trace system calls made by a command|
|`strace -o <file> <command>`|Save `strace` output to a file|
|`strace <command> 2> <file>`|Redirect `strace` stderr output to a file|
|`strace -ff <command>`|Follow child processes and trace them separately|
|`strace -o <file> -ff <command>`|Save traces for the process and children to PID-specific files|
|`ltrace <command>`|Trace shared-library calls made by a command|

---

# Process vs. Thread — Quick View

|Concept|Process|Thread|
|---|---|---|
|Identifier|PID|TID|
|Scheduling|Scheduled by kernel|Scheduled by kernel|
|Memory|Generally isolated from other processes|Shares memory within same process|
|Resources|Generally separate|Shares process resources|
|Creation|`fork()` commonly used|Main thread can create additional threads|
|Main thread|N/A|Usually has TID equal to PID|
|Parallel execution|Possible|Multiple threads can execute simultaneously|
|`ps` default|Shows process|Doesn't normally show individual threads|
|`ps m`|Shows process + threads|Displays associated threads|
|`top` default|Shows processes|Threads hidden|
|`top` `H`|—|Enables thread display|

## Threads Command / Key Reference

|Command / Key|Meaning|
|---|---|
|`ps`|Display processes|
|`ps m`|Display processes together with their threads|
|`ps m -o pid,tid,command`|Display PID, TID, and command information|
|`top`|Interactive process/resource monitor; threads hidden by default|
|`H` _(inside `top`)_|Toggle/display individual threads|
|`fork()`|Creates a new process; mentioned as a comparison to creating threads|

# Resource Monitoring Command Reference

| Command                         | Meaning / Use                                                                      |
| ------------------------------- | ---------------------------------------------------------------------------------- |
| `top`                           | Interactive real-time process/resource monitoring                                  |
| `top -p PID`                    | Monitor a specific process                                                         |
| `time command`                  | Measure command elapsed, user, and system CPU time                                 |
| `/usr/bin/time command`         | Detailed execution statistics, including page faults                               |
| `ps -l`                         | Display process priority information                                               |
| `renice 20 PID`                 | Change a process's nice value                                                      |
| `uptime`                        | Show system uptime and 1/5/15-minute load averages                                 |
| `free`                          | Display system memory and swap usage                                               |
| `cat /proc/meminfo`             | View detailed kernel memory statistics                                             |
| `getconf PAGE_SIZE`             | Display system memory page size in bytes                                           |
| `ps -o pid,min_flt,maj_flt PID` | Display minor/major page faults for a process                                      |
| `vmstat 2`                      | Continuously monitor processes, memory, swap, I/O, system, and CPU every 2 seconds |
| `vmstat -d`                     | Display detailed per-device I/O statistics                                         |
| `iostat`                        | Display CPU and device I/O statistics                                              |
| `iostat 2`                      | Continuously update I/O statistics every 2 seconds                                 |
| `iostat -d 2`                   | Continuously display device I/O statistics                                         |
| `iostat -p ALL`                 | Display disk and partition statistics                                              |
| `iotop`                         | Monitor disk I/O by process/thread                                                 |
| `ionice`                        | View/change I/O scheduling priority                                                |
| `pidstat -p PID 1`              | Monitor a process's CPU usage every second                                         |
| `pidstat -r -p PID 1`           | Monitor process memory statistics                                                  |
| `pidstat -d -p PID 1`           | Monitor process disk I/O                                                           |
| `top` → `f` → `nMaj`            | Add total major page faults to `top`                                               |
| `top` → `f` → `vMj`             | Display major page faults since the last update                                    |

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


## Important io stat device statistics:

|Field|Meaning|
|---|---|
|`tps`|Average transfers per second|
|`kB_read/s`|KB read per second|
|`kB_wrtn/s`|KB written per second|
|`kB_read`|Total KB read|
|`kB_wrtn`|Total KB written|


# Cgroups Quick Reference

|Command|Meaning|
|---|---|
|`cat /proc/self/cgroup`|Show the current process's cgroup memberships|
|`cat /proc/<PID>/cgroup`|Show cgroup memberships for a specific PID|
|`cd /sys/fs/cgroup/...`|Navigate to a cgroup's filesystem representation|
|`ls`|List cgroup interface files|
|`cat cgroup.procs`|Show PIDs belonging to the cgroup|
|`cat cgroup.threads`|Show threads belonging to the cgroup|
|`cat cgroup.controllers`|Show controllers available for the cgroup|
|`cat pids.current`|Show current process count tracked by the PIDs controller|
|`cat pids.max`|Show the maximum PID/process limit|
|`echo 3000 > pids.max`|Set the PID limit to 3,000|
|`cat memory.max`|Show the cgroup's memory limit|
|`cat memory.current`|Show current memory usage|
|`cat memory.stat`|Show detailed memory statistics|
|`cat cpu.stat`|Show accumulated CPU usage statistics|
|`echo <PID> > cgroup.procs`|Move a process into the cgroup|
|`echo +cpu +pids > cgroup.subtree_control`|Enable CPU and PIDs controllers for child cgroups|
|`mkdir my-cgroup`|Create a cgroup directory|
|`rmdir my-cgroup`|Remove an empty cgroup|


## Htop Columns

|Column|Meaning|
|---|---|
|**PID**|Process ID — unique identifier for the process|
|**USER**|User who owns the process|
|**PRI**|Kernel scheduling priority of the process|
|**NI**|Nice value — user-space adjustment to scheduling priority|
|**VIRT**|Total virtual memory/address space associated with the process|
|**RES**|Resident memory — physical RAM currently resident for the process|
|**SHR**|Shared resident memory — memory potentially shared with other processes|
|**S**|Process state (`R` running, `S` sleeping, `D` uninterruptible sleep, `T` stopped, `Z` zombie, etc.)|
|**CPU%**|Percentage of CPU capacity currently being consumed|
|**MEM%**|Percentage of physical RAM used by the process|
|**TIME+**|Total accumulated CPU time consumed by the process|
|**Command**|Command/executable used to start the process|
|**PPID**|Parent Process ID — PID of the process that created this process|
|**PGRP**|Process Group ID|
|**SID**|Session ID|
|**TGID**|Thread Group ID — usually the PID of the main thread/process|
|**TID**|Thread ID|
|**NLWP**|Number of lightweight processes/threads belonging to the process|
|**ELAPSED**|Time elapsed since the process started|
|**STARTTIME**|Time/date when the process started|
|**STATE**|More detailed process-state information|
|**Processor**|CPU/core on which the process is currently executing|
|**CPU Affinity**|CPUs on which the process is allowed to run|
|**Command Line**|Full command line, including arguments|
|**EXE**|Executable associated with the process|
|**CWD**|Current working directory of the process|
|**IO_R**|Bytes read by the process|
|**IO_W**|Bytes written by the process|
|**IO_RATE**|Current I/O rate|
|**M_RESIDENT**|Resident memory|
|**M_SHARE**|Shared memory|
|**M_TRS**|Text/code resident memory|
|**M_DRS**|Data resident memory|
|**M_LRS**|Library resident memory|
|**M_DT**|Dirty pages|
|**SWAP**|Amount of the process's memory currently swapped out|
|**M_SIZE**|Total process memory size|
|**M_PSS**|Proportional Set Size — resident memory accounting that divides shared pages among processes using them|
|**OOM**|Out-of-memory killer score/adjustment|
|**OOMADJ**|Adjustment to the OOM-killer score|
|**RCHAR**|Bytes read through system calls such as `read()`|
|**WCHAR**|Bytes written through system calls such as `write()`|
|**RBYTES**|Actual bytes read from storage|
|**WBYTES**|Actual bytes written to storage|
|**SYSCR**|Number of read-related system calls|
|**SYSCW**|Number of write-related system calls|
|**UTIME**|CPU time spent executing user-space code|
|**STIME**|CPU time spent executing kernel/system code|

### The ones I'd memorize for Linux/threat hunting

```
PID       → Who is the process?
PPID      → Who spawned it?
USER      → Who owns it?
S         → What state is it in?
CPU%      → Is it consuming CPU?
MEM%      → How much RAM?
VIRT      → How large is its virtual address space?
RES       → How much RAM is resident?
SHR       → How much resident memory is potentially shared?
SWAP      → How much has been swapped?
TIME+     → How much CPU time has it accumulated?
Command   → What was executed?
```