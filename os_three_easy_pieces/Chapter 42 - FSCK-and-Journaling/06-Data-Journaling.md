### Key Notes on Data Journaling

1. **Definition and Overview**:
    
    - Data journaling is a mode available in the Linux ext3 file system.
    - It ensures data consistency by writing updates to a log (journal) before applying them to their final disk locations.
    - The journal contains a record of the transaction, which can be used for recovery in case of a crash.
```
| Super | Journal | Group 0 | Group 1 | . . . | Group N |
```
1. **Process of Data Journaling**:
    
    - **Step 1: Journal Write**:
        - Write the transaction to the journal, including:
            - `TxB` (Transaction Begin): Contains information about the update, such as the final addresses of the blocks and a transaction identifier (TID).
            - Data and metadata blocks (e.g., `I[v2]`, `B[v2]`, `Db`).
            - `TxE` (Transaction End): Marks the end of the transaction and contains the TID.
        - Example of a journal entry:
            
            ```
            Journal
            __________________________________
            | TxB | I[v2] | B[v2] | Db | TxE |
            __________________________________
            ```
            
    - **Step 2: Checkpoint**:
        - After the journal write is complete, the pending metadata and data updates are written to their final locations on the disk.
        - Example of checkpointing:
            - Write `I[v2]`, `B[v2]`, and `Db` to their final disk locations.
2. **Crash Recovery**:
    
    - If a crash occurs during the journal write, the system can use the journal to recover the file system.
    - However, if the journal write is incomplete or corrupted, recovery can lead to issues, such as copying invalid data (`??`) to the disk.
3. **Challenges with Journal Writes**:
    
    - Writing all blocks of a transaction at once is faster but can be unsafe due to disk scheduling.
    - Example of an unsafe journal state after a crash:
        
        ```
        Journal
        ----------------------------------------------
        | TxB id=1 | I[v2] |  B[v2] |  ?? | TxE id=1 | ----------->
        ----------------------------------------------
        ```
        
    - The transaction appears valid (matching `TxB` and `TxE`), but the data block (`Db`) is corrupted, which can lead to data loss or file system corruption.
4. **Safe Journal Write Protocol**:
    
    - To ensure safety, the file system writes the transaction in two steps:
        - **Step 1**: Write all blocks except the `TxE` block to the journal in a single operation.
            
            ```
            Journal
            | TxB id=1 | I[v2] | B[v2]| Db | -------->
            ```
            
        - **Step 2**: Write the `TxE` block to the journal as a separate operation.
            
            ```
            Journal
	         --------------------------------------------
            | TxB id=1 | I[v2] | B[v2] | Db  | TxE id=1 | ---------->
	         --------------------------------------------
            ```
            
    - This ensures that the transaction is only considered complete when the `TxE` block is safely written.
5. **Atomicity Guarantee**:
    
    - Disks guarantee that any 512-byte write will either fully succeed or fail, ensuring no partial writes.
    - The `TxE` block is written as a single 512-byte block to ensure atomicity.
6. **Final Protocol for File System Update**:
    
    - **Phase 1: Journal Write**:
        - Write the transaction contents (`TxB`, metadata, and data) to the journal and wait for completion.
    - **Phase 2: Journal Commit**:
        - Write the transaction commit block (`TxE`) to the journal and wait for completion. The transaction is now considered committed.
    - **Phase 3: Checkpoint**:
        - Write the metadata and data updates to their final on-disk locations.
7. **Advantages of Data Journaling**:
    
    - Provides a robust mechanism for crash recovery.
    - Ensures data consistency by maintaining a record of updates in the journal.
    - Reduces the risk of data corruption by following a safe write protocol.

These notes summarize the key concepts, processes, and challenges of data journaling in the Linux ext3 file system.

[](https://developers.fg.rbc.com/catalog/default/component/artifactory/docs/architecture/environments)[](https://rbcgithub.fg.rbc.com/pages/Innersource-Commons/helios-docs/docs/application_security/application_coding_security_requirement)


## Aside
### Key Notes for Quick Review

#### **Forcing Writes to Disk**

1. **Old Method for Write Ordering**:
    
    - Write A to disk.
    - Wait for disk to confirm completion.
    - Write B to disk.
2. **Challenges with Write Caches**:
    
    - Modern disks use write caches (write buffering or immediate reporting).
    - Disk reports write completion when data is in memory cache, not physically written to disk.
    - This can lead to out-of-order writes.
3. **Solutions**:
    
    - **Disable Write Buffering**: Ensures ordering but reduces performance.
    - **Write Barriers**: Explicit barriers ensure all writes before the barrier are completed before subsequent writes.
4. **Trust Issues with Disks**:
    
    - Some disk manufacturers ignore write-barrier requests to improve performance, risking incorrect operation.
    - Research highlights this issue ([C+13, R+11]).

---

#### **Optimizing Log Writes**

1. **Inefficiency in Log Writing**:
    
    - Writing a transaction to the log requires:
        - Writing the transaction-begin block.
        - Writing transaction contents.
        - Waiting for these writes to complete.
        - Writing the transaction-end block.
    - This process often incurs an extra disk rotation, reducing performance.
2. **Proposed Optimization**:
    
    - **Checksum in Begin and End Blocks**:
        - Include a checksum of the journal contents in both the transaction-begin and transaction-end blocks.
        - Write the entire transaction at once without waiting for intermediate writes.
        - During recovery, if the checksum mismatches, the system discards the incomplete transaction.
3. **Benefits**:
    
    - Faster common-case performance (no extra rotation).
    - Improved reliability (checksum protects journal reads).
4. **Adoption**:
    
    - This optimization was implemented in the Linux ext4 file system.
    - Ext4 is widely used in millions of devices, including Android platforms.

---

### Code Example for Optimized Log Writes

```c
// Pseudo-code for writing a transaction with checksum
void write_transaction_with_checksum(transaction_t *txn) {
    // Compute checksum for the transaction
    txn->checksum = compute_checksum(txn->data);

    // Write the entire transaction (begin, data, end) at once
    write_to_disk(txn->begin_block, txn->data, txn->end_block);

    // During recovery, verify checksum
    if (recovery_mode) {
        if (verify_checksum(txn->data, txn->checksum) == false) {
            discard_transaction(txn);
        }
    }
}
```

---

### Summary

- **Forcing Writes to Disk**: Write barriers ensure proper ordering of writes, but some disks may ignore them for performance.
- **Optimizing Log Writes**: Using checksums in transaction blocks allows faster and more reliable log writes, implemented in Linux ext4.