### Key Notes on Basic Flash Operations

#### 1. **Flash Operations Overview**

- Flash memory supports three primary low-level operations:
    - **Read (a page):**
        - Reads a page (e.g., 2KB or 4KB) by specifying the read command and page number.
        - Fast operation: ~10s of microseconds.
        - Random access device: Uniformly quick access regardless of location.
    - **Erase (a block):**
        - Required before writing to a page.
        - Erases the entire block containing the page, setting all bits in the block to `1`.
        - Destroys all data in the block, so data must be backed up before erasing.
        - Expensive operation: Takes a few milliseconds.
    - **Program (a page):**
        - Changes some `1`s in a page to `0`s and writes the desired content.
        - Requires the block to be erased first.
        - Less expensive than erasing but more costly than reading: ~100s of microseconds.

#### 2. **Page States**

- Pages in flash memory have states:
    - **Invalid (i):** Initial state of pages in a block.
    - **Erased (E):** After erasing a block, all pages are reset and ready to be programmed.
    - **Valid (V):** After programming a page, its state is set to valid.
- **State Transitions Example:**
    
    ```
    Initial: iiii   (All pages invalid)
    Erase() → EEEE   (All pages erased and ready for programming)
    Program(0) → VEEE   (Page 0 programmed; state set to valid)
    Program(0) → error   (Cannot re-program a page after programming)
    Program(1) → VVEE   (Page 1 programmed; state set to valid)
    Erase() → EEEE   (All pages erased; contents lost; ready for programming)
    ```
    

#### 3. **Detailed Example**

- **Initial State:**
    
    ```
    Page 0: 77733777 VALID
    Page 1: 33773337 VALID
    Page 2: 44444449 VALID
    Page 3: 44999999 VALID
    ```
    
- **After Erase:**
    
    ```
    Page 0: 00000000 ERASED
    Page 1: 00000000 ERASED
    Page 2: 00000000 ERASED
    Page 3: 00000000 ERASED
    ```
    
- **After Programming Page 0:**
    
    ```
    Page 0: 00000011 VALID
    Page 1: 00000000 ERASED
    Page 2: 00000000 ERASED
    Page 3: 00000000 ERASED
    ```
    
- **Key Takeaway:**
    - Erasing a block removes all data in the block. Before overwriting any page, data from other pages in the block must be moved elsewhere to avoid data loss.

#### 4. **Performance Characteristics**

- Flash chips offer excellent random read performance compared to traditional disk drives due to the absence of mechanical seek and rotation delays.
- Writing is more complex and expensive due to the erase-before-write requirement.
- Frequent program/erase cycles can lead to **wear out**, which is a major reliability concern for flash memory.

#### 5. **Raw Flash Performance Characteristics**

|**Device Type**|**Read (µs)**|**Program (µs)**|**Erase (µs)**|
|---|---|---|---|
|**SLC**|25|200-300|1500-2000|
|**MLC**|50|600-900|~3000|
|**TLC**|~75|~900-1350|~4500|

#### 6. **Summary**

- **Reading:** Fast and efficient, with uniform access time.
- **Writing:** Requires erasing the entire block first, which is time-consuming and can lead to wear out.
- **Design Considerations:**
    - Data must be backed up before erasing a block.
    - Performance and reliability of writing are critical factors in flash-based storage system design.