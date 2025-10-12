### Key Notes on "Support for Sharing"

1. **Purpose of Sharing**:
    
    - Sharing memory segments between address spaces can save memory.
    - **Code sharing** is a common use case, allowing multiple processes to share the same code segment.
2. **Hardware Support for Sharing**:
    
    - **Protection bits** are introduced to control access to memory segments.
    - Protection bits specify whether a segment can be:
        - **Read**
        - **Written**
        - **Executed**
3. **Benefits of Code Sharing**:
    
    - By marking a code segment as **read-only**, the same code can be shared across multiple processes.
    - This ensures isolation:
        - Each process believes it is accessing its own private memory.
        - The OS secretly shares the memory, which cannot be modified by the process.
4. **Example of Segment Register with Protection**:
    
    - Example configuration with protection bits:
        
        |Segment|Base|Size (max 4K)|Grows Positive?|Protection|
        |---|---|---|---|---|
        |Code|32K|2K|1|Read-Execute|
        |Heap|34K|3K|1|Read-Write|
        |Stack|28K|2K|0|Read-Write|
        
5. **Hardware Algorithm Updates**:
    
    - In addition to bounds checking, the hardware must verify access permissions:
        - **Write to a read-only segment** → Exception raised.
        - **Execute from a non-executable segment** → Exception raised.
    - The OS handles exceptions by managing the offending process.
6. **Preservation of Illusion**:
    
    - Processes remain unaware of shared memory due to enforced protection.
    - The OS ensures that shared memory segments are immutable (e.g., read-only code), maintaining process isolation.