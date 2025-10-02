## Key Points: Ticket Mechanisms in Lottery Scheduling

1. Ticket Currency

- Allows users to allocate tickets among their jobs in a local currency.

- System converts local currency to global currency for scheduling.

- Example: User A allocates 500 tickets (local) to each job, converted to 50 tickets (global) each.
![alt_test](chapter-9-2.png)

2. Ticket Transfer

- Processes can temporarily transfer tickets to another process.

- Useful in client/server settings to speed up work on behalf of the client.

- Tickets are transferred back to the client when the work is completed.

3. Ticket Inflation

- Processes can temporarily raise or lower their ticket count.

- Useful in trusted environments where processes cooperate.

- Allows processes to dynamically adjust their priority without communication.

These mechanisms provide flexibility and dynamic allocation of resources in lottery scheduling.