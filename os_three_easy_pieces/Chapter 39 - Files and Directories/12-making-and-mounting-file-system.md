### Key Notes for Quick Review

#### **39.17 Making and Mounting a File System**

1. **Creating a File System:**
    
    - Use the `mkfs` tool (e.g., `mkfs.ext3`) to create a file system.
    - Input: a device (e.g., `/dev/sda1`) and a file system type (e.g., `ext3`).
    - The tool writes an empty file system (starting with a root directory) onto the specified disk partition.
2. **Mounting a File System:**
    
    - Use the `mount` program to make the file system accessible within the directory tree.
    - Command example:
        
        ```bash
        prompt> mount -t ext3 /dev/sda1 /home/users
        ```
        
    - This mounts the file system at `/home/users`.
3. **Accessing the Mounted File System:**
    
    - After mounting, the new file system is accessible as part of the directory tree:
        - Root directory: `/home/users/`
        - Subdirectories: `/home/users/a`, `/home/users/b`
        - Files: `/home/users/a/foo`, `/home/users/b/foo`
4. **Viewing Mounted File Systems:**
    
    - Run the `mount` command to see all mounted file systems and their mount points:
        
        ```
        /dev/sda1 on / type ext3 (rw)
        proc on /proc type proc (rw)
        sysfs on /sys type sysfs (rw)
        /dev/sda5 on /tmp type ext3 (rw)
        /dev/sda7 on /var/vice/cache type ext3 (rw)
        tmpfs on /dev/shm type tmpfs (rw)
        AFS on /afs type afs (rw)
        ```
        
    - Example includes various file systems like `ext3`, `proc`, `tmpfs`, and `AFS`.
5. **Purpose of Mounting:**
    
    - Mounting unifies separate file systems into a single directory tree, making naming uniform and convenient.

---

#### **TOCTTOU Problem (Time Of Check To Time Of Use):**

1. **Definition:**
    
    - A vulnerability where a time gap exists between a validity check and the operation, allowing malicious changes during this interval.
2. **Example Attack:**
    
    - A mail service running as root:
        - Checks a file (e.g., `lstat()` ensures it’s a regular file).
        - Attacker switches the file (e.g., via `rename()`) to a sensitive file like `/etc/passwd` during the gap.
        - The service updates the sensitive file, leading to privilege escalation.
3. **Mitigation Strategies:**
    
    - Reduce services requiring root privileges.
    - Use the `O_NOFOLLOW` flag to prevent symbolic link attacks.
    - Explore transactional file systems (though not widely deployed).

---

#### **39.18 Summary**

1. **File System Interface:**
    
    - The UNIX file system interface is simple but requires practice and deeper understanding to master.
    - Practical usage and further reading (e.g., Stevens [SR05]) are recommended for mastery.
2. **Key Takeaway:**
    
    - The file system unifies multiple file systems into a single tree, simplifying access and management. However, developers must be cautious of vulnerabilities like TOCTTOU when writing privileged code.