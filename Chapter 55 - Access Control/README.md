# Access Control — Summary (Notes)

## 1. Introduction

Access control determines **whether a subject can perform an action on an object**.  
Typical example: a process (subject) calls `open("/var/foo", O_RDWR)` (access) on a file (object).

Two key steps:

1. **Check if request obeys security policy**.
    
2. **Allow or deny the operation**.
    

The OS enforces this through a **reference monitor**, which must be correct and efficient.

---

## 2. Core Concepts

- **Subject** – user or process requesting access
    
- **Object** – resource being accessed (file, device, memory, etc.)
    
- **Access** – operation on the object (read, write, execute)
    

### Design Considerations

- When to check access? (Ideally every time → _complete mediation_)
    
- Efficiency matters: repeated checks add overhead
    
- Virtualization helps reduce checks (e.g., processes freely accessing their own virtual memory)
    

---

## 3. Two Major Access Control Models

### A. **Access Control Lists (ACLs)**

Think **bouncer with a list**.  
Each object has a list of subjects + their allowed operations.

**How ACLs work**:

1. OS identifies the requesting user/process.
    
2. OS retrieves the object’s ACL (usually stored in metadata/inode).
    
3. OS checks if the subject is listed with the required permission.
    

**Unix-style ACLs**:

- Early Unix systems stored ACLs compactly: **9 bits**
    
    - Owner: rwx
        
    - Group: rwx
        
    - Others: rwx
        

**Advantages**

- Easy to see _who_ can access a given object
    
- Easy to change access for an object
    
- Access info stored close to the file → efficient in distributed systems
    

**Disadvantages**

- Hard to answer: “What objects can user X access?”
    
- Long lists can be slow to search (mitigated by Unix design)
    
- Distributed systems require shared identity namespace
    

---

### B. **Capabilities**

Think **keys** (or tickets).  
A capability is an _unforgeable token_ giving permission to access an object.

**How capability systems work**:

- A process has a **capability list** stored in the OS (not user space).
    
- To access an object, the capability must be present.
    
- Transfers of capabilities require OS mediation.
    

**Key challenges**

- Capabilities are just bits → risk of **forgery & copying**
    
- Revocation can be hard if capabilities escape OS control
    
- Capability lists can grow large if many objects exist
    

**Advantages**

- Easy to list all permissions a subject has
    
- Efficient to check (just presence of capability)
    
- Naturally supports least privilege: processes can be given small subsets of capabilities
    

**Disadvantages**

- Hard to determine who can access a given object
    
- Managing capability creation, storage, and revocation is complex
    

---

## 4. Combined Use in Real Systems

Modern OSes often **combine** both models:

- Initial `open()` uses **ACLs**
    
- If allowed, OS creates an **in-memory capability-like object** (file descriptor entry)
    
- Subsequent reads/writes use that capability, avoiding repeated ACL checks
    

This hybrid model improves performance and scalability.

---

## 5. Mandatory vs. Discretionary Access Control

### Discretionary Access Control (DAC)

- Object owner chooses access permissions
    
- Common in Unix/Linux/Windows
    

### Mandatory Access Control (MAC)

- Access controlled by central authority
    
- Users **cannot** override policies (e.g., military “Top Secret”)
    
- Implementations: SELinux, etc.
    

MAC and DAC can coexist.

---

## 6. Practicalities & Real-World Approaches

### Defaults

- Users rarely change access permissions → **good defaults are essential**
    
- Linux uses `umask` to control default file permissions
    

### Android Access Control (Overview)

- Apps each run as a **unique Linux user ID**
    
- Apps declare required permissions (permission labels)
    
- Installation-time permission assignment → resembles **capabilities**
    
- Feels like **mandatory access control** for apps
    
- User understanding of permissions remains a challenge
    

### Role-Based Access Control (RBAC)

Created to manage per-role privileges in organizations.

**Key characteristics**

- Users are assigned **roles** (e.g., Doctor, Pharmacist)
    
- Roles carry privileges
    
- Users may switch roles when performing tasks
    
- Prevents accidental leakage → enforces **least privilege**
    

**Type Enforcement (TE)**

- Finer-grained access (e.g., salespeople can add purchase records but not restocking records)
    

### Privilege Escalation

- Mechanisms like `setuid` and `sudo` temporarily grant elevated privileges
    
- Risky: attackers exploit these paths to gain **root**
    
- Must be used sparingly and carefully
    

---

## 7. Summary

- Access control enforces _who_ can do _what_ to _which_ resources.
    
- Two fundamental mechanisms:
    
    - **ACLs** (lists on objects)
        
    - **Capabilities** (keys possessed by subjects)
        
- Both have strengths/weaknesses; modern OSes often mix them.
    
- Systems may use **discretionary** or **mandatory** control.
    
- Advanced approaches (RBAC, type enforcement) enhance security.
    
- Policy design is as important as mechanism—poor policies cause most failures.