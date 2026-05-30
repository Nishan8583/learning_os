### Key Notes on "Reading and Writing, But Not Sequentially"

#### Overview

- File access can be sequential (reading/writing from start to end) or non-sequential (reading/writing at specific offsets).
- Non-sequential access is achieved using the `lseek()` system call.

#### `lseek()` System Call

- **Function Prototype**: `off_t lseek(int fildes, off_t offset, int whence);`
    - `fildes`: File descriptor.
    - `offset`: Position within the file.
    - `whence`: Determines how the offset is calculated:
        - `SEEK_SET`: Offset is set to `offset` bytes from the beginning of the file.
        - `SEEK_CUR`: Offset is set to the current location plus `offset` bytes.
        - `SEEK_END`: Offset is set to the size of the file plus `offset` bytes.

#### Key Concepts

1. **Current Offset**:
    
    - Each open file has a "current offset" tracked by the OS.
    - The offset determines where the next read or write will begin.
    - The offset is updated:
        - Implicitly: After a read/write operation, the offset is incremented by the number of bytes read/written.
        - Explicitly: Using `lseek()`.
2. **File Structure**:
    
    - The OS maintains a structure for each open file:
        
        ```c
        struct file {
            int ref;              // Reference count
            char readable;        // Readable flag
            char writable;        // Writable flag
            struct inode *ip;     // Pointer to the underlying file
            uint off;             // Current offset
        };
        ```
        
    - The structure tracks whether the file is readable/writable, the file's inode, and the current offset.
3. **Open File Table (OFT)**:
    
    - Represents all currently opened files in the system.
    - Managed as an array in the kernel:
        
        ```c
        struct {
            struct spinlock lock;
            struct file file[NFILE];
        } ftable;
        ```
        
4. **`lseek()` vs Disk Seek**:
    
    - `lseek()` **does not perform a disk seek**. It only updates the current offset in OS memory.
    - However, using `lseek()` to access random parts of a file may lead to disk seeks during subsequent read/write operations.

#### Examples

1. **Sequential Reading**:
    
    - A process opens a file and reads it sequentially in chunks of 100 bytes: 
    
| System Calls | Return Code | Current Offset | 
| :------- | :------: | -------: |
| `fd = open("file", O_RDONLY);` | 3 | 0 | 
| `read(fd, buffer, 100);` | 100 | 100 | 
| `read(fd, buffer, 100);` | 100 | 200 | 
| `read(fd, buffer, 100);` | 100 | 300 |
| `read(fd, buffer, 100);` | 0 | 300 | 
| `close(fd);` | 0 | – |
        
    - Notes:
        
        - The offset starts at 0 when the file is opened.
        - Each `read()` increments the offset by the number of bytes read.
        - Reading past the end of the file returns 0.
2. **Multiple File Descriptors**:
    
    - A process opens the same file twice and reads from each descriptor independently: 
    
| System Calls                      | Return Code | Current Offset (OFT[10]) | Current Offset (OFT[11]) |
| :-------------------------------- | :---------: | -----------------------: | ------------------------ |
|  `fd1 = open("file", O_RDONLY);`  |      3      |                        0 | –                        |
|  `fd2 = open("file", O_RDONLY);`  |      4      |                        0 | 0                        |
|  `read(fd1, buffer1, 100);`       |     100     |                      100 | 0                        |
|  `read(fd2, buffer2, 100);`       |     100     |                      100 | 100                      |
|  `close(fd1);`                    |      0      |                        – | 100                      |
|  `close(fd2);`                    |      0      |                        – | –                        |
        
    - Notes:
        
        - Each file descriptor has its own entry in the open file table.
        - The current offset is updated independently for each descriptor.
3. **Using `lseek()`**:
    
    - A process uses `lseek()` to reposition the current offset before reading: 
| System Calls | Return Code | Current Offset | 
| :------- | :------: | -------: |
| `fd = open("file", O_RDONLY);` | 3 | 0 | 
| `lseek(fd, 200, SEEK_SET);` | 200 | 200 |
| `read(fd, buffer, 50);` | 50 | 250 | 
| `close(fd);` | 0 | – |
        
    - Notes:
        
        - `lseek()` explicitly sets the current offset to 200.
        - The subsequent `read()` starts from the new offset and updates it accordingly.