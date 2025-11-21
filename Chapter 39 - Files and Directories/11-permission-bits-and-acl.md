### Key Notes on Permission Bits and Access Control Lists (ACLs)

---

#### **Permission Bits**

1. **Overview**:
    
    - Permission bits in UNIX determine who can access a file and how.
    - Permissions are divided into three groups:
        - **Owner**: The user who owns the file.
        - **Group**: Users in the same group as the file.
        - **Others**: All other users.
2. **Viewing Permissions**:
    
    - Use the `ls -l` command to view file permissions.
    - Example:
        
        ```bash
        prompt> ls -l foo.txt
        -rw-r--r-- 1 remzi wheel 0 Aug 24 16:29 foo.txt
        ```
        
    - Breakdown:
        - `-`: File type (`-` for regular file, `d` for directory, `l` for symbolic link).
        - `rw-`: Owner permissions (read and write).
        - `r--`: Group permissions (read-only).
        - `r--`: Others permissions (read-only).
3. **Changing Permissions**:
    
    - Use the `chmod` command to modify permissions.
    - Example:
        
        ```bash
        prompt> chmod 600 foo.txt
        ```
        
    - Explanation:
        - `6`: Owner has read (4) and write (2) permissions (`rw-`).
        - `0`: Group has no permissions (`---`).
        - `0`: Others have no permissions (`---`).
4. **Execute Bit**:
    
    - For regular files, the execute bit determines if a file can be executed as a program.
    - Example:
        
        ```bash
        prompt> chmod 600 hello.csh
        prompt> ./hello.csh
        ./hello.csh: Permission denied.
        ```
        
    - For directories, the execute bit allows users to:
        - Change into the directory (`cd`).
        - Create files (if combined with the writable bit).
5. **Superuser Privileges**:
    
    - The superuser (`root`) can access all files regardless of permissions.
    - In distributed file systems like AFS, a group (e.g., `system:administrators`) may have similar privileges.

---

#### **Access Control Lists (ACLs)**

1. **Overview**:
    
    - ACLs provide a more flexible and detailed way to control access to files and directories compared to traditional permission bits.
    - ACLs allow users to specify exactly who can access a resource and what actions they can perform.
2. **Example in AFS**:
    
    - Use the `fs listacl` command to view ACLs for a directory.
    - Example:
        
        ```bash
        prompt> fs listacl private
        Access list for private is
        Normal rights:
        system:administrators rlidwka
        remzi rlidwka
        ```
        
    - Breakdown:
        - `rlidwka`: Permissions include lookup, insert, delete, administer, read, write, and lock.
3. **Modifying ACLs**:
    
    - Use the `fs setacl` command to modify ACLs.
    - Example:
        
        ```bash
        prompt> fs setacl private/ andrea rl
        ```
        
    - This grants the user `andrea` read (`r`) and lookup (`l`) permissions for the `private` directory.
4. **Comparison to Permission Bits**:
    
    - Permission bits use a simple owner/group/others model.
    - ACLs allow fine-grained control, enabling specific users or groups to have customized access rights.