# Distributed Systems — Summary Notes

## 1. Introduction

Distributed systems power modern services (Google, Facebook, etc.) by coordinating **thousands of machines**.  
Key challenge: **components fail regularly**, but the _system_ should appear reliable.

**Core problem:**

> _How do we build systems that work even when their components fail?_

Additional concerns:

- **Unreliable communication**
    
- **Performance** (minimize messages, latency, maximize bandwidth)
    
- **Security** (authentication, confidentiality, integrity)
    

---

## 2. Communication Basics

Networking is **inherently unreliable**:

- Packet loss (buffer overflows, congestion)
    
- Packet corruption
    
- Hardware failures
    
- Broken links or unresponsive hosts
    

Even with perfect hardware, **router or host buffers overflow → packet drop**.

Thus, all communication protocols must handle:

- Loss
    
- Corruption
    
- Reordering
    

---

## 3. Unreliable Communication: UDP

UDP provides:

- Simple datagram delivery
    
- No guarantees of delivery, order, or deduplication
    
- Checksums for corruption detection
    

Used when applications handle failures themselves (end-to-end argument).

Key takeaway: UDP = fast, simple, **unreliable**.

---

## 4. Building Reliability

Reliable communication must overcome unreliable networks.

### 4.1 Acknowledgments (ACKs)

Sender sends message → receiver replies with ACK.  
Confirms delivery (**but doesn’t handle loss**).

### 4.2 Timeout + Retry (Timeout/Retry)

Sender sets a timer:

- If no ACK arrives → _assume loss_ → resend.
    

This requires:

- Saving a copy of the message
    
- Retrying until ACK arrives
    

### 4.3 Duplicate Messages

Problems arise if:

- ACK is lost
    
- Message is resent  
    → Receiver gets duplicates.
    

### 4.4 Sequence Numbers

Solution:

- Sender attaches **sequence numbers**
    
- Receiver tracks expected number
    
- If duplicate arrives → ACK but **do not deliver twice**  
    → supports exactly-once (ideal) or at-most-once semantics.
    

### 4.5 Timeout Tuning

- Too small → needless retransmissions
    
- Too large → latency increases
    

**Exponential backoff** helps avoid congestion collapse.

---

## 5. Communication Abstractions

### 5.1 Distributed Shared Memory (DSM)

- Makes distributed processes appear to share memory pages
    
- Uses VM system for page fetching
    
- Problems:
    
    - Hard failure semantics (missing pages after node failure)
        
    - Poor performance (remote page faults)  
        → Not widely used today.
        

---

## 6. Remote Procedure Call (RPC)

RPC abstracts remote calls so they look like **local function calls**.

### Components:

#### 6.1 Stub Generator

Generates:

- **Client stubs** (marshaling, sending messages, waiting for replies)
    
- **Server stubs** (receiving messages, unmarshaling, calling functions)
    

Steps in client stub:

1. Build message buffer
    
2. Marshal arguments
    
3. Send to server
    
4. Wait for reply
    
5. Unmarshal response
    
6. Return to caller
    

Server stub:

1. Unmarshal request
    
2. Execute function
    
3. Marshal result
    
4. Send reply
    

### 6.2 Handling Complex Arguments

Pointers require:

- Known data types, or
    
- Annotations for serialization
    

### 6.3 Server Concurrency

Often implemented with:

- **Thread pools**  
    → Prevent blocking during long RPC operations.
    

---

## 7. RPC Runtime Challenges

### 7.1 Naming

Clients need to locate services: hostname + port.  
DNS and other naming systems solve global lookup.

### 7.2 Transport Choice: TCP vs. UDP

- **TCP:** simplifies reliability but adds extra ACK messages → inefficient for RPC.
    
- **UDP:** more efficient, but requires reliability implemented at RPC layer.
    

Many RPC systems choose **UDP + custom reliability**.

### 7.3 Handling Long-Running Calls

Risk: long execution looks like failure → needless retries.  
Solution: server sends:

- Immediate ACK-of-receipt
    
- Progress notifications (client polls for completion)
    

### 7.4 Fragmentation & Reassembly

Large messages must be:

- Split into fragments on send
    
- Reassembled at receiver  
    Handled by network or RPC layer.
    

### 7.5 Endianness

Different machines store integers differently:

- Big-endian vs little-endian  
    RPC uses a standardized representation (e.g., XDR).
    

### 7.6 Asynchronous RPC

Allows:

- Client to send request without waiting
    
- Later retrieve results  
    Improves parallelism and performance.
    

---

## 8. End-to-End Argument (Aside)

Key principle:

- Some correctness properties **must be verified at the highest layer**, not by lower layers.
    

Example: reliable file transfer  
→ Must verify full file correctness at application layer (checksum at end), regardless of network reliability.

---

## 9. Summary

Distributed systems:

- Must tolerate regular component failures
    
- Rely on communication abstractions (messaging, RPC) for coordination
    

RPC provides:

- A familiar programming interface
    
- Reliability on top of unreliable networks
    
- Tools like stub generators, runtime libraries, marshaling, naming, and message fragmentation
    

Modern systems (gRPC, Thrift) build on this foundation.

---

## 10. Homework Tasks (from chapter)

1. Build UDP client/server
    
2. Convert to communication library
    
3. Add reliability (timeout/retry, ACKs)
    
4. Support very large messages
    
5. Improve performance (parallel fragments)
    
6. Add asynchronous, in-order delivery
    
7. Measure latency and bandwidth