### Key Notes for Quick Review

#### 1. **Cache Affinity in Multiprocessor Systems**:

- **Definition**:
    - Cache affinity refers to the benefit of running a process on the same CPU where it previously executed.
    - When a process runs on a CPU, it builds up state in the CPU's caches (and TLBs).
- **Performance Impact**:
    - If the process is scheduled on the same CPU during subsequent runs, it can leverage the cached state, leading to faster execution.
    - If the process is moved to a different CPU, it must reload its state into the new CPU's caches, which can degrade performance.
- **Correctness**:
    - Even if a process is moved to a different CPU, it will still run correctly due to hardware cache coherence protocols, but performance will suffer.

#### 2. **Scheduler Considerations**:

- A multiprocessor scheduler should account for cache affinity when making scheduling decisions.
- **Preferred Strategy**:
    - Keep a process on the same CPU whenever possible to maximize performance by utilizing the existing cache state.
- **Trade-offs**:
    - While prioritizing cache affinity can improve performance, it may conflict with other scheduling goals, such as load balancing across CPUs.

#### 3. **Key Takeaways**:

- Cache affinity is an important factor in multiprocessor scheduling to optimize performance.
- Scheduling decisions should balance cache affinity with other considerations like CPU load distribution.
- Cache coherence protocols ensure correctness even when processes are moved between CPUs, but performance may degrade due to cache state reloading.