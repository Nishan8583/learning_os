# **Sun’s Network File System (NFS) — Summary Notes**

## **1. Overview**

- NFS is a classic distributed file system allowing clients to access files stored on a remote server **transparently**.
    
- Clients see the same file interface as a local filesystem (open/read/write/close).
    
- Motivation:
    
    - Easy data sharing across multiple clients.
        
    - Centralized admin + backups.
        
    - Security benefits (centralized server storage).
        

---

## **2. Design Goal: Fast Server Crash Recovery**

- NFSv2 focuses on **simple and fast crash recovery**.
    
- Server crashes affect all clients → reducing recovery time is critical.
    

---

## **3. Stateless Protocol**

- Core strategy: **server maintains no per-client state**.
    
- Server does _not_ track:
    
    - Open files
        
    - File descriptors
        
    - Client caches
        
    - File pointers
        
- Each client request contains **all information needed**.
    

### Why avoid state?

- Stateful server complicates recovery:
    
    - After crash, server forgets file descriptors, client cache mappings, etc.
        
    - Requires complex recovery protocols.
        

---

## **4. File Handles**

- NFS operates using **file handles**, not file descriptors.
    
- A file handle includes:
    
    - **Volume ID**
        
    - **Inode number**
        
    - **Generation number** (prevents stale handles)
        
- Used in all READ/WRITE/LOOKUP operations.
    

---

## **5. NFSv2 Protocol Operations (Key Examples)**

|Operation|Purpose|
|---|---|
|**LOOKUP**|Resolve name → get file handle + attributes|
|**GETATTR**|Fetch metadata (size, timestamps, owner)|
|**READ**|Read file data using file handle + offset + count|
|**WRITE**|Write data; must provide exact offset|
|**CREATE/MKDIR/REMOVE/RMDIR**|Create/remove files and directories|
|**READDIR**|Read directory entries|

---

## **6. Idempotent Operations**

### Why idempotency matters

- Clients resend requests after timeouts.
    
- Many failure modes (lost request, server down, lost reply) are handled uniformly via **retry**.
    

### Idempotent Ops

- **LOOKUP**, **READ**: naturally idempotent (read-only).
    
- **WRITE**: idempotent because offset + data are included; repeating Rewrite yields same result.
    

### Non-idempotent example

- **MKDIR**: This may fail on retry if directory was created but reply was lost.
    

---

## **7. Client-Side Caching**

Caching significantly improves performance:

- First access → network.
    
- Subsequent accesses → client memory.
    

### Write buffering

- Writes are cached locally before being sent to the server.
    
- Improves application write latency.
    

---

## **8. Cache Consistency Problem**

Multiple clients with caches cause consistency issues:

### Two sub-problems:

1. **Update visibility**
    
    - C2 writes but buffers changes; C3 sees old data.
        
2. **Stale caches**
    
    - Server has new version but C1 still serves old version from cache.
        

---

## **9. NFSv2 Solutions to Cache Consistency**

### A. Flush-on-close (Close-to-open consistency)

- When a file is closed, client **pushes all writes to server**.
    
- Ensures next opener sees up-to-date contents.
    

### B. Attribute cache

- Before reading a cached file, client performs **GETATTR**.
    
- If modification time changed → invalidate cache.
    
- To reduce GETATTR traffic:
    
    - Clients cache attributes for ~3 seconds.
        

### Downsides

- Hard to reason about consistency.
    
- Occasional stale data behavior.
    

---

## **10. Server-Side Write Buffering**

### Must force writes to persistent storage before replying.

- Prevents inconsistent file contents after a crash.
    
- Without this, server might acknowledge a write that never reached disk.
    

### Performance note

- Write-back caching is expensive → server write performance often the bottleneck.
    
- Optimizations (e.g., battery-backed NVRAM, log-structured filesystems) used to accelerate stable writes.
    

---

## **11. VFS/Vnode Interface**

- Introduced by Sun to integrate NFS cleanly into UNIX.
    
- **VFS (Virtual File System)**: filesystem-wide operations.
    
- **Vnode layer**: per-file operations.
    
- Allows multiple filesystem types to coexist.
    

---

## **12. Summary Points**

- NFSv2’s stateless protocol enables simple, robust crash recovery.
    
- Idempotency is essential for client retry handling.
    
- Client caching improves performance but complicates consistency.
    
- Flush-on-close and attribute caching provide practical but imperfect solutions.
    
- Servers must commit writes to disk before acknowledging.
    
- VFS/vnode architecture was a long-lasting byproduct of NFS.