### Key Notes on "What About The Stack?"

1. **Stack Growth Direction**:
    
    - The stack grows **backwards** (towards lower addresses) in physical memory.
    - Example: In the diagram, it starts at **28KB** and grows back to **26KB**, corresponding to virtual addresses **16KB to 14KB**.
2. **Hardware Support for Stack Growth**:
    
    - Additional hardware support is required to handle segments that grow negatively.
    - A **growth direction bit** is introduced:
        - `1`: Segment grows in the positive direction.
        - `0`: Segment grows in the negative direction.
3. **Segment Registers with Negative-Growth Support**:
    
    - Example of segment register configuration:
        
        |Segment|Base|Size (max 4K)|Grows Positive?|
        |---|---|---|---|
        |Code|32K|2K|1|
        |Heap|34K|3K|1|
        |Stack|28K|2K|0|
        
4. **Translation of Stack Virtual Addresses**:
    
    - Example: Translating virtual address **15KB** to physical address **27KB**:
        - Virtual address in binary: `11 1100 0000 0000` (hex: `0x3C00`).
        - Top two bits (`11`) identify the **stack segment**.
        - Offset calculation:
            - Offset = Virtual offset (3KB) - Max segment size (4KB) = **-1KB**.
        - Physical address = Base (28KB) + Offset (-1KB) = **27KB**.
5. **Bounds Check for Negative Growth**:
    
    - Ensure the **absolute value** of the negative offset is ≤ the segment’s current size.
    - Example: Offset = `-1KB`, Segment size = `2KB` → Bounds check passes.