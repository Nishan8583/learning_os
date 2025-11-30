### Key Notes on "Which Blocks to Clean, and When?" and Related Topics in LFS

#### **43.11 A Policy Question: Which Blocks to Clean, and When?**

1. **When to Clean**:
    
    - Cleaning can occur:
        - **Periodically**: At regular intervals.
        - **During idle time**: When the system is not busy.
        - **When the disk is full**: Cleaning becomes mandatory to free up space.
2. **Which Blocks to Clean**:
    
    - **Hot vs. Cold Segments**:
        - **Hot Segments**: Contain frequently overwritten data. Cleaning is delayed as more blocks are likely to become dead over time.
        - **Cold Segments**: Contain mostly stable data with a few dead blocks. These are prioritized for cleaning.
    - **Heuristics**:
        - The original LFS paper [RO91] proposed a heuristic to clean cold segments sooner and hot segments later.
        - Later research [MR+97] introduced improved approaches to optimize cleaning policies.

---

#### **43.12 Crash Recovery and the Log**

1. **Crash Scenarios**:
    
    - Crashes can occur during:
        - Writing to a segment.
        - Updating the **Checkpoint Region (CR)**.
2. **Handling CR Updates**:
    
    - **Dual Checkpoint Regions**:
        - LFS maintains two CRs at opposite ends of the disk.
        - CRs are written alternately to ensure atomic updates.
    - **Update Protocol**:
        - Write a header with a timestamp.
        - Write the body of the CR.
        - Write a final block with a timestamp.
    - **Crash Recovery**:
        - If a crash occurs during a CR update, LFS checks the timestamps.
        - The most recent CR with consistent timestamps is used for recovery.
3. **Handling Segment Writes**:
    
    - LFS writes the CR every 30 seconds, so the last consistent snapshot may be outdated.
    - **Recovery Process**:
        - Read the last checkpoint region and the inode map it points to.
        - Rebuild the file system from the checkpoint region.
        - **Roll Forward**:
            - Start from the last checkpoint region.
            - Find the end of the log and read subsequent segments for valid updates.
            - Recover data and metadata written since the last checkpoint.

---

#### **43.13 Summary**

1. **LFS Approach**:
    
    - LFS uses a **copy-on-write** approach (similar to **shadow paging** in databases).
    - Instead of overwriting files, LFS writes to unused portions of the disk and reclaims old space through cleaning.
2. **Advantages**:
    
    - **Efficient Writing**:
        - Writes are gathered into in-memory segments and written sequentially.
        - Large sequential writes improve performance on:
            - **Hard drives**: Minimize positioning time.
            - **RAID systems**: Avoid the small-write problem.
            - **Flash-based SSDs**: Large I/Os improve performance.
3. **Challenges**:
    
    - **Garbage Generation**:
        - Old data copies are scattered across the disk.
        - Cleaning is required to reclaim space, which can be costly.
    - **Cleaning Controversy**:
        - Cleaning costs were a significant concern and limited LFS's initial adoption.
4. **Modern Applications**:
    
    - LFS's copy-on-write approach influenced modern file systems:
        - **NetApp’s WAFL**: Turns cleaning into a feature by providing snapshots of old file versions.
        - **Sun’s ZFS** and **Linux btrfs**: Adopt similar approaches.
        - **Flash-based SSDs**: Use LFS-style techniques for high performance.
5. **Turning Flaws into Features**:
    
    - Example: NetApp’s WAFL uses old file versions as a feature (snapshots), reducing the need for frequent cleaning.
    - **Lesson**: Fundamental flaws in a system can sometimes be transformed into useful features.

---

### Summary of Key Concepts:

- **Cleaning Policies**:
    - Clean during idle time, periodically, or when the disk is full.
    - Prioritize cleaning cold segments over hot segments.
- **Crash Recovery**:
    - Use dual checkpoint regions and a careful update protocol to ensure atomicity.
    - Employ roll-forward techniques to recover updates since the last checkpoint.
- **LFS Advantages**:
    - Efficient sequential writes improve performance on various storage devices.
- **LFS Challenges**:
    - Cleaning costs and garbage generation are significant downsides.
- **Modern Impact**:
    - LFS principles influence modern file systems like WAFL, ZFS, and btrfs.
- **Design Insight**:
    - Turn system flaws into features to add value and mitigate challenges.