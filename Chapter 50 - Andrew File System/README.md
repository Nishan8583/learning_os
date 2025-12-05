# **Notes: The Andrew File System (AFS) – Full Summary**

## **Overview**

- AFS (Andrew File System) was created at **Carnegie-Mellon University (CMU)** in the 1980s.
    
- Primary goal: **Scalability**—support as many clients per server as possible.
    
- Designed with **predictable and simple cache consistency**—unlike NFS, AFS guarantees that opening a file returns the latest version.
    

---

# **50.1 AFS Version 1 (AFSv1)**

### **Key Characteristics**

- **Whole-file caching** on client local disk:
    
    - On `open()`: entire file fetched and stored locally.
        
    - `read()`/`write()`: operate on **local copy**, no network traffic.
        
    - On `close()`: if modified, entire file sent back via `Store`.
        

### **Client/Server Model**

- Client component: **Venus**
    
- Server component: **Vice**
    

### **AFSv1 Protocol Messages**

(From Figure 50.1)

- `TestAuth`: Check if cached version is still valid.
    
- `GetFileStat`: Fetch metadata.
    
- `Fetch`: Fetch file contents.
    
- `Store`: Send modified file to server.
    
- `SetFileStat`: Update metadata.
    
- `ListDir`: Get directory listing.
    

### **Issues with V1 Behavior**

- Path traversal always done on server.
    
- Directories **not cached**, only files.
    

---

# **50.2 Problems with Version 1**

AFSv1 had poor scalability due to:

### **1. Path Traversal Overhead**

- Client sends entire pathname.
    
- Server performs expensive directory walking for each request.
    
- With many clients, server CPU becomes bottleneck.
    

### **2. Excessive `TestAuth` Messages**

- Clients frequently check if cached copies are still valid.
    
- Similar to excessive `GETATTR` in NFS.
    
- Server spends much time answering “no change.”
    

### **Other (Secondary) Problems**

- **Load imbalance** across servers → fixed later with **volumes** that can migrate.
    
- Server used **one process per client**, causing overhead → later replaced with **threads**.
    

### **Patterson’s Law (Sidebar)**

- Measure before building.
    
- AFS team used extensive measurement to understand scalability issues.
    

---

# **50.3 Improving the Protocol**

### **Goals**

- Reduce server interactions.
    
- Make interactions more efficient.
    
- Thus: increase scalability.
    

This leads to the development of AFSv2.

---

# **50.4 AFS Version 2**

### **Major Innovations**

1. **Callbacks**
    
    - Server promises to notify client when a file it caches is updated.
        
    - Eliminates client polling (`TestAuth`), reducing load drastically.
        
    - Analogous to shifting from polling → interrupts.
        
2. **File Identifiers (FIDs)**
    
    - Replaces pathnames in protocol messages.
        
    - FID = (volume ID, file ID, uniquifier).
        
    - Client performs pathname traversal itself, caching results.
        

### **Directory + File Fetching Example**

Client opens `/home/remzi/notes.txt`:

- Fetch `home` → cache + callback.
    
- Fetch `remzi` → cache + callback.
    
- Fetch `notes.txt` → cache + callback.
    
- Future opens require **no server contact** if callbacks valid.
    

### **Result**

- Frequent accesses become **local**.
    
- Strongly improves scalability and performance.
    

---

# **50.5 Cache Consistency in AFS**

There are two key aspects:

1. **Update visibility**
    
2. **Cache staleness**
    

### **AFS Consistency Model**

#### **Between Different Machines**

- Writes become visible **on close()**.
    
- On close:
    
    - Modified file is flushed to server.
        
    - Server **breaks callbacks** for clients caching the file.
        
    - Clients invalidate and must re-fetch on next open.
        

#### **Between Processes on Same Machine**

- Local writes visible immediately (UNIX semantics).
    

### **Concurrent Writers (Cross-machine)**

- AFS uses **last writer (closer) wins.**
    
- Because entire file is flushed at close, final file = full version from whichever client closed last.
    

### **Comparison With NFS**

- NFS may produce **mixed-block** results due to block-based updates.
    
- AFS always produces a complete file from one writer—more predictable.
    

### **Timeline Example (Figure 50.3)**

Shows:

- Multiple clients reading/writing file `F`.
    
- When data becomes visible.
    
- How callback breaks propagate.
    
- Illustration of last-writer-wins behavior.
    

---

# **50.6 Crash Recovery**

AFS has more complex crash recovery than NFS due to callbacks.

### **Client Crash or Reboot**

- Client may miss callback recalls while down.
    
- On restart, client must treat **entire cache as suspect**.
    
- First access triggers `TestAuth` to validate cached file.
    

### **Server Crash**

- Callback state stored in memory only -> lost on crash.
    
- After restart:
    
    - All clients must check validity of cached files.
        
    - Server or clients use mechanisms like:
        
        - Broadcast warnings
            
        - Heartbeats
            

### **Consequences**

- More recovery overhead than NFS.
    
- But required for scalable caching.
    

---

# **50.7 Scale and Performance of AFSv2**

### **Scalability Improvements**

- AFSv1: ~20 clients/server
    
- AFSv2: **~50 clients/server**
    
- Because clients seldom contact the server after first fetch.
    

### **Performance Analysis (Figure 50.4)**

#### **Key Assumptions**

- `Lmem` < `Ldisk` < `Lnet`
    
- Small/medium files fit in RAM; large files fit in disk but not RAM.
    

### **Observations**

#### **1. First Reads**

- AFS ≈ NFS (network dominates cost).
    

#### **2. Re-reads of Large Files**

- AFS faster: served from local disk.
    
- NFS: must re-fetch from server (memory cache cannot hold large file).
    

#### **3. Sequential Writes**

- Both similar; AFS writes to local cache then flushes at close.
    

#### **4. Overwrites (Entire File Existing)**

- AFS is **worse**:
    
    - Must fetch entire existing file first, then overwrite.
        
- NFS writes only touched blocks.
    

#### **5. Small Accesses to Large Files**

- AFS is **much worse**:
    
    - Whole-file fetch + whole-file store for tiny read/write.
        
- NFS block-based I/O → much cheaper.
    

### **Takeaway**

- AFS excels when:
    
    - Files are accessed sequentially
        
    - Files are re-read many times
        
    - Workloads match “read-mostly” assumptions
        
- AFS performs poorly for:
    
    - Random or small writes within large files
        
    - Overwrites
        
    - Log updates or database-like workloads
        

### **Aside: Importance of Workload**

- AFS designers assumed:
    
    - Files are not frequently shared.
        
    - Access is usually sequential.
        
- But other workloads (logging, random updates) behave poorly.
    

---

# **50.8 Other Improvements in AFS**

### **Global Namespace**

- Same file paths on all clients.
    
- Easier system-wide consistency.
    
- NFS requires manual mount coordination.
    

### **Security**

- Stronger authentication.
    
- Better protection for private data.
    

### **Flexible Access Control**

- Rich user-managed ACLs.
    
- Much more nuanced than UNIX permissions.
    

### **Simplified Administration**

- Tools for:
    
    - Volume management
        
    - Load balancing
        
    - User administration
        

AFS was far ahead of NFS in admin/UI design.

---

# **50.9 Summary**

- AFS introduced:
    
    - **Whole-file caching**
        
    - **Callbacks**
        
    - **File identifiers**
        
    - **Global namespace**
        
    - **Security + ACLs**
        
- These drastically improved:
    
    - Scalability
        
    - Cache consistency clarity
        
    - User experience
        
- Real-world adoption:
    
    - AFS declined as NFS (open standard) dominated.
        
    - Ideas lived on:
        
        - NFSv4 adopts server state, callbacks, and more AFS-like features.