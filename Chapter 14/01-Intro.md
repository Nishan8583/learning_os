NOTE: I have experience with C library calls, so not digging into it in much detail.
### Key Notes: Memory API in UNIX Systems

#### CRUX: How to Allocate and Manage Memory

- Understanding memory allocation and management is critical for building robust and reliable software.
- Two main types of memory in C programs: **stack memory** and **heap memory**.

---

#### 1. **Stack Memory (Automatic Memory)**

- **Managed implicitly** by the compiler.
- Allocated and deallocated automatically during function calls.
- Example:
    
    ```c
    void func() {
        int x; // declares an integer on the stack
    }
    ```
    
- **Key Characteristics**:
    - Memory is temporary and tied to the function's scope.
    - Deallocated when the function returns.
    - **Limitation**: Not suitable for data that needs to persist beyond the function's execution.

---

#### 2. **Heap Memory (Explicit Memory)**

- **Managed explicitly** by the programmer.
- Allocated using functions like `malloc()` and deallocated using `free()`.
- Example:
    
    ```c
    void func() {
        int *x = (int *) malloc(sizeof(int)); // allocates memory on the heap
    }
    ```
    
- **Key Characteristics**:
    - Memory persists until explicitly freed by the programmer.
    - Offers flexibility for long-lived data.
    - **Challenges**: Prone to bugs such as memory leaks and dangling pointers if not managed carefully.

---

#### 3. **Comparison: Stack vs. Heap**

|**Aspect**|**Stack Memory**|**Heap Memory**|
|---|---|---|
|**Management**|Implicit (handled by compiler)|Explicit (handled by programmer)|
|**Lifetime**|Limited to function scope|Persistent until freed|
|**Ease of Use**|Simple|Complex, prone to errors|
|**Use Case**|Temporary, short-lived data|Long-lived, dynamic data|

---

#### 4. **Common Mistakes to Avoid**

- Forgetting to free heap memory, leading to **memory leaks**.
- Using freed memory, resulting in **dangling pointers**.
- Allocating memory without checking for `NULL` (allocation failure).

---

#### 5. **Key Takeaways**

- Use **stack memory** for temporary, short-lived data.
- Use **heap memory** for dynamic, long-lived data but manage it carefully.
- Always pair `malloc()` with `free()` to avoid memory leaks.
- Be cautious with pointers to avoid undefined behavior.