# RAID Notes (Summary)
(Summary of the attached PDF)

## What is RAID?
- RAID = Redundant Array of Inexpensive Disks.
- Combines multiple disks to improve **capacity, performance, and reliability**.
- Externally appears as one large disk.

## RAID Evaluation Axes
- **Capacity:** usable storage after redundancy.
- **Reliability:** number of disk failures tolerated.
- **Performance:** depends on workload (sequential vs random).

## RAID-0 (Striping)
- **No redundancy**, just performance.
- Data striped across disks in round-robin fashion.
- **Capacity:** N × B  
- **Reliability:** 0 (any disk failure loses all data)
- **Performance:**  
  - Sequential: N × S  
  - Random: N × R

## RAID-1 (Mirroring)
- Two copies of each block stored on two disks.
- **Capacity:** (N × B) / 2  
- **Reliability:** tolerates 1 disk failure (more if "lucky")  
- **Performance:**  
  - Sequential read/write: (N/2) × S  
  - Random read: N × R  
  - Random write: (N/2) × R

## RAID-4 (Parity Disk)
- One disk stores parity for each stripe.
- Parity computed via **XOR** across blocks.
- **Capacity:** (N − 1) × B  
- **Reliability:** 1 disk failure  
- **Performance:**  
  - Sequential read/write: (N − 1) × S  
  - Random read: (N − 1) × R  
  - **Random write:** terrible → parity disk bottleneck (“small-write problem”)

## RAID-5 (Rotating Parity)
- Parity distributed across all disks.
- Eliminates RAID-4 bottleneck.
- **Capacity:** (N − 1) × B  
- **Reliability:** 1 disk failure  
- **Performance:**  
  - Sequential read/write: (N − 1) × S  
  - Random read: N × R  
  - Random write: N/4 × R (better than RAID-4)

## Parity & XOR Basics
- XOR ensures even number of 1s across data + parity bits.
- Missing data reconstructed by XORing remaining bits.
- Block-level parity computed bit-by-bit across entire blocks.

## Latency (Simplified)
- Read: T  
- RAID-1/5 writes: ~2T  

## RAID Comparison Table (Simplified)

| RAID | Capacity | Fault Tolerance | Seq Read | Seq Write | Rand Read | Rand Write |
|------|----------|----------------|----------|-----------|------------|-----------|
| 0 | N·B | 0 | N·S | N·S | N·R | N·R |
| 1 | (N·B)/2 | 1 | (N/2)·S | (N/2)·S | N·R | (N/2)·R |
| 4 | (N−1)·B | 1 | (N−1)·S | (N−1)·S | (N−1)·R | R/2 |
| 5 | (N−1)·B | 1 | (N−1)·S | (N−1)·S | N·R | N/4·R |

## Key Takeaways
- **Best performance (no reliability):** RAID-0  
- **Best random performance + reliability:** RAID-1  
- **Best capacity + reliability:** RAID-5  
- **Worst for small writes:** RAID-4  
