# Scheduling: Proportional Share

- Proportional-share scheduling (or fair-share scheduling) aims to guarantee each job a certain percentage of CPU time.

- Unlike other schedulers, it doesn't optimize for turnaround or response time.

- Lottery scheduling is an example of proportional-share scheduling, where processes are given chances to "win" CPU time based on their priority.

The main question being addressed is: **How to design a scheduler to share the CPU proportionally?**

## Key Points: Lottery Scheduling

- **Tickets represent share**: Tickets determine the proportion of a resource (e.g., CPU) a process should receive.

- **Ticket allocation**: Processes are assigned tickets, and the percentage of tickets determines their share of the resource.

- **Lottery mechanism**: The scheduler holds a "lottery" to determine which process runs next, selecting a winning ticket randomly.

- **Probabilistic correctness**: Lottery scheduling achieves desired proportions probabilistically, not deterministically.

- **Randomness**: The use of randomness leads to variations in actual allocation, but desired proportions are more likely to be achieved over time.

**Example**:

- Process A has 75 tickets, Process B has 25 tickets.

- Desired allocation: A (75%), B (25%).

- Actual allocation may vary (e.g., B gets 20% instead of 25%), but will converge to desired proportions over time.

![alt_test](chapter-9-1.png)

## Key Points: Advantages of Randomness in Scheduling

- **Robustness**: Random approaches avoid strange corner-case behaviors and worst-case performance.

- **Lightweight**: Randomness requires minimal state tracking, reducing overhead.

- **Speed**: Random decision-making can be fast, especially when generating random numbers is quick.

**Benefits of Randomness**:

- Handles cyclic-sequential workloads better than traditional algorithms (e.g., LRU).

- Requires minimal per-process state (e.g., number of tickets).

- Fast decision-making, suitable for high-speed applications.

```
TIP: USE TICKETS TO REPRESENT SHARES
One of the most powerful (and basic) mechanisms in the design of lottery
(and stride) scheduling is that of the ticket. The ticket is used to represent
a process’s share of the CPU in these examples, but can be applied much
more broadly. For example, in more recent work on virtual memory man-
agement for hypervisors, Waldspurger shows how tickets can be used to
represent a guest operating system’s share of memory [W02]. Thus, if you
are ever in need of a mechanism to represent a proportion of ownership,
this concept just might be ... (wait for it) ... the ticket.
```

