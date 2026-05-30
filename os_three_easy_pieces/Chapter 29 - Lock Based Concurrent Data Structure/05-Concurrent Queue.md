### Key Notes on 29.3 Concurrent Queues

#### Overview

- A standard method to create a concurrent data structure is to use a single large lock, but this approach is not optimal for performance.
- Michael and Scott designed a more concurrent queue that uses two locks: one for the head and one for the tail.
- The goal of this design is to allow concurrent `enqueue` and `dequeue` operations, improving performance in multi-threaded environments.

#### Key Features of Michael and Scott Concurrent Queue

1. **Two Locks**:
    
    - `head_lock`: Used for dequeue operations.
    - `tail_lock`: Used for enqueue operations.
    - This separation allows enqueue and dequeue operations to execute concurrently in most cases.
2. **Dummy Node**:
    
    - A dummy node is allocated during queue initialization.
    - It serves as a placeholder to separate the head and tail operations.
3. **Use Case**:
    
    - Commonly used in multi-threaded applications.
    - This implementation may not fully meet the needs of programs requiring bounded queues or waiting mechanisms for empty/full queues. These features are covered in the next chapter on condition variables.

#### Code Implementation

```c
typedef struct __node_t {
    int value;
    struct __node_t *next;
} node_t;

typedef struct __queue_t {
    node_t *head;
    node_t *tail;
    pthread_mutex_t head_lock, tail_lock;
} queue_t;

void Queue_Init(queue_t *q) {
    node_t *tmp = malloc(sizeof(node_t));
    tmp->next = NULL;
    q->head = q->tail = tmp;
    pthread_mutex_init(&q->head_lock, NULL);
    pthread_mutex_init(&q->tail_lock, NULL);
}

void Queue_Enqueue(queue_t *q, int value) {
    node_t *tmp = malloc(sizeof(node_t));
    assert(tmp != NULL);
    tmp->value = value;
    tmp->next = NULL;

    pthread_mutex_lock(&q->tail_lock);
    q->tail->next = tmp;
    q->tail = tmp;
    pthread_mutex_unlock(&q->tail_lock);
}

int Queue_Dequeue(queue_t *q, int *value) {
    pthread_mutex_lock(&q->head_lock);
    node_t *tmp = q->head;
    node_t *new_head = tmp->next;
    if (new_head == NULL) {
        pthread_mutex_unlock(&q->head_lock);
        return -1; // Queue is empty
    }
    *value = new_head->value;
    q->head = new_head;
    pthread_mutex_unlock(&q->head_lock);
    free(tmp);
    return 0;
}
```

#### Key Points in the Code

1. **Initialization (`Queue_Init`)**:
    
    - A dummy node is created and both `head` and `tail` pointers are initialized to point to it.
    - Two mutex locks (`head_lock` and `tail_lock`) are initialized.
2. **Enqueue Operation (`Queue_Enqueue`)**:
    
    - A new node is created with the given value.
    - The `tail_lock` is acquired to ensure exclusive access to the tail.
    - The new node is added to the end of the queue, and the `tail` pointer is updated.
    - The `tail_lock` is released after the operation.
3. **Dequeue Operation (`Queue_Dequeue`)**:
    
    - The `head_lock` is acquired to ensure exclusive access to the head.
    - The current head node is stored in a temporary variable, and the new head is set to the next node.
    - If the queue is empty (`new_head == NULL`), the function returns `-1`.
    - Otherwise, the value of the new head is retrieved, the head pointer is updated, and the old head is freed.
    - The `head_lock` is released after the operation.

#### Advantages

- **Improved Concurrency**: By using separate locks for the head and tail, enqueue and dequeue operations can proceed in parallel, reducing contention.
- **Simplicity**: The use of a dummy node simplifies the logic for handling edge cases, such as an empty queue.

#### Limitations

- This implementation does not handle bounded queues or provide mechanisms for threads to wait when the queue is empty or full. These features are addressed in the context of condition variables in the next chapter.