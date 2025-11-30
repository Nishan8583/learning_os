### Key Notes on Detecting Corruption: The Checksum

#### **1. Preserving Data Integrity Despite Corruption**

- **Problem**: Silent failures due to data corruption can lead to disks returning bad data.
- **Solution**: Use detection techniques to identify corrupted data and recover it using a non-corrupt copy.
- **Primary Mechanism**: **Checksum** - A small summary of data (e.g., 4 or 8 bytes) computed using a function over the data (e.g., 4KB block). The checksum is stored with the data and verified during access to detect corruption.

#### **2. Key Concept: "There’s No Free Lunch"**

- Stronger checksum functions provide better data integrity but are computationally more expensive.
- Trade-off: Protection strength vs. computational speed.

#### **3. Common Checksum Functions**

- **XOR Checksum**:
    
    - Simple and fast.
    - Computed by XOR’ing each chunk of the data block.
    - Example:
        - Data block (16 bytes in hex): `365e c4cd ba14 8a92 ecef 2c3a 40be f666`
        - XOR each column (in binary) to compute the checksum.
        - Resulting checksum (in hex): `0x201b9403`.
```
365e c4cd ba14 8a92 ecef 2c3a 40be f666
```
If we view them in binary, we get the following:

```
0011 0110 0101 1110     1100 0100 1100 1101

1011 1010 0001 0100     1000 1010 1001 0010

1110 1100 1110 1111     0010 1100 0011 1010

0100 0000 1011 1110     1111 0110 0110 0110
```

```
0010 0000 0001 1011     1001 0100 0000 0011
```
    
	
- **Limitation**: Cannot detect errors if two bits in the same position change.
- **Addition Checksum**:
    
    - Uses 2’s-complement addition over data chunks, ignoring overflow.
    - Fast but less effective for detecting shifted data.
- **Fletcher Checksum**:
    
    - Computes two check bytes, `s1` and `s2`:
        - `s1 = (s1 + di) mod 255` (over all data bytes `di`).
        - `s2 = (s2 + s1) mod 255` (over all data bytes `di`).
    - Detects single-bit, double-bit, and many burst errors.
    - Stronger than XOR and addition checksums.
- **Cyclic Redundancy Check (CRC)**:
    
    - Treats the data block as a large binary number and divides it by a predefined value (`k`).
    - The remainder of the division is the CRC value.
    - Efficient and widely used in networking.
    - Stronger than XOR, addition, and Fletcher checksums.

#### **4. Limitations of Checksums**

- **Collisions**: Two different data blocks can produce the same checksum.
- **Reason**: Checksums summarize large data (e.g., 4KB) into a much smaller value (e.g., 4 or 8 bytes).
- **Goal**: Minimize the chance of collisions while maintaining computational efficiency.
---
### Key Notes on Checksum Layout and Usage

#### **Checksum Layout**

1. **Basic Checksum Storage**:
    
    - A checksum is stored with each disk sector (or block).
    - For a data block `D`, the checksum is denoted as `C(D)`.
    - Without checksums: `D0 D1 D2 D3 D4 D5 D6`.
    - With checksums: `C[D0] D0 C[D1] D1 C[D2] D2 C[D3] D3 C[D4] D4`.
2. **Sector Size and Checksum Storage**:
    
    - Checksum size is typically small (e.g., 8 bytes).
    - Disks write in sector-sized chunks (e.g., 512 bytes or multiples).
    - **Solution 1**: Use 520-byte sectors (extra 8 bytes for checksum).
    - **Solution 2**: Store checksums in separate sectors:
        
        ```
        C[D0] C[D1] C[D2] C[D3] C[D4]       D0 D1 D2 D3 D4
        ```
        
        - This method works on all disks but is less efficient:
            - Overwriting a block (e.g., `D1`) requires:
                - Reading the checksum sector containing `C(D1)`.
                - Updating `C(D1)` in the checksum sector.
                - Writing the updated checksum sector and the new data block `D1`.
            - Results in **1 read and 2 writes**.
        - The first approach (1 checksum per sector) requires only **1 write**.

---

#### **Using Checksums**

1. **Checksum Verification**:
    
    - When reading a block `D`:
        - The client (file system or storage controller) reads the stored checksum `Cs(D)` from disk.
        - The client computes the checksum over the retrieved block `D`, denoted as `Cc(D)`.
        - The client compares the stored checksum (`Cs(D)`) with the computed checksum (`Cc(D)`):
            - If `Cs(D) == Cc(D)`: Data is likely not corrupted and can be safely returned.
            - If `Cs(D) != Cc(D)`: Data has been corrupted since it was stored.
2. **Handling Corruption**:
    
    - If corruption is detected:
        - **Redundant Copy Available**: Use the redundant copy to recover the data.
        - **No Redundant Copy**: Return an error, as the data cannot be recovered.
    - **Key Insight**: Corruption detection is not a "magic bullet." If no non-corrupted data exists, recovery is impossible.