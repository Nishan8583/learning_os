### Key Notes on Dealing With I/O and Sleeping Processes in CFS

1. **Problem with Sleeping Processes**:
    
    - When a process (e.g., **B**) sleeps for a long time (e.g., 10 seconds) and wakes up, its `vruntime` will lag significantly behind a continuously running process (e.g., **A**).
    - If not addressed, **B** could monopolize the CPU to "catch up," effectively **starving** process **A**.
2. **CFS Solution**:
    
    - When a sleeping process wakes up, its `vruntime` is **adjusted** to match the **minimum vruntime** in the red-black tree (which tracks only running processes).
    - This ensures that the waking process does not dominate the CPU and maintains fairness.
3. **Avoiding Starvation**:
    
    - By resetting the `vruntime` of the waking process to the minimum value, CFS prevents **starvation** of other processes.
4. **Trade-Off**:
    
    - This approach may slightly penalize processes that sleep for **short periods**, as their `vruntime` adjustment might not fully reflect their actual CPU usage.
5. **Fairness and Balance**:
    
    - The adjustment ensures that both long-sleeping and continuously running processes are treated fairly, maintaining the **fairness principle** of CFS.
    - 