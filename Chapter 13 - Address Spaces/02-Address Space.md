### Key Notes: The Address Space

#### 1. **Definition of Address Space**

- **Abstraction of Physical Memory**: The OS creates an abstraction called the _address space_, which is the running program’s view of memory.
- **Components of Address Space**:
    - **Code**: Contains the program’s instructions, static in size and location.
    - **Stack**: Used for function call tracking, local variables, and parameter passing. Grows upward.
    - **Heap**: Used for dynamically-allocated memory (e.g., `malloc()` in C, `new` in C++/Java). Grows downward.
- **Example Layout**:
    - Code at the top (e.g., starts at address 0).
    - Stack at the bottom (e.g., starts at 16KB).
    - Heap in between, growing in the opposite direction of the stack.

#### 2. **Address Space as an OS Abstraction**

- **Virtual vs. Physical Memory**:
    - The address space is a _virtual_ representation of memory provided to the program.
    - The program is not actually loaded at physical addresses 0–16KB but at arbitrary physical addresses (e.g., 320KB for process A).
- **Virtualization of Memory**:
    - The OS, with hardware support, maps virtual addresses (e.g., 0) to physical addresses (e.g., 320KB).
    - This allows each process to think it has its own private, large address space (e.g., 32-bit or 64-bit), even though physical memory is shared.

#### 3. **Why Virtualization is Necessary**

- **Multiple Processes**: Enables multiple processes to coexist in memory without interfering with each other.
- **Protection**: Ensures that one process cannot access or modify another process’s memory.
- **Efficiency**: Allows processes to share physical memory while maintaining the illusion of private memory spaces.

#### 4. **Challenges in Virtualizing Memory**

- **Dynamic Growth**: The stack and heap grow during program execution, requiring careful management to avoid overlap.
- **Physical Memory Mapping**: The OS must efficiently map virtual addresses to physical addresses for each process.
- **Hardware Support**: Virtualization relies on hardware mechanisms (e.g., memory management units) to translate addresses.

#### 5. **The Crux of Virtualization**

- **Key Question**: How does the OS provide private, large address spaces for multiple processes while sharing a single physical memory?
- **Solution**: The OS uses virtualization techniques, supported by hardware, to map virtual addresses to physical memory dynamically and securely.

#### 6. **Takeaways**

- The address space is a fundamental abstraction that simplifies memory management for programs.
- Virtualization of memory is essential for modern computing, enabling multitasking, protection, and efficient resource utilization.
- The OS and hardware work together to ensure that each process has its own isolated and consistent view of memory.
![[chapter-13-3.png]]