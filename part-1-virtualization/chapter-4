**OS 3 Easy Pieces**

# Process

### Key Notes on the Process and CPU Virtualization

#### Definition of a Process

- A **process** is a running program.
- A program on disk is lifeless until the operating system (OS) transforms it into a running process.

#### Need for Multiple Processes

- Users often run multiple programs simultaneously (e.g., browser, mail, music player).
- A system may appear to run tens or hundreds of processes concurrently.
- The challenge: **How to provide the illusion of many CPUs when only a few physical CPUs exist?**

#### Virtualization of the CPU

- The OS **virtualizes the CPU** to create the illusion of many CPUs.
- Achieved through **time sharing**:
    - The OS runs one process, stops it, runs another, and repeats.
    - This gives the appearance of concurrent execution on a single or limited number of CPUs.
    - **Trade-off**: Performance may degrade as processes share CPU time.

#### Mechanisms and Policies

1. **Mechanisms**:
    
    - Low-level methods or protocols to implement functionality.
    - Example: **Context switch**:
        - Allows the OS to stop one program and start another on the CPU.
        - Fundamental to time-sharing in modern OSes.
2. **Policies**:
    
    - High-level intelligence in the OS to make decisions.
    - Example: **Scheduling policy**:
        - Decides which program to run on the CPU.
        - Factors considered:
            - Historical data (e.g., which program has run more recently).
            - Workload knowledge (e.g., types of programs running).
            - Performance goals (e.g., optimizing for interactivity or throughput).

#### Time Sharing vs. Space Sharing

- **Time Sharing**:
    - Resource (e.g., CPU) is shared over time among multiple entities.
    - Example: CPU time is divided among processes.
- **Space Sharing**:
    - Resource is divided spatially among users.
    - Example: Disk space is allocated to files and not reused until the file is deleted.

  

  

### Key Notes on **The Abstraction: A Process**

#### Definition of a Process

- A **process** is the OS abstraction of a running program.
- It represents the program's **machine state** during execution.

#### Components of a Process

1. **Memory (Address Space)**:
    
    - Contains the instructions and data the process reads or writes.
    - Defines the memory the process can address.
2. **Registers**:
    
    - Includes general-purpose registers that are read or updated by instructions.
    - Special registers include:
        - **Program Counter (PC)**: Tracks the next instruction to execute.
        - **Stack Pointer**: Manages the stack for function parameters, local variables, and return addresses.
3. **I/O Information**:
    
    - Includes persistent storage details, such as files the process has open.

#### Key Design Principle: Separate Policy and Mechanism

- **Mechanism**:
    - Answers the "how" question (e.g., how does the OS perform a context switch?).
    - Provides low-level functionality.
- **Policy**:
    - Answers the "which" question (e.g., which process should run next?).
    - Guides high-level decision-making.
- **Benefit**:
    - Separation allows modularity, enabling changes to policies without altering mechanisms.

  

  

### Key Notes on **Process API**

#### Overview

- The **Process API** defines the interface provided by the operating system for managing processes.
- These APIs are essential for process lifecycle management and are available in all modern operating systems.

#### Core Functions of the Process API

1. **Create**:
    
    - Allows the creation of new processes.
    - Triggered when a user types a command (e.g., in a shell) or double-clicks an application icon.
    - The OS creates a new process to run the specified program.
2. **Destroy**:
    
    - Provides a way to terminate processes forcefully.
    - Useful for halting runaway or unresponsive processes.
    - Many processes exit naturally upon completion, but this interface is critical for manual intervention.
3. **Wait**:
    
    - Enables waiting for a process to complete execution.
    - Useful for synchronization between processes (e.g., a parent process waiting for a child process to finish).
4. **Miscellaneous Control**:
    
    - Includes additional controls for process management:
        - **Suspend**: Temporarily stop a process from running.
        - **Resume**: Restart a suspended process.
5. **Status**:
    
    - Provides information about a process, such as:
        - How long it has been running.
        - Its current state (e.g., running, waiting, suspended).

  !(cpu.png)[cpu.png]

  
Homework:
https://github.com/JorritWillaert/Operating-systems-Three-easy-pieces
Normal Github: https://github.com/remzi-arpacidusseau/ostep-code/tree/master?tab=readme-ov-file
### Key Notes on **Process Creation**

#### Overview

- Process creation involves transforming a program into a running process.
- The OS performs several steps to prepare the program for execution.

#### Steps in Process Creation

1. **Loading Code and Static Data**:
    
    - Programs reside on disk (or SSD) in an executable format.
    - The OS loads the program's **code** and **static data** (e.g., initialized variables) into the process's **address space** in memory.
    - **Loading Methods**:
        - **Eager Loading**: Load all program data into memory before execution (used in simpler systems).
        - **Lazy Loading**: Load pieces of code or data only when needed during execution (used in modern systems).
2. **Allocating Memory for the Stack**:
    
    - The **stack** is used for:
        - Local variables.
        - Function parameters.
        - Return addresses.
    - The OS initializes the stack with arguments for the `main()` function (e.g., `argc` and `argv`).
3. **Allocating Memory for the Heap**:
    
    - The **heap** is used for dynamically-allocated data (e.g., via `malloc()` in C).
    - Initially small, the heap grows as the program requests more memory.
    - The OS allocates additional memory to the heap as needed.
4. **I/O Initialization**:
    
    - The OS sets up default **file descriptors** for I/O:
        - **Standard Input** (stdin).
        - **Standard Output** (stdout).
        - **Standard Error** (stderr).
    - These descriptors allow programs to interact with the terminal for input and output.
5. **Starting Execution**:
    
    - The OS sets the **entry point** of the program to `main()`.
    - It transfers control of the CPU to the newly-created process, allowing the program to begin execution.

### Key Notes on **Process States**

#### Overview

- A process can exist in one of **three primary states** during its lifecycle:
    - **Running**: Actively executing instructions on the CPU.
    - **Ready**: Prepared to run but waiting for the OS to schedule it.
    - **Blocked**: Waiting for an event (e.g., I/O completion) before it can proceed.

#### Process State Transitions

- **Ready → Running**: The OS **schedules** the process to execute on the CPU.
- **Running → Ready**: The OS **deschedules** the process, often to allow another process to run.
- **Running → Blocked**: The process performs an operation (e.g., I/O request) that requires it to wait for an event.
- **Blocked → Ready**: The event (e.g., I/O completion) occurs, making the process ready to run again.

#### State Transition Diagram

- The transitions between states can be visualized as follows:

|**State**|**Transition Trigger**|**Next State**|
|---|---|---|
|**Ready → Running**|Process is scheduled by the OS|Running|
|**Running → Ready**|Process is descheduled by the OS|Ready|
|**Running → Blocked**|Process initiates an I/O operation|Blocked|
|**Blocked → Ready**|I/O operation or event completes|Ready|

#### Example Scenarios

1. **CPU-Only Processes**:
    
    - Two processes (Process0 and Process1) alternate between **Running** and **Ready** states.
    - Example Trace:
        - Time 1–4: Process0 runs while Process1 remains ready.
        - Time 5–8: Process1 runs after Process0 completes.
2. **CPU and I/O Processes**:
    
    - Process0 initiates an I/O operation and becomes **Blocked**.
    - The OS schedules Process1 to run while Process0 waits for the I/O to complete.
    - Example Trace:
        - Time 1–3: Process0 runs while Process1 is ready.
        - Time 4–6: Process0 initiates I/O and becomes blocked; Process1 runs.
        - Time 7: Process0's I/O completes, moving it to **Ready**.
        - Time 8–10: Process1 finishes, and Process0 resumes running.

#### Key Observations

- **Resource Utilization**:
    - The OS schedules another process (e.g., Process1) while one process (e.g., Process0) is blocked, ensuring the CPU remains busy.
- **Scheduling Decisions**:
    - The OS decides whether to switch back to a process (e.g., Process0) immediately after its I/O completes or let the currently running process (e.g., Process1) finish first.
    - These decisions are made by the **OS scheduler**, which balances performance, fairness, and resource utilization.


### Key Notes on "4.5 Data Structures" (Process Management in Operating Systems)

#### **1. Overview of OS Data Structures**

- The OS uses key data structures to track information about processes.
- **Process List**: Tracks all processes in the system, including those ready to run, currently running, or blocked.
- **Blocked Processes**: The OS ensures blocked processes are woken up and made ready when their I/O events complete.

---

#### **2. Process Information in xv6 Kernel**

- The OS tracks various details about each process using a structure similar to the **Process Control Block (PCB)** in modern systems.
- **Key Components of `struct proc` in xv6**:
    - **Memory Management**:
        - `char *mem`: Start of process memory.
        - `uint sz`: Size of process memory.
    - **Kernel Stack**:
        - `char *kstack`: Bottom of the kernel stack for the process.
    - **Process State**:
        - `enum proc_state`: Tracks process states (e.g., UNUSED, RUNNING, SLEEPING, ZOMBIE).
    - **Identifiers**:
        - `int pid`: Process ID.
        - `struct proc *parent`: Pointer to the parent process.
    - **Synchronization**:
        - `void *chan`: Indicates the channel the process is sleeping on (if any).
    - **Termination**:
        - `int killed`: Indicates if the process has been killed.
    - **File and Directory Management**:
        - `struct file *ofile[NOFILE]`: Array of open files.
        - `struct inode *cwd`: Current working directory.
    - **Context Switching**:
        - `struct context context`: Stores register values for stopping and restarting the process.
        - `struct trapframe *tf`: Trap frame for the current interrupt.

---

#### **3. Process States**

- Processes can be in various states:
    - **UNUSED**: Not in use.
    - **EMBRYO**: Initial state during process creation.
    - **SLEEPING**: Waiting for an event.
    - **RUNNABLE**: Ready to run but not currently running.
    - **RUNNING**: Actively executing.
    - **ZOMBIE**: Process has exited but not yet cleaned up.

---

#### **4. Context Switching**

- **Register Context**:
    - Holds the contents of a process's registers when it is stopped.
    - Registers are saved to memory during a context switch and restored to resume the process.
- **Purpose**:
    - Enables the OS to switch between processes efficiently.

---

#### **5. Zombie State**

- A process enters the **zombie state** after it exits but before its resources are cleaned up.
- **Purpose**:
    - Allows the parent process to examine the return code of the child process.
    - Parent calls `wait()` to retrieve the return code and signal the OS to clean up the process's data structures.

---

#### **6. Process List (Task List)**

- A fundamental OS data structure to track all running programs.
- Each process is represented by a **Process Control Block (PCB)** or **process descriptor**.
- Essential for managing multiple programs in a multitasking OS.

---

#### **7. Practical Insights**

- Modern operating systems like Linux, Mac OS X, and Windows use similar but more complex structures to manage processes.
- The xv6 kernel provides a simplified view of how processes are tracked and managed, serving as a foundation for understanding real-world OS implementations.
```c
    struct context {
    int eip;
    int esp;
    int ebx;
    int ecx;
    int edx;
    int esi;
    int edi;
    int ebp;
    };
    // the different states a process can be in
    enum proc_state { UNUSED, EMBRYO, SLEEPING,
    RUNNABLE, RUNNING, ZOMBIE };
    // the information xv6 tracks about each process
    // including its register context and state
    struct proc {
    char *mem; // Start of process memory
    uint sz; // Size of process memory
    char *kstack; // Bottom of kernel stack
    // for this process
    enum proc_state state; // Process state
    int pid; // Process ID
    struct proc *parent; // Parent process
    void *chan; // If !zero, sleeping on chan
    int killed; // If !zero, has been killed
    struct file *ofile[NOFILE]; // Open files
    struct inode *cwd; // Current directory
    struct context context; // Switch here to run process
    struct trapframe *tf; // Trap frame for the
    // current interrupt
```
