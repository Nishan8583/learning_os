### Key Notes on Recovery and Batching Log Updates

#### **Recovery in File Systems**

1. **Crash Scenarios and Recovery Process**:
    
    - If a crash occurs **before the transaction is written to the log** (Step 2), the pending update is skipped.
    - If a crash occurs **after the transaction is committed to the log but before the checkpoint is complete**, the file system recovers by:
        - Scanning the log during system boot.
        - Identifying transactions committed to the disk.
        - Replaying these transactions in order to write blocks to their final on-disk locations.
2. **Redo Logging**:
    
    - This recovery method is called **redo logging**.
    - It ensures on-disk structures are consistent by recovering committed transactions from the journal.
    - The file system can then mount and handle new requests.
3. **Crash During Checkpointing**:
    
    - Crashes during checkpointing are not critical.
    - Some updates may be redundantly performed during recovery, but this is acceptable as recovery is a rare operation.

---

#### **Batching Log Updates**

1. **Problem with Excessive Disk Traffic**:
    
    - Without optimization, journaling can lead to redundant writes.
    - Example: Creating two files (`file1` and `file2`) in the same directory requires multiple updates:
        - Update the **inode bitmap** to allocate new inodes.
        - Update the **newly-created inodes** of the files.
        - Update the **data block** of the parent directory to include the new directory entries.
        - Update the **parent directory inode** (e.g., modification time).
    - Without batching, these updates are written to the journal separately, causing repeated writes for the same blocks.
2. **Solution: Buffering Updates into a Global Transaction**:
    
    - Instead of committing each update individually, some file systems (e.g., Linux ext3) **buffer updates** into a global transaction.
    - Example:
        - When creating two files, the file system marks the **inode bitmap**, **inodes of the files**, **directory data**, and **directory inode** as dirty.
        - These updates are added to a list of blocks forming the current transaction.
    - After a timeout (e.g., 5 seconds), the global transaction is committed, containing all updates in a single write.
3. **Benefits of Batching**:
    
    - Reduces excessive write traffic to the disk.
    - Improves performance by consolidating multiple updates into a single transaction.

---

### Code Example: Redo Logging and Batching

```c
// Pseudo-code for redo logging and batching updates

// Recovery process during system boot
void recoverFileSystem() {
    scanLog(); // Scan the log for committed transactions
    for (Transaction txn : committedTransactions) {
        replayTransaction(txn); // Replay transactions in order
    }
    mountFileSystem(); // Mount the file system after recovery
}

// Batching updates into a global transaction
void createFile(char* fileName, Directory* parentDir) {
    markInodeBitmapDirty(); // Mark inode bitmap as dirty
    markInodeDirty(fileName); // Mark the new file's inode as dirty
    markDirectoryDataDirty(parentDir); // Mark parent directory data as dirty
    markDirectoryInodeDirty(parentDir); // Mark parent directory inode as dirty
    addToGlobalTransaction(); // Add all updates to the global transaction
}

// Commit global transaction after timeout
void commitGlobalTransaction() {
    if (timeoutReached()) {
        writeTransactionToLog(globalTransaction); // Write all updates to the log
        checkpointTransaction(globalTransaction); // Write updates to final on-disk locations
    }
}
```

These notes and code examples summarize the key concepts of recovery and batching log updates in file systems for quick review.