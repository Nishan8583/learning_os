
### Key Notes: 36.8 Case Study: A Simple IDE Disk Driver

#### Overview:

- The case study explores the protocol of an IDE disk drive and its implementation in the `xv6` operating system.
- The IDE disk provides a simple interface to the system, consisting of four types of registers: **control**, **command block**, **status**, and **error**.
- Registers are accessed via specific I/O addresses using x86 `in` and `out` instructions.

---

#### **Registers and Their Functions:**

1. **Control Register:**
    
    - **Address:** `0x3F6 = 0x08 (0000 1RE0)`
    - **Bits:**
        - `R`: Reset
        - `E=0`: Enable interrupt
2. **Command Block Registers:**
    
    - **Address 0x1F0:** Data Port
    - **Address 0x1F1:** Error
    - **Address 0x1F2:** Sector Count
    - **Address 0x1F3:** LBA (Logical Block Address) low byte
    - **Address 0x1F4:** LBA mid byte
    - **Address 0x1F5:** LBA high byte
    - **Address 0x1F6:** `1B1D TOP4LBA` (B=LBA, D=drive)
    - **Address 0x1F7:** Command/Status
3. **Status Register (Address 0x1F7):**
    
    - **Bits:**
        - `7`: BUSY
        - `6`: READY
        - `5`: FAULT
        - `4`: SEEK
        - `3`: DRQ (Drive Request for Data)
        - `2`: CORR
        - `1`: IDDEX
        - `0`: ERROR
4. **Error Register (Address 0x1F1):**
    
    - **Bits:**
        - `7`: BBK (Bad Block)
        - `6`: UNC (Uncorrectable Data Error)
        - `5`: MC (Media Changed)
        - `4`: IDNF (ID Mark Not Found)
        - `3`: MCR (Media Change Requested)
        - `2`: ABRT (Command Aborted)
        - `1`: T0NF (Track 0 Not Found)
        - `0`: AMNF (Address Mark Not Found)

#### Key Functions in `xv6` IDE Driver

1. **`ide_rw()`**:
    - Queues a request or issues it directly to the disk using `ide_start_request()`.
    - Waits for the request to complete, putting the calling process to sleep.
2. **`ide_start_request()`**:
    - Sends a request (and data for writes) to the disk.
    - Uses x86 `in` and `out` instructions to read/write device registers.
3. **`ide_wait_ready()`**:
    - Ensures the drive is ready before issuing a request.
4. **`ide_intr()`**:
    - Invoked during an interrupt.
    - Reads data from the device (for reads).
    - Wakes the process waiting for I/O completion.
    - Launches the next I/O request in the queue, if any.

#### Summary of IDE Disk Functionality

- The IDE interface operates through specific I/O addresses and registers.
- The protocol involves waiting for readiness, writing parameters, initiating I/O, handling data transfer, managing interrupts, and error handling.
- The `xv6` IDE driver implements this protocol through four main functions: `ide_rw()`, `ide_start_request()`, `ide_wait_ready()`, and `ide_intr()`.