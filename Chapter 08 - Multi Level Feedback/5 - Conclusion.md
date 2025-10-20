```
• Rule 1: If Priority(A) > Priority(B), A runs (B doesn’t).
• Rule 2: If Priority(A) = Priority(B), A & B run in round-robin fash-
ion using the time slice (quantum length) of the given queue.
• Rule 3: When a job enters the system, it is placed at the highest
priority (the topmost queue).
• Rule 4: Once a job uses up its time allotment at a given level (re-
gardless of how many times it has given up the CPU), its priority is
reduced (i.e., it moves down one queue).
• Rule 5: After some time period S, move all the jobs in the system
to the topmost queue.
```

**MLFQ (Multi-Level Feedback Queue) Summary**

- **MLFQ Rules**:

- Rule 1: Higher priority job runs first

- Rule 2: Equal priority jobs run in round-robin fashion

- Rule 3: New job enters at highest priority

- Rule 4: Job priority reduced after using up time allotment

- Rule 5: Periodic priority boost to topmost queue

- **Key Characteristics**:

- Uses feedback to determine priority

- Observes job execution and prioritizes accordingly

- Achieves best of both worlds: excellent performance for short-running jobs and fairness for long-running workloads

- **Advantages**:

- Doesn't require a priori knowledge of job nature

- Delivers excellent overall performance and fairness

- **Real-world Usage**:

- Used in various operating systems, including:

- BSD UNIX derivatives

- Solaris

- Windows NT and subsequent Windows operating systems