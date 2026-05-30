### Key Notes for Quick Review

#### 1. **Multiprocessor Architecture Overview**:

- **Key Difference**: Single-CPU vs. Multi-CPU hardware lies in how hardware caches are used and how data is shared across processors.
- **Hardware Caches**:
    - Small, fast memory that stores copies of frequently accessed data from main memory.
    - Improves performance by reducing access time to frequently used data.
- **Main Memory**:
    - Larger but slower memory that holds all data.
    - Accessing data from main memory is slower compared to caches.

#### 2. **Cache Basics in Single-CPU Systems**:

- **Cache Hierarchy**:
    - Example: A CPU with a 64 KB cache and large main memory.
    - First-time data access fetches data from main memory (slow).
    - Subsequent accesses fetch data from the cache (fast).
- **Locality**:
    - **Temporal Locality**: Frequently accessed data is likely to be accessed again soon (e.g., variables in loops).
    - **Spatial Locality**: Data near a recently accessed address is likely to be accessed (e.g., array traversal).

#### 3. **Challenges in Multi-CPU Systems**:

- **Cache Coherence Problem**:
    - When multiple CPUs share a single main memory, maintaining consistency between caches becomes complex.
    - Example:
        - CPU 1 reads data (value D) from address A into its cache.
        - CPU 1 modifies the data (new value D′) in its cache but delays writing it back to main memory.
        - If the program moves to CPU 2 and re-reads address A, it fetches the old value D from main memory instead of the updated value D′.
- **Impact**: This inconsistency leads to incorrect program behavior.

#### 4. **Solutions to Cache Coherence**:

- **Hardware-Based Solutions**:
    - Hardware monitors memory accesses to ensure consistency.
    - **Bus Snooping**:
        - Each cache observes memory updates via the bus connecting them to main memory.
        - When a CPU detects an update to a data item it holds in its cache, it either:
            - **Invalidates** its copy (removes it from the cache).
            - **Updates** its copy (replaces it with the new value).
    - **Write-Back Caches**:
        - Adds complexity as updates to main memory are delayed, but the basic principle remains the same.