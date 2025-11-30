### Key Notes: Data Integrity and Protection

#### **Overview**

- Focus: Ensuring **data integrity** and **protection** in storage systems.
- Goal: Ensure data written to storage is the same when retrieved, despite the unreliable nature of modern storage devices.
- Key Challenges:
    - How to protect data written to storage.
    - Techniques to ensure data integrity with minimal space and time overhead.

---

#### **Disk Failure Modes**

1. **Traditional Fail-Stop Model**:
    
    - Disk either works completely or fails entirely.
    - Detection of failure is straightforward.
2. **Modern Fail-Partial Model**:
    
    - Disks may appear functional but exhibit partial failures:
        - **Latent Sector Errors (LSEs)**: Disk sectors become unreadable due to damage (e.g., head crash, cosmic rays).
        - **Block Corruption**: Data is corrupted but not detected by the disk (e.g., buggy firmware, faulty bus).

---

#### **Types of Disk Faults**

1. **Non-Silent Partial Faults**:
    - Disk returns an error when trying to read/write a block.
2. **Silent Partial Faults**:
    - Disk returns incorrect data without indicating a problem.

---

#### **Frequency of Disk Faults**

- **Latent Sector Errors (LSEs)**:
    - Cheap drives: 9.40% of drives exhibit LSEs.
    - Costly drives: 1.40% of drives exhibit LSEs.
- **Block Corruption**:
    - Cheap drives: 0.50% of drives exhibit corruption.
    - Costly drives: 0.05% of drives exhibit corruption.

---

#### **Findings on LSEs**

- Costly drives with more than one LSE are as likely to develop additional errors as cheaper drives.
- Annual error rate increases in year two for most drives.
- LSEs increase with disk size.
- Most disks with LSEs have fewer than 50 errors.
- Disks with LSEs are more likely to develop additional LSEs.
- Significant **spatial** and **temporal locality** observed.
- **Disk scrubbing** (periodic scanning) is effective in detecting LSEs.

---

#### **Findings on Block Corruption**

- Corruption rates vary significantly across drive models within the same class.
- Age effects differ across models.
- Workload and disk size have little impact on corruption rates.
- Most disks with corruption have only a few corruptions.
- Corruption is not independent within a disk or across disks in RAID.
- **Spatial locality** and some **temporal locality** observed.
- Weak correlation between LSEs and corruption.

---

#### **Key Takeaways**

- Both LSEs and block corruption are rare but significant enough to require attention in storage system design.
- Reliable storage systems must include mechanisms to **detect** and **recover** from:
    - Latent Sector Errors (LSEs).
    - Block corruption.

#### **Recommendations**

- Use **disk scrubbing** to detect LSEs.
- Invest in better-quality drives to reduce failure rates, though issues may still occur.
- Implement robust error detection and correction mechanisms (e.g., **ECC**).
- Consider spatial and temporal locality when designing recovery mechanisms.

#### **References**

- Bairavasundaram et al. [B+07, B+08]: Studies on disk failure modes.
- Prabhakaran et al. [P+05]: Fail-partial disk failure model.