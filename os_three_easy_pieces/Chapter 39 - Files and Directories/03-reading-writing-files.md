### Key Notes on Reading and Writing Files

#### Basics of Reading and Writing Files

- Files can be read or written using system calls like `read()` and `write()`.
- Example: Using the `cat` command to read and display the contents of a file.
    
    ```bash
    prompt> echo hello > foo
    prompt> cat foo
    hello
    ```
    

---

#### Using `strace` to Trace System Calls

- **Purpose**: `strace` traces system calls made by a program and displays them.
- **Example**: Tracing the `cat` command with `strace`:
    
    ```bash
    prompt> strace cat foo
    ...
    open("foo", O_RDONLY|O_LARGEFILE) = 3
    read(3, "hello\n", 4096) = 6
    write(1, "hello\n", 6) = 6
    hello
    read(3, "", 4096) = 0
    close(3) = 0
    ...
    prompt>
    ```
    
    - **Key Observations**:
        - `open("foo", O_RDONLY|O_LARGEFILE)`: Opens the file `foo` for reading (`O_RDONLY`) with a 64-bit offset (`O_LARGEFILE`). Returns file descriptor `3`.
        - `read(3, "hello\n", 4096)`: Reads 6 bytes (5 for "hello" and 1 for the newline character) from the file into a 4 KB buffer.
        - `write(1, "hello\n", 6)`: Writes the 6 bytes to file descriptor `1` (standard output), displaying "hello" on the screen.
        - `read(3, "", 4096)`: Attempts to read more but returns `0`, indicating the end of the file.
        - `close(3)`: Closes the file `foo`.

---

#### File Descriptors

- **Definition**: File descriptors are integers used to access files in UNIX systems.
- **Standard File Descriptors**:
    - `0`: Standard input (stdin).
    - `1`: Standard output (stdout).
    - `2`: Standard error (stderr).
- **Example**: The first file opened by a process (after the standard descriptors) is assigned file descriptor `3`.

---

#### Reading Files

- **Steps**:
    1. Open the file for reading using `open()` with the `O_RDONLY` flag.
    2. Use `read()` to read data from the file:
        - **Arguments**:
            - File descriptor: Identifies the file to read.
            - Buffer: Stores the data read from the file.
            - Buffer size: Specifies the maximum number of bytes to read.
        - **Return Value**: Number of bytes read (or `0` if the end of the file is reached).
    3. Close the file using `close()`.

---

#### Writing Files

- **Steps**:
    1. Open the file for writing using `open()` with appropriate flags (e.g., `O_WRONLY`).
    2. Use `write()` to write data to the file:
        - **Arguments**:
            - File descriptor: Identifies the file to write to.
            - Buffer: Contains the data to write.
            - Buffer size: Specifies the number of bytes to write.
    3. Close the file using `close()`.

---

#### Example: Writing a File

- Using the `dd` utility to write data from one file to another:
    
    ```bash
    dd if=foo of=bar
    ```
    

---

#### Data Structure: Open File Table

- **Definition**: A system-wide table that tracks open files.
- **Details**:
    - Each process maintains an array of file descriptors.
    - Each file descriptor refers to an entry in the open file table.
    - Open file table entries store:
        - The underlying file the descriptor refers to.
        - The current offset in the file.
        - File permissions (readable, writable, etc.).

---

#### Useful Tool: `strace`

- **Purpose**: Trace system calls made by a program.
- **Capabilities**:
    - Shows system calls, arguments, and return values.
    - Useful for debugging and understanding program behavior.
- **Common Flags**:
    - `-f`: Follows forked child processes.
    - `-t`: Reports the time of day for each call.
    - `-e trace=open,close,read,write`: Traces specific system calls (e.g., `open`, `close`, `read`, `write`).