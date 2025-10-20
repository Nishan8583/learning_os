### Key Notes: Mechanism - Address Translation

#### **General Concept: Limited Direct Execution (LDE)**

- **Definition**: A mechanism where programs run directly on hardware for efficiency, but the OS intervenes at critical points (e.g., system calls, timer interrupts) to maintain control.
- **Goals**:
    - **Efficiency**: Minimize OS interference to allow programs to run directly on hardware.
    - **Control**: Ensure the OS maintains authority over hardware and system behavior.
- **Application to Memory Virtualization**:
    - Similar strategy as CPU virtualization.
    - Use hardware support for efficiency and OS intervention for control.

---

#### **Virtualizing Memory**

- **Goals**:
    1. **Efficiency**: Use hardware mechanisms (e.g., registers, TLBs, page tables) to minimize overhead.
    2. **Control**: Ensure applications can only access their allocated memory, protecting the OS and other applications.
    3. **Flexibility**: Allow applications to use their address spaces freely, simplifying programming.
- **Crux**:
    - How to efficiently and flexibly virtualize memory while maintaining control over memory access.

---

#### **Mechanism: Address Translation**

- **Definition**: A hardware-based technique where every memory access (instruction fetch, load, store) translates a **virtual address** to a **physical address**.
- **Process**:
    - Hardware performs address translation for every memory reference.
    - OS sets up and manages the hardware to ensure correct translations.
- **Purpose**:
    - Create the illusion of private memory for each program.
    - Hide the reality of shared physical memory among multiple programs.

---

#### **Role of the OS in Address Translation**

- **Responsibilities**:
    - Configure hardware to perform correct address translations.
    - Manage memory allocation (track free and used memory locations).
    - Intervene to maintain control and enforce memory access restrictions.
- **Outcome**:
    - Programs perceive a private memory space (virtual memory).
    - OS abstracts the complexity of shared physical memory.

---

#### **Assumptions for Initial Virtualization**

1. **Contiguous Placement**: User address spaces are placed contiguously in physical memory.
2. **Address Space Size**: Address spaces are smaller than physical memory.
3. **Uniform Size**: All address spaces are of the same size.

- **Note**: These assumptions are simplified and will be relaxed later to achieve realistic memory virtualization.


### Key Notes: Interposition in Computer Systems

#### **Definition of Interposition**

- **Interposition**: A technique where a system intercepts operations at a well-defined interface to add functionality or modify behavior.
- **In Memory Virtualization**:
    - Hardware interposes on every memory access.
    - Translates **virtual addresses** issued by a process into **physical addresses** where data is stored.

---

#### **General Applications of Interposition**

- **Broad Applicability**: Can be used in various systems beyond memory virtualization.
- **Interface Interposition**:
    - Any well-defined interface can be interposed upon.
    - Enables the addition of new functionality or improvements to the system.

---

#### **Benefits of Interposition**

1. **Transparency**:
    - Interposition is often invisible to the client.
    - No changes are required to the client interface.
2. **Flexibility**:
    - Allows for seamless integration of new features or optimizations.
3. **Control**:
    - Provides a mechanism to monitor, modify, or manage system behavior at critical points.