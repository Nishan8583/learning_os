### Key Notes on The File System Interface

#### Basics of File System Interface

- The file system interface includes operations for creating, accessing, and deleting files.
- A key system call for deleting files is `unlink()`, which removes a file.

---

#### Creating Files

- **System Call**: `open()` is used to create files with the `O_CREAT` flag.
- **Example Code**:
    
    ```c
    int fd = open("foo", O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR|S_IWUSR);
    ```
    
    - **Flags**:
        - `O_CREAT`: Creates the file if it does not exist.
        - `O_WRONLY`: Opens the file in write-only mode.
        - `O_TRUNC`: Truncates the file to zero bytes if it already exists.
    - **Permissions**:
        - `S_IRUSR`: Read permission for the owner.
        - `S_IWUSR`: Write permission for the owner.

---

#### The `creat()` System Call

- **Legacy Method**: `creat()` is an older way to create files.
- **Example Code**:
    
    ```c
    int fd = creat("foo");
    ```
    
    - Equivalent to calling `open()` with `O_CREAT | O_WRONLY | O_TRUNC`.
- **Trivia**: Ken Thompson, one of UNIX's creators, mentioned he would have spelled `creat` with an "e" if redesigning UNIX.

---

#### File Descriptors

- **Definition**: A file descriptor is an integer, private to each process, used to access files.
- **Purpose**:
    - Acts as a capability (an opaque handle) to perform operations like `read()` and `write()`.
    - Functions as a pointer to a `file` object.
- **Management**:
    - File descriptors are managed by the OS on a per-process basis.
    - Example from the `xv6` kernel:
        
        ```c
        struct proc {
          ...
          struct file *ofile[NOFILE]; // Open files
          ...
        };
        ```
        
        - `ofile` is an array (size `NOFILE`) that tracks open files for each process.
        - Each entry in the array is a pointer to a `struct file`, which contains information about the file.

---

#### Useful Tool: `strace`

- **Purpose**: `strace` is a tool to trace system calls made by a program.
- **Capabilities**:
    - Shows system calls, arguments, and return codes.
    - Helps understand program behavior.
- **Common Flags**:
    - `-f`: Follows forked child processes.
    - `-t`: Reports the time of day for each call.
    - `-e trace=open,close,read,write`: Traces only specific system calls (e.g., `open`, `close`, `read`, `write`).
- **Tip**: Refer to the `man` pages for more options and usage.