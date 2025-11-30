### Key Notes for Quick Review: **43.3 How Much To Buffer?**

1. **Core Question**:
    
    - How many updates should Log-Structured File Systems (LFS) buffer before writing to disk?
    - The answer depends on the disk's **positioning overhead** (seek + rotation time) and **transfer rate**.
2. **Key Variables**:
    
    - **Tposition**: Time for disk positioning (seek + rotation overhead).
    - **Rpeak**: Peak disk transfer rate (in MB/s).
    - **D**: Amount of data to buffer (in MB).
    - **Twrite**: Total time to write data to disk.
    - **Reffective**: Effective write rate (data written / total time).
    - **F**: Desired fraction of peak rate (e.g., 0.9 for 90% of peak rate).
3. **Time to Write (Twrite)**:  
    The total time to write a chunk of data (D MB) is:
    
    ```
    Twrite = Tposition + (D / Rpeak)
    ```
    
4. **Effective Write Rate (Reffective)**:  
    The effective rate of writing is:
    
    ```
    Reffective = D / Twrite = D / (Tposition + (D / Rpeak))
    ```
    
5. **Goal**:
    
    - Achieve an effective rate (Reffective) that is a fraction (F) of the peak rate (Rpeak):
        
        ```
        Reffective = F × Rpeak
        ```
        
6. **Solving for Buffer Size (D)**:  
    Rearrange the equation to calculate the required buffer size (D):
    
    ```
    D = (F / (1 - F)) × Rpeak × Tposition
    ```
    
7. **Example Calculation**:
    
    - **Disk Parameters**:
        - Positioning time (**Tposition**) = 10 ms = 0.01 seconds.
        - Peak transfer rate (**Rpeak**) = 100 MB/s.
    - **Desired Effective Bandwidth**:
        - **F = 0.9** (90% of peak rate).
    - **Buffer Size (D)**:
        
        ```
        D = (0.9 / (1 - 0.9)) × 100 MB/s × 0.01 seconds  
          = 9 MB
        ```
        
8. **Further Exploration**:
    
    - To achieve **95% of peak rate (F = 0.95)**:
        
        ```
        D = (0.95 / (1 - 0.95)) × 100 MB/s × 0.01 seconds  
          = 19 MB
        ```
        
    - To achieve **99% of peak rate (F = 0.99)**:
        
        ```
        D = (0.99 / (1 - 0.99)) × 100 MB/s × 0.01 seconds  
          = 99 MB
        ```
        
9. **Conclusion**:
    
    - The larger the buffer size (D), the closer the effective write rate (Reffective) gets to the peak transfer rate (Rpeak).
    - To achieve higher fractions of the peak rate (e.g., 95%, 99%), significantly larger buffers are required.