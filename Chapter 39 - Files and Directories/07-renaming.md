### Key Notes on Renaming Files and Persistent Memory

#### **Renaming Files**

1. **Command Line Renaming**:
    
    - Use the `mv` command to rename files.
    - Example: `mv foo bar` renames the file `foo` to `bar`.
2. **System Call for Renaming**:
    
    - The `rename(char *old, char *new)` system call is used for renaming files programmatically.
    - It takes two arguments:
        - `old`: Original file name.
        - `new`: New file name.
3. **Atomicity of `rename()`**:
    
    - The `rename()` system call is atomic with respect to system crashes.
    - If a crash occurs during renaming, the file will either retain the old name or the new name, avoiding any inconsistent state.
4. **Use Case: File Editor Example**:
    
    - To ensure atomic updates, a file editor (e.g., `emacs`) might:
        1. Write the new file contents to a temporary file (`foo.txt.tmp`).
        2. Use `fsync()` to ensure the temporary file is written to disk.
        3. Rename the temporary file to the original file name (`foo.txt`) using `rename()`.
    - Code Example:
        
        ```c
        int fd = open("foo.txt.tmp", O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
        write(fd, buffer, size); // Write new version of file
        fsync(fd);
        close(fd);
        rename("foo.txt.tmp", "foo.txt");
        ```
        

---

#### **Memory Mapping and Persistent Memory**

1. **Memory Mapping with `mmap()`**:
    
    - The `mmap()` system call maps a file's byte offsets to virtual memory addresses.
    - This allows the file to be accessed using CPU instructions (loads and stores) as if it were in memory.
2. **Persistent Memory**:
    
    - Combines file persistence with memory access semantics.
    - Simplifies applications by eliminating the need to translate between memory and storage data formats.
3. **Code Example for `mmap()`**:
    
    ```c
    p = mmap(NULL, file_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    assert(p != MAP_FAILED);
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "pop") == 0) { // pop
            if (p->n > 0) // stack not empty
                printf("%d\n", p->stack[--p->n]);
        } else { // push
            if (sizeof(pstack_t) + (1 + p->n) * sizeof(int) <= file_size) // stack not full
                p->stack[p->n++] = atoi(argv[i]);
        }
    }
    ```
    
4. **Persistent Stack Example**:
    
    - A program (`pstack.c`) stores a persistent stack in a file (`ps.img`).
    - The file contains:
        - A count of the stack size (`p->n`).
        - An array of integers representing the stack contents (`p->stack`).
    - Data pushed in one program invocation can be popped in the next.
5. **Crash Consistency**:
    
    - Crashes during operations (e.g., between increment and assignment in a push operation) can leave the stack in an inconsistent state.
    - Applications use mechanisms to ensure atomic updates to persistent memory to prevent such issues.