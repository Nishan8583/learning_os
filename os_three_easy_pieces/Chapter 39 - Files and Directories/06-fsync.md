### Key Notes on Writing Immediately With `fsync()`

#### 1. **Default Behavior of `write()`**

- When a program calls `write()`, it **does not immediately write data to persistent storage**.
- Instead, the file system buffers the writes in memory for a period (e.g., 5 or 30 seconds) before issuing them to the storage device.
- This approach improves performance but introduces a risk of **data loss** if the system crashes before the buffered data is written to disk.

#### 2. **Need for Immediate Persistence**

- Some applications, such as **database management systems (DBMS)**, require guarantees that data is written to disk immediately to ensure **correct recovery protocols**.
- For such cases, the **eventual guarantee** provided by `write()` is insufficient.

#### 3. **`fsync()` System Call**

- The `fsync(int fd)` system call forces all **dirty (not yet written)** data associated with a file descriptor to be written to disk.
- The function **returns only after all writes are complete**, ensuring data persistence.
- **Use Case**: Applications that require immediate data durability, such as databases or critical logging systems.

#### 4. **Example Code Using `fsync()`**

```c
int fd = open("foo", O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR|S_IWUSR);
assert(fd > -1);
int rc = write(fd, buffer, size);
assert(rc == size);
rc = fsync(fd);
assert(rc == 0);
```

- **Explanation**:
    - Opens the file `foo` for writing (creates it if it doesn’t exist, truncates it if it does).
    - Writes a chunk of data (`buffer`) to the file.
    - Calls `fsync()` to ensure the data is immediately written to disk.
    - After `fsync()` returns, the application can safely assume the data is persisted.

#### 5. **Additional Step: `fsync()` on Directory**

- In some cases, calling `fsync()` on the file itself is **not sufficient**.
- If the file is **newly created**, you must also call `fsync()` on the **directory** containing the file to ensure:
    - The file itself is durably part of the directory.
    - This step prevents potential bugs where the file metadata is not properly persisted.

#### 6. **Practical Insights**

- While `fsync()` ensures data durability, it can **impact performance** due to the immediate write to disk.
- Developers must carefully balance the need for durability with performance considerations.
- Omitting the `fsync()` call for the containing directory can lead to **application-level bugs**, especially in systems requiring high reliability.