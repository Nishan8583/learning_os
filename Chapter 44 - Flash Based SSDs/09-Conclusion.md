### Key Notes for Quick Review

#### **44.10 Wear Leveling**

- **Purpose**: To evenly distribute erase/program cycles across all blocks in an SSD to prevent a few blocks from wearing out prematurely.
- **Problem**: Some blocks may contain long-lived data that is not overwritten, making them unavailable for garbage collection and write operations.
- **Solution**:
    - Periodically read live data from such blocks and rewrite it elsewhere to free up the block.
    - This process increases **write amplification**, reducing performance due to extra I/O operations.
- **Algorithms**: Various wear-leveling algorithms exist in the literature (e.g., [A+08], [M+14]).

---

#### **44.11 SSD Performance and Cost**

##### **Performance**

- **Key Differences Between SSDs and HDDs**:
    - SSDs have no mechanical components and are "random access" devices, similar to DRAM.
    - SSDs excel in **random I/O performance** compared to HDDs:
        - SSDs achieve tens or hundreds of MB/s in random I/Os.
        - HDDs achieve only ~2 MB/s in random I/Os.
    - **Sequential I/O performance** shows less difference between SSDs and HDDs.
- **Performance Data** (from Table 44.4):

|Device|Random Reads (MB/s)|Random Writes (MB/s)|Sequential Reads (MB/s)|Sequential Writes (MB/s)|
|---|---|---|---|---|
|Samsung 840 Pro SSD|299|972|192|384|
|Seagate 600 SSD|84|797|373|733|
|Intel SSD 335 SSD|39|666|277|227|
|Seagate Savvio 15K.3 HDD|2|2|223|223|

- **Insights**:
    1. SSDs outperform HDDs significantly in random I/O performance.
    2. Sequential performance differences are smaller; HDDs are still viable for sequential workloads.
    3. SSD random write performance is often better than random read performance due to log-structured designs that convert random writes into sequential writes.
    4. File system design for SSDs should still consider the gap between sequential and random I/Os.

##### **Cost**

- **SSD vs. HDD Cost**:
    - SSD: ~$150 for 250 GB (~60 cents/GB).
    - HDD: ~$50 for 1 TB (~5 cents/GB).
    - SSDs are ~10× more expensive per GB than HDDs.
- **Implications**:
    - SSDs are ideal for performance-critical applications, especially those requiring high random read performance.
    - HDDs are preferred for large-scale storage due to their lower cost.
    - **Hybrid Approach**: Combine SSDs for "hot" (frequently accessed) data and HDDs for "cold" (less accessed) data to balance performance and cost.

---

#### **44.12 Summary**

- **SSDs in Modern Systems**:
    - Widely used in laptops, desktops, and data centers.
    - Essential to understand their performance and cost trade-offs.
- **Further Reading**:
    - Research on SSD performance: Chen et al. [CK+09], Grupp et al. [GC+09].
    - FTL design: Agrawal et al. [A+08], Gupta et al. [GY+09], Huang et al. [H+14], etc.
    - Distributed systems with flash: Gordon [CG+09], CORFU [B+12].
    - Practical advances: Samsung's use of TLC and SLC cells for performance and capacity.
    - Comprehensive surveys: Ma et al. [M+14].
- **Caution**: The field is vast and complex; further exploration is encouraged.