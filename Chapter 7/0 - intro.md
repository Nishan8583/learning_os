### Key Notes: Scheduling: Introduction

1. **Low-Level Mechanisms vs. High-Level Policies**:
    
    - Low-level mechanisms like context switching should already be understood.
    - High-level policies (scheduling policies) are the focus of this section.
2. **Scheduling Policies**:
    
    - Also referred to as scheduling disciplines.
    - Developed by various experts over the years.
3. **Origins of Scheduling**:
    
    - Pre-date computer systems.
    - Derived from operations management (e.g., assembly lines).
    - Similar concerns exist in both fields, such as efficiency.
4. **Core Problem (The Crux)**:
    
    - **Key Question**: How to develop a scheduling policy framework?
    - **Key Considerations**:
        - Assumptions underlying scheduling.
        - Important metrics to evaluate scheduling.
        - Basic approaches used in early computer systems.
5. **Objective**:
    
    - Understand the foundational principles and metrics for designing effective scheduling policies.


### Key Notes: 7.1 Workload Assumptions

1. **Workload and Scheduling Policies**:
    
    - Workload refers to the processes (or jobs) running in the system.
    - Understanding the workload is critical for designing effective scheduling policies.
    - More knowledge about the workload allows for more fine-tuned policies.
2. **Initial Simplifying Assumptions**:
    
    - These assumptions are unrealistic but serve as a starting point for building scheduling policies.
    - Assumptions will be relaxed over time to develop a fully-operational scheduling discipline.
3. **Assumptions About Jobs**:
    
    - **Uniform Job Duration**: Each job runs for the same amount of time.
    - **Simultaneous Arrival**: All jobs arrive at the same time.
    - **No Preemption**: Once started, each job runs to completion.
    - **CPU-Only Jobs**: Jobs only use the CPU and perform no I/O.
    - **Known Run-Time**: The run-time of each job is known in advance.
4. **Unrealistic Nature of Assumptions**:
    
    - Some assumptions are more unrealistic than others.
    - Example: Knowing the run-time of each job assumes the scheduler is omniscient, which is highly unlikely in real-world systems.
5. **Objective**:
    
    - Use these assumptions as a foundation to gradually build more realistic and practical scheduling policies.

---
### Key Notes: 7.2 Scheduling Metrics

1. **Purpose of Scheduling Metrics**:
    
    - Metrics are used to measure and compare the effectiveness of different scheduling policies.
    - They help evaluate the performance and fairness of a scheduling system.
2. **Primary Metric: Turnaround Time**:
    
    - **Definition**: The time a job completes minus the time it arrived in the system.
    - **Formula**:  
        **Tturnaround = Tcompletion − Tarrival**
    - **Simplification**:
        - Initially, assume all jobs arrive at the same time (**Tarrival = 0**), so:  
            **Tturnaround = Tcompletion**.
        - This will change as assumptions are relaxed.
3. **Types of Metrics**:
    
    - **Performance Metric**: Turnaround time is the primary focus in this chapter.
    - **Fairness Metric**: Example includes **Jain’s Fairness Index**, which measures fairness in resource allocation.
4. **Trade-Off Between Performance and Fairness**:
    
    - Optimizing for performance may reduce fairness (e.g., some jobs may be delayed or starved).
    - Balancing these metrics is a key challenge in scheduling.
5. **Objective**:
    
    - Understand and use metrics like turnaround time to evaluate scheduling policies.
    - Explore the tension between performance and fairness in scheduling decisions.