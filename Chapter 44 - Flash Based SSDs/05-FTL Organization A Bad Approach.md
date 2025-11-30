### Key Notes for Quick Review: 44.6 FTL Organization: A Bad Approach

1. **Direct-Mapped FTL Overview**:
    
    - Simplest form of Flash Translation Layer (FTL) organization.
    - Logical page N is directly mapped to physical page N for reads.
    - Writes involve:
        - Reading the entire block containing page N.
        - Erasing the block.
        - Reprogramming the block with old and new pages.
2. **Performance Issues**:
    
    - **Write Amplification**:
        - Each write requires reading, erasing, and reprogramming the entire block.
        - Write amplification is proportional to the number of pages in a block.
    - **Severe Write Performance**:
        - Slower than traditional hard drives due to costly operations (read, erase, program).
        - Mechanical seeks and rotational delays in hard drives are faster in comparison.
3. **Reliability Issues**:
    
    - **Rapid Wear-Out**:
        - Repeated overwriting of file system metadata or user data causes the same block to be erased and programmed repeatedly.
        - Leads to rapid wear-out of physical blocks.
    - **Uneven Write Load**:
        - Workloads that do not distribute writes evenly across logical blocks cause popular physical blocks to wear out faster.
    - **Data Loss Risk**:
        - Overuse of specific blocks increases the risk of data loss.
4. **Conclusion**:
    
    - Direct-mapped FTL is not suitable due to:
        - Poor performance (severe write amplification and slow write speeds).
        - High risk of reliability issues (uneven wear-out and data loss).