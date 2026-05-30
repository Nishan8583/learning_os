### Key Notes on FFS Policies for Allocating Files and Directories

#### **General Mantra**

- **Keep related stuff together**: Place related files and directories close to each other.
- **Keep unrelated stuff far apart**: Place unrelated files and directories in different block groups.

---

#### **Placement Heuristics**

1. **Directories**:
    
    - Place directories in cylinder groups with:
        - A low number of allocated directories (to balance directories across groups).
        - A high number of free inodes (to allow allocation of files later).
    - Other factors like the number of free data blocks can also be considered.
2. **Files**:
    
    - **File Data and Inode Proximity**: Allocate data blocks of a file in the same group as its inode to minimize seek time.
    - **Namespace-Based Locality**: Place files in the same directory within the same cylinder group as the directory itself.
        - Example: Files `/a/c`, `/a/d`, and `/a/e` are placed in the same group as `/a`, while `/b/f` is placed in a different group with `/b`.

---

#### **Example of FFS Allocation**

- Assumptions:
    - 10 inodes and 10 data blocks per group.
    - Three directories: `/`, `/a`, `/b`.
    - Four files: `/a/c`, `/a/d`, `/a/e`, `/b/f`.
    - Regular files are 2 blocks in size; directories have 1 block of data.

**FFS Allocation:**

|Group|Inodes|Data|
|---|---|---|
|0|`/---------`|`/---------`|
|1|`acde------`|`accddee---`|
|2|`bf--------`|`bff-------`|
|3|`----------`|`----------`|
|4-7|`----------`|`----------`|

- **Observations**:
    - Data blocks of each file are near their respective inodes.
    - Files in the same directory are grouped together (e.g., `/a/c`, `/a/d`, `/a/e` in Group 1).
    - Files in different directories are placed in separate groups (e.g., `/b/f` in Group 2).

---

#### **Comparison with Arbitrary Inode Allocation**

- **Arbitrary Allocation**:
    
    - Inodes are spread across groups to prevent any group’s inode table from filling up.
    - Example Allocation:
        
        |Group|Inodes|Data|
        |---|---|---|
        |0|`/---------`|`/---------`|
        |1|`a---------`|`a---------`|
        |2|`b---------`|`b---------`|
        |3|`c---------`|`cc--------`|
        |4|`d---------`|`dd--------`|
        |5|`e---------`|`ee--------`|
        |6|`f---------`|`ff--------`|
        |7|`----------`|`----------`|
        
- **Drawbacks**:
    
    - Files within the same directory are spread across multiple groups.
    - Namespace-based locality is not preserved.
    - Accessing files like `/a/c`, `/a/d`, and `/a/e` requires spanning three groups, increasing seek time.

---

#### **Advantages of FFS Policy**

- **Improved Performance**:
    - Shorter seek times between related files due to namespace-based locality.
    - Files in the same directory are often accessed together (e.g., during compilation and linking), so grouping them improves efficiency.
- **Common-Sense Approach**:
    - FFS heuristics are not based on extensive studies but rely on practical reasoning about file system usage patterns.