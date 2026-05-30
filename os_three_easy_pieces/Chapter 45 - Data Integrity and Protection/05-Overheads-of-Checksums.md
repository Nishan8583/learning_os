### Key Notes for Quick Review

#### **45.8 Overheads of Checksumming**

1. **Overheads of Checksumming**:
    
    - Two types of overheads: **Space** and **Time**.
2. **Space Overheads**:
    
    - **On-disk space overhead**:
        - Each checksum takes up storage space.
        - Example: 8-byte checksum per 4 KB data block = **0.19% on-disk space overhead**.
    - **In-memory space overhead**:
        - Memory must store checksums temporarily during data access.
        - Overhead is minimal if checksums are discarded after verification.
        - Overhead increases if checksums are retained in memory for added protection (e.g., against memory corruption).
3. **Time Overheads**:
    
    - **CPU Overhead**:
        - CPU computes checksums during data storage (to generate checksum) and access (to verify checksum).
        - Optimization: Combine **data copying** and **checksumming** into one operation (e.g., copying data from kernel page cache to user buffer).
    - **I/O Overhead**:
        - Occurs when checksums are stored separately from data, requiring additional I/O operations.
        - Extra I/O may also occur for **background scrubbing** (periodic verification of data integrity).
        - Mitigation strategies:
            - Store checksums alongside data to reduce extra I/O.
            - Schedule scrubbing during low-usage periods (e.g., nighttime).

---

#### **45.9 Summary**

1. **Data Protection in Modern Storage Systems**:
    
    - Checksums are critical for protecting against various types of faults.
    - Different checksum algorithms address different failure modes.
2. **Future Challenges**:
    
    - As storage devices evolve, new failure modes may emerge.
    - This may require revisiting existing checksum approaches or inventing new methods.
3. **Uncertainty of Time**:
    
    - The evolution of storage systems and data protection methods will depend on future developments, which are inherently unpredictable.