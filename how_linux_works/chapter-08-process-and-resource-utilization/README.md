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