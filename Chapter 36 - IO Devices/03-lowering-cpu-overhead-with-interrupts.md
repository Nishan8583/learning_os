### Key Notes on Lowering CPU Overhead with Interrupts

#### 1. **Introduction to Interrupts**

- Interrupts are used to improve CPU utilization by avoiding constant polling of devices.
- Instead of polling, the OS issues a request, puts the calling process to sleep, and switches to another task.
- When the device completes its operation, it raises a hardware interrupt, triggering the CPU to execute an **Interrupt Service Routine (ISR)** or **Interrupt Handler**.
- The ISR completes the request (e.g., reading data or error codes) and wakes the waiting process.

#### 2. **Benefits of Interrupts**

- **Overlap of Computation and I/O**: Interrupts allow the CPU to perform other tasks while waiting for I/O operations to complete.
- Example Timeline:
    - Without Interrupts:
        
        ```
        CPU: 1 1 1 1 1 p p p p p 1 1 1 1 1
        Disk:         1 1 1 1 1
        ```
        
        - The CPU is idle (polling) while waiting for the disk to complete the I/O request.
    - With Interrupts:
        
        ```
        CPU: 1 1 1 1 1 2 2 2 2 2 1 1 1 1 1
        Disk:         1 1 1 1 1
        ```
        
        - The CPU runs another process (Process 2) while the disk services the I/O request. When the disk finishes, an interrupt wakes Process 1.

#### 3. **When Interrupts Are Not Ideal**

- **Fast Devices**: If a device completes tasks quickly, polling may be more efficient than interrupts. Context switching and interrupt handling can introduce unnecessary overhead.
- **Interrupt Flooding**: A high rate of interrupts (e.g., from a network device handling a flood of incoming packets) can lead to **livelock**, where the OS spends all its time processing interrupts and cannot service user-level processes.
- **Hybrid Approach**: For devices with variable speeds, a two-phased approach can be used:
    - Poll for a short time.
    - If the device is not done, switch to interrupts.

#### 4. **Interrupt Coalescing**

- **Definition**: A device delays raising an interrupt to allow multiple requests to complete, coalescing them into a single interrupt.
- **Trade-off**: Reduces interrupt overhead but increases request latency.
- Example: See Ahmad et al. [A+11] for a detailed summary.

#### 5. **Key Takeaways**

- **Interrupts are not always better than polling**:
    - Use interrupts for slow devices to allow overlap of computation and I/O.
    - Use polling for fast devices to avoid the overhead of context switching and interrupt handling.
- **Hybrid Approach**: Combines polling and interrupts for devices with variable speeds.
- **Interrupt Coalescing**: Reduces interrupt overhead by batching multiple interrupts, but at the cost of increased latency.
- **Livelock Risk**: High interrupt rates can overload the system, making polling a better option in some cases (e.g., network packet floods).