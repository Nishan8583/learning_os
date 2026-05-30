# 📘 Introduction to Operating System Security — Summary Notes

## 1. Why OS Security Matters

- Every computing activity relies on an operating system; insecurity in the OS undermines everything above it.
    
- OS flaws can let attackers:
    
    - Steal or alter data
        
    - Crash applications
        
    - Misuse hardware resources
        
- Modern OSes are large and complex → higher chance of bugs and vulnerabilities.
    
- OSes must manage **multiple concurrent processes**, many of which are untrusted.
    
- Applications rely on OS abstractions (files, memory, scheduling). If these cannot be trusted, applications cannot achieve their own security goals.
    

---

## 2. What the OS Must Protect

- The OS controls nearly all hardware → it can:
    
    - Read/modify any process memory
        
    - Access/alter any file
        
    - Schedule or halt any process
        
    - Send/alter messages
        
    - Manage devices
        
- A malicious OS can break all security guarantees; therefore OS flaws are extremely dangerous.
    
- **Security enclaves** (TPM, secure enclaves) aim to isolate sensitive operations even from the OS, but they are difficult to design perfectly.
    

---

## 3. Security Goals & Policies

### Core Security Goals

1. **Confidentiality** – Prevent unauthorized disclosure of information.
    
2. **Integrity** – Prevent unauthorized modification; ensure authenticity.
    
3. **Availability** – Ensure resources/services are accessible when needed.
    
4. **Controlled Sharing** – Different permissions for different users/processes.
    
5. **Non-repudiation** – Prevent users from denying their actions.
    

### From Goals → Policies

- High-level goals must become **specific, enforceable security policies**.
    
    - Example: “Users A and B may write file X; no one else may.”
        
- OS provides mechanisms, but administrators/users define the policies.
    

---

## 4. Principles for Designing Secure Systems

(Based on Saltzer & Schroeder)

1. **Economy of mechanism** – Keep designs simple.
    
2. **Fail-safe defaults** – Deny by default; allow only when explicitly permitted.
    
3. **Complete mediation** – Check permissions every time an action occurs.
    
4. **Open design** – Assume attackers know the system design.
    
5. **Separation of privilege** – Require multiple credentials for important actions.
    
6. **Least privilege** – Processes get only the minimum permissions needed.
    
7. **Least common mechanism** – Minimize shared mechanisms; reduce attack surface.
    
8. **Acceptability** – Security must be usable or users won't follow it.
    

---

## 5. Basics of OS Security Mechanisms

### Key Built-In OS Goals

- Separate processes from one another.
    
- Prevent unauthorized access to memory, CPU, files, and devices.
    
- Enforce security in system services like file systems, IPC, and scheduling.
    

### Mechanisms Used

- **Virtual memory**: hardware-enforced separation of process address spaces.
    
- **System calls**:
    
    - Switch CPU to supervisor mode.
        
    - Identify the calling process.
        
    - Enforce access control before performing the operation.
        
- **Device drivers**: mediate hardware access and enforce security checks.
    

---

## 6. Key Takeaways

- OS security failures are catastrophic because the OS has total control.
    
- Security depends on:
    
    - Clear goals (confidentiality, integrity, availability)
        
    - Precise policies
        
    - Strong enforcement mechanisms
        
- Virtualization (processes, memory) is foundational for OS security.
    
- The following chapters will describe specific OS security mechanisms.