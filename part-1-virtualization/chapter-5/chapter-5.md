# Chapter 5 Process API
### Key Notes: Interlude on Process Creation in UNIX Systems

#### Purpose of Interludes

- Focus on **practical aspects** of systems, particularly **operating system APIs**.
- Practical knowledge is highly valuable for real-world applications and employability.
- This interlude specifically discusses **process creation** in UNIX systems.

#### Process Creation in UNIX

- UNIX uses a **unique approach** to process creation with the following system calls:
    1. **`fork()`**: Creates a new process by duplicating the calling process.
    2. **`exec()`**: Replaces the current process image with a new program.
    3. **`wait()`**: Allows a process to wait for a child process to complete.

#### CRUX: Key Problem

- **How to create and control processes effectively?**
    - The operating system must provide interfaces that:
        - Enable **powerful functionality**.
        - Are **easy to use**.
        - Deliver **high performance**.

### Key Notes: The `fork()` System Call

#### Overview of `fork()`

- **Purpose**: Used to create a new process in UNIX systems.
- **Uniqueness**: It is one of the **strangest routines** you will encounter because:
    - It creates an **almost exact copy** of the calling process.
    - Both parent and child processes continue execution from the point where `fork()` was called.

#### Example Code: `fork()` (p1.c)

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    printf("hello (pid:%d)\n", (int) getpid());
    int rc = fork();
    if (rc < 0) { // fork failed
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) { // child process
        printf("child (pid:%d)\n", (int) getpid());
    } else { // parent process
        printf("parent of %d (pid:%d)\n", rc, (int) getpid());
    }
    return 0;
}
```

#### Key Observations

1. **Process IDs (PIDs)**:
    
    - Each process is assigned a unique **PID**.
    - The parent process can use the PID to manage the child process (e.g., stop it).
2. **Behavior of `fork()`**:
    
    - The **parent process** receives the **PID of the child** as the return value of `fork()`.
    - The **child process** receives a return value of **0**.
    - This differentiation allows the program to handle parent and child processes separately.
3. **Output Example**:
    
    ```
    prompt> ./p1
    hello (pid:29146)
    parent of 29147 (pid:29146)
    child (pid:29147)
    ```
    
4. **Non-Deterministic Execution**:
    
    - The order of execution between the parent and child processes is **not deterministic**.
    - Example of alternate output:
        
        ```
        prompt> ./p1
        hello (pid:29146)
        child (pid:29147)
        parent of 29147 (pid:29146)
        ```
        
    - The **CPU scheduler** determines which process runs first, leading to variability.

#### Example Code: `fork()` with `wait()` (p2.c)

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    printf("hello (pid:%d)\n", (int) getpid());
    int rc = fork();
    if (rc < 0) { // fork failed
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) { // child process
        printf("child (pid:%d)\n", (int) getpid());
    } else { // parent process
        int rc_wait = wait(NULL);
        printf("parent of %d (rc_wait:%d) (pid:%d)\n", rc, rc_wait, (int) getpid());
    }
    return 0;
}
```

#### Key Observations with `wait()`

1. **`wait()` System Call**:
    
    - Used by the parent process to **wait for the child process to complete**.
    - Returns the **PID of the child process** that has finished.
2. **Output Example**:
    
    ```
    prompt> ./p2
    hello (pid:29146)
    child (pid:29147)
    parent of 29147 (rc_wait:29147) (pid:29146)
    ```
    

#### Important Notes

- **Child Process Characteristics**:
    
    - Has its own **address space**, **registers**, and **program counter (PC)**.
    - Starts execution as if it had just called `fork()`.
- **Non-Determinism**:
    
    - The order of execution between parent and child depends on the **CPU scheduler**.
    - This behavior introduces **nondeterminism**, which becomes more complex in multi-threaded programs.
- **Practical Use**:
    
    - `fork()` is a foundational concept for process creation in UNIX.
    - Understanding its behavior is critical for working with **process management** and **concurrent programming**.
The operating system creates a **new process** (called the **child process**) that is an **almost exact replica** of the original process (called the **parent process**). However, the key difference lies in the **return value of `fork()`**, which allows the parent and child processes to execute different parts of the code.

Here’s a **comprehensive explanation**:

---

### **How `fork()` Works**

1. **Process Duplication**:
    
    - When `fork()` is called, the operating system duplicates the calling process.
    - The child process gets its own:
        - **Address space** (a copy of the parent's memory).
        - **Registers**.
        - **Program Counter (PC)**.
    - Both processes (parent and child) continue execution **from the same point** in the code—right after the `fork()` call.
2. **Return Value of `fork()`**:
    
    - The **parent process** receives the **PID of the child process** as the return value of `fork()`.
    - The **child process** receives a return value of **0**.
    - If `fork()` fails (e.g., due to insufficient resources), it returns **-1** to the parent process.
3. **Code Execution**:
    
    - After the `fork()` call, both processes execute the same code, but the **return value of `fork()`** determines which branch of the code each process executes:
        - **Child Process (`rc == 0`)**: Executes the code inside the `if (rc == 0)` block.
        - **Parent Process (`rc > 0`)**: Executes the code inside the `else` block, where `rc` contains the PID of the child.

---

### **Example Walkthrough**

Consider the following code snippet:

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    printf("hello (pid:%d)\n", (int) getpid());
    int rc = fork();
    if (rc < 0) { // fork failed
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) { // child process
        printf("child (pid:%d)\n", (int) getpid());
    } else { // parent process
        printf("parent of %d (pid:%d)\n", rc, (int) getpid());
    }
    return 0;
}
```

#### **Step-by-Step Execution**:

1. **Before `fork()`**:
    
    - The program starts with the `main()` function.
    - The `printf("hello...")` statement is executed, printing the PID of the process.
2. **`fork()` Call**:
    
    - The `fork()` system call is invoked.
    - The operating system creates a **new child process** that is a copy of the parent process.
3. **After `fork()`**:
    
    - Both the parent and child processes continue execution from the point where `fork()` was called.
    - The **return value of `fork()`** determines the execution path:
        - **Child Process (`rc == 0`)**:
            - Executes the code inside the `if (rc == 0)` block.
            - Prints: `"child (pid:...)"`
        - **Parent Process (`rc > 0`)**:
            - Executes the code inside the `else` block.
            - Prints: `"parent of ... (pid:...)"`
4. **Output**:
    
    - The output depends on the **order of execution**, which is determined by the CPU scheduler (non-deterministic). Example outputs:
        
        ```
        hello (pid:29146)
        parent of 29147 (pid:29146)
        child (pid:29147)
        ```
        
        OR
        
        ```
        hello (pid:29146)
        child (pid:29147)
        parent of 29147 (pid:29146)
        ```
        

---

### **Key Takeaways**

- **Replica with Differences**:
    
    - The child process is an almost exact replica of the parent process, but:
        - It has its own memory space.
        - It receives a return value of `0` from `fork()`.
- **Execution Divergence**:
    
    - The return value of `fork()` is used to differentiate between the parent and child processes, allowing them to execute different parts of the code.
- **Non-Determinism**:
    
    - The order in which the parent and child processes execute depends on the CPU scheduler, making the output non-deterministic.

---

### **Why This Design?**

The `fork()` design is powerful because:

1. It allows the parent and child processes to share the same codebase but execute different logic based on the return value.
2. It simplifies process creation by duplicating the current process rather than starting from scratch.
3. It enables flexible workflows, such as:
    - The parent process managing the child (e.g., using `wait()`).
    - The child process performing specific tasks independently.

This behavior is foundational for process management in UNIX systems and is widely used in real-world applications like servers, where processes are forked to handle client requests.


### Key Notes: The `wait()` System Call

#### **Purpose of `wait()`**

- The `wait()` system call is used by a **parent process** to **pause execution** until one of its child processes finishes executing.
- It ensures that the parent process does not proceed until the child process has **terminated**.

---

#### **How `wait()` Works**

1. **Parent-Child Synchronization**:
    
    - When the parent process calls `wait()`, it **blocks** (pauses) until the child process:
        - Completes its execution.
        - Terminates (either normally or abnormally).
    - Once the child process finishes, `wait()` returns control to the parent process.
2. **Return Value**:
    
    - `wait()` returns the **PID of the terminated child process**.
    - This allows the parent to identify which child process has finished (useful when managing multiple children).
3. **Deterministic Output**:
    
    - By using `wait()`, the parent ensures that the child process **always completes first**, making the program's output **deterministic**.

---

#### **Example Code: `wait()` in Action (p2.c)**

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    printf("hello (pid:%d)\n", (int) getpid());
    int rc = fork();
    if (rc < 0) { // fork failed
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) { // child process
        printf("child (pid:%d)\n", (int) getpid());
    } else { // parent process
        int rc_wait = wait(NULL); // parent waits for child
        printf("parent of %d (rc_wait:%d) (pid:%d)\n", rc, rc_wait, (int) getpid());
    }
    return 0;
}
```

---

#### **Key Observations**

1. **Output Without `wait()`**:
    
    - Without `wait()`, the output is **non-deterministic** because the parent and child processes execute independently.
    - Example outputs:
        
        ```
        hello (pid:29266)
        parent of 29267 (pid:29266)
        child (pid:29267)
        ```
        
        OR
        
        ```
        hello (pid:29266)
        child (pid:29267)
        parent of 29267 (pid:29266)
        ```
        
2. **Output With `wait()`**:
    
    - With `wait()`, the parent process **always waits** for the child to finish, ensuring **deterministic output**:
        
        ```
        hello (pid:29266)
        child (pid:29267)
        parent of 29267 (rc_wait:29267) (pid:29266)
        ```
        
3. **Why Deterministic?**
    
    - If the child process runs first, it prints its message and exits, allowing the parent to proceed.
    - If the parent process runs first, it immediately calls `wait()` and blocks until the child finishes. Once the child exits, `wait()` returns, and the parent prints its message.

---

#### **Key Use Cases of `wait()`**

1. **Parent-Child Coordination**:
    
    - Ensures that the parent process waits for the child to complete before proceeding.
    - Useful in scenarios where the parent depends on the child's results or actions.
2. **Resource Management**:
    
    - Prevents **zombie processes**:
        - When a child process terminates, its **exit status** remains in the system until the parent retrieves it using `wait()`.
        - If the parent does not call `wait()`, the child process becomes a **zombie** (a process that has terminated but still occupies system resources).
3. **Error Handling**:
    
    - The parent can retrieve the **exit status** of the child process using `wait()` or its sibling `waitpid()`, allowing it to handle errors or abnormal terminations.

---

#### **Advanced Version: `waitpid()`**

- `waitpid()` is a more flexible version of `wait()`:
    - Allows the parent to wait for a **specific child process** by specifying its PID.
    - Can be used with options to **poll** or **non-block** while waiting.


### Key Notes on `exec()` System Call

1. **Purpose of `exec()`**:
    
    - Used to run a program different from the calling program.
    - Transforms the currently running process into a new program.
2. **Behavior**:
    
    - Loads the code and static data of the specified executable into the current process.
    - Overwrites the current code segment and static data.
    - Reinitializes the heap, stack, and other memory segments.
    - Does **not** create a new process; it replaces the current process.
3. **Execution Flow**:
    
    - After a successful `exec()` call, the original program ceases to exist in memory.
    - The new program starts execution, and the `exec()` call does not return.
4. **Example Usage**:
    
    - The child process created by `fork()` can call `exec()` to run a different program.
    - Example program (`p3.c`):
        - Parent process prints its PID and waits for the child.
        - Child process calls `execvp()` to run the `wc` (word count) program on a file (`p3.c`).
    - Output:
        
        ```
        hello (pid:29383)
        child (pid:29384)
        29 107 1030 p3.c
        parent of 29384 (rc_wait:29384) (pid:29383)
        ```
        
5. **Variants of `exec()`**:
    
    - Linux provides six variants: `execl()`, `execlp()`, `execle()`, `execv()`, `execvp()`, and `execvpe()`.
    - Each variant differs in how arguments and environment variables are passed. Refer to the man pages for details.
6. **Key Insights**:
    
    - `fork()` and `exec()` together form a powerful and simple API for process creation and execution.
    - `exec()` is particularly useful when the child process needs to run a completely different program.
7. **Design Philosophy**:
    
    - Lampson’s Law: "Get it right." The combination of `fork()` and `exec()` is an example of a well-designed API that is both simple and powerful.
8. **Important Notes**:
    
    - A successful `exec()` call never returns.
    - The process calling `exec()` is replaced entirely by the new program.

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
  printf("hello (pid:%d)\n", (int) getpid());
  int rc = fork();
  if (rc < 0) { // fork failed; exit
    fprintf(stderr, "fork failed\n");
    exit(1);
  } else if (rc == 0) { // child (new process)
    printf("child (pid:%d)\n", (int) getpid());
    char *myargs[3];
    myargs[0] = strdup("wc"); // program: "wc"
    myargs[1] = strdup("p3.c"); // arg: input file
    myargs[2] = NULL; // mark end of array
    execvp(myargs[0], myargs); // runs word count
    printf("this shouldn’t print out");
  } else { // parent goes down this path
    int rc_wait = wait(NULL);
    printf("parent of %d (rc_wait:%d) (pid:%d)\n",
    rc, rc_wait, (int) getpid());
  }
  return 0;
 }
```

### Why `exec()` Overwrites the Current Process Instead of Creating a New One

1. **Separation of Concerns**:
    
    - The UNIX process creation model is built around the combination of `fork()` and `exec()`.
    - `fork()` is responsible for creating a new process by duplicating the calling process.
    - `exec()` is responsible for transforming the newly created process into a different program.
    - This separation allows for flexibility, such as setting up the child process (e.g., redirecting input/output, setting environment variables) before replacing it with the new program.
2. **Efficiency**:
    
    - By overwriting the current process, `exec()` avoids the overhead of creating a completely new process from scratch.
    - The process structure (e.g., PID, open file descriptors, environment variables) remains intact, which is often desirable when transitioning to a new program.
    - However, the process name is updated.
3. **Memory Management**:
    
    - When `exec()` is called, the operating system replaces the code segment and static data of the current process with that of the new program.
    - The heap, stack, and other memory regions are reinitialized to ensure the new program starts with a clean slate.
    - This ensures that no remnants of the old program interfere with the execution of the new one.
4. **Behavior of `exec()`**:
    
    - After a successful `exec()` call, the original program ceases to exist in memory.
    - The process is now running the new program, and the `exec()` call does **not** return unless there is an error.
    - This behavior makes it seem as though the process has "morphed" into the new program.
5. **Design Philosophy**:
    
    - The UNIX designers opted for simplicity and power in their API. The combination of `fork()` and `exec()` allows for a wide range of use cases while keeping the system calls themselves straightforward.
    - Lampson’s Law ("Get it right") highlights the elegance of this design: `fork()` and `exec()` together provide a robust mechanism for process creation and execution.
6. **Practical Example**:
    
    - Consider a shell program. When you type a command (e.g., `ls`), the shell:
        1. Calls `fork()` to create a new process.
        2. In the child process, calls `exec()` to replace the shell program with the `ls` program.
        3. The parent process (the shell) waits for the child to complete.
    - This approach ensures that the shell itself is not replaced, allowing it to continue accepting new commands.
7. **Why Not Create a New Process in `exec()`?**:
    
    - If `exec()` created a new process, it would duplicate functionality already provided by `fork()`.
    - Additionally, it would complicate scenarios where the parent process needs to control or interact with the child process before it starts executing the new program.
