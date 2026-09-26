# systemd: Instances, Handoff & Boot Optimization

## 1. Service Instances and Handoff

### Why `echo@.service` has `@`

An `@` in a systemd unit filename indicates that the unit is **parameterized/template-based**.

```
echo@.service
     ^
     template
```

This allows systemd to create **multiple instances** of the same service:

```
echo@1.service
echo@2.service
echo@3.service
```

Each instance can handle a separate resource, such as a network connection.

---

## 2. `Accept=true` and Per-Connection Instances

Recall the socket unit:

```
[Socket]
ListenStream=...
Accept=true
```

With:

```
Accept=true
```

systemd:

1. Listens on the network port.
2. Accepts an incoming connection.
3. Creates a **separate service instance** for that connection.
4. Passes the accepted connection to the service instance.

Conceptually:

```
                    ┌── Connection 1
                    │       ↓
Network ──→ echo.socket ──→ echo@1.service
                    │
                    ├── Connection 2
                    │       ↓
                    └────→ echo@2.service
```

Each service instance receives its connection as **standard input/output**.

The service therefore doesn't necessarily need to know that the data originated from a network socket.

### Important limitation

This simple socket-to-stdin/stdout model is useful for basic services, but **most real network services require more control and flexibility** than this arrangement provides.

---

## 3. `Accept=true` vs. Service-Controlled Sockets

There are two different approaches.

### Approach A — systemd accepts connections

Use:

```
# echo.socket
[Socket]
Accept=true
```

and a template service:

```
echo@.service
```

Systemd creates a separate service instance for each connection.

```
Client
   │
   ▼
echo.socket
   │ accepts connection
   ▼
echo@.service instance
```

### Approach B — service accepts connections itself

If the service itself knows how to accept network connections:

- **Don't** use `@` in the service filename.
- **Don't** use `Accept=true`.

Instead, the service takes control of the socket and handles the connections itself.

```
echo.socket
     │
     ▼
echo.service
     │
     ├── accepts connection
     ├── handles connection
     ├── accepts another connection
     └── ...
```

In this arrangement, systemd doesn't try to listen on the port again until the service finishes.

### Key distinction

|Configuration|Who accepts connection?|Service type|
|---|---|---|
|`Accept=true`|systemd|`echo@.service` template|
|`Accept=false` / omitted|Service itself|`echo.service`|

---

# 4. Resource Handoff

Systemd supports handing different types of resources to services.

Examples include:

- Network sockets
- Filesystem paths
- Devices
- Other activation resources

The exact handoff mechanism depends on the type of resource.

Useful manual pages:

|Resource / topic|Manual|
|---|---|
|Socket activation|`systemd.socket(5)`|
|Path activation|`systemd.path(5)`|
|Device units|`systemd.device(5)`|
|How services receive resources|`systemd.exec(5)`|

### Important

`systemd.exec(5)` is particularly useful because it documents **how the service receives resources when systemd activates it**.

---

# 5. Boot Optimization with Auxiliary Units

One of systemd's major goals is to:

- simplify dependency management
- parallelize startup
- reduce boot time

Resource units can help achieve this.

The important idea is:

> **Make a resource available early, even while the service responsible for that resource is still starting.**

---

## 6. Traditional Sequential Boot

Consider:

```
Service E
   │
   └── provides Resource R
            │
            ├── Service A depends on R
            ├── Service B depends on R
            └── Service C depends on R
```

A traditional sequential boot might look like:

```
Service E:  Starting ───────→ Started
                              Resource R ready
                                   │
Service A:                       Starting → Started
                                              │
Service B:                                  Starting → Started
                                                          │
Service C:                                              Starting → ...
```

Each service waits for the previous one.

Therefore:

```
E → A → B → C
```

Even though A, B, and C don't depend on each other, they end up starting sequentially.

---

# 7. systemd Resource-Unit Approach

systemd can represent the resource separately:

```
              Resource Unit R
                    │
        ┌───────────┼───────────┐
        │           │           │
        ▼           ▼           ▼
    Service A   Service B   Service C
                    │
                    │
                Service E
             provides/owns R
```

The important trick is that **R can become available before E has completely started**.

Timeline:

```
Resource R:  Available ────────────────────────────────
                  ↑
                  │ E eventually takes over R
                  │
Service E:    Starting ─────────────→ Started
                                        takes over R

Service A:    Starting ─────→ Started
Service B:    Starting ─────→ Started
Service C:    Starting ─────→ Started
```

Therefore, instead of:

```
E → A → B → C
```

systemd can effectively do:

```
       ┌──→ A
       │
R ─────┼──→ B
       │
       ├──→ C
       │
       └──→ E
```

They can **start in parallel**.

---

# 8. Why This Works

Suppose Service E is something essential such as:

```
systemd-journald.service
```

Many other services depend on journaling.

Starting E completely before allowing dependent services to start would introduce unnecessary waiting.

Instead:

```
             Resource R
          (available quickly)
                 │
       ┌─────────┼─────────┐
       ↓         ↓         ↓
      A          B         C
                 │
                 ↓
             Service E
            takes over R
```

The resource interface becomes available quickly, allowing other units to begin startup.

Once E is ready:

```
Resource R
    │
    ↓
Service E takes control
```

---

## 9. Important Subtlety

A service depending on R **doesn't necessarily have to actually use R during startup**.

The system is essentially giving the service the **opportunity** to access the resource as soon as possible.

For example:

```
R available
   │
   ├── A starts
   ├── B starts
   └── C starts
```

Even if A doesn't need R until later in its startup, it doesn't have to wait for E to completely initialize.

This is one reason systemd can reduce unnecessary startup serialization.

---

# 10. Resource Units vs. On-Demand Activation

These mechanisms resemble on-demand activation, but there is an important difference.

### On-demand activation

The service starts **when something requests the resource**.

```
No request
    │
    ▼
Service not running

Request arrives
    │
    ▼
Service starts
```

### Boot parallelization

The service is **started immediately**, while its resource is made available early.

```
Boot
 │
 ├── Resource becomes available
 │
 ├── Service E starts
 │
 ├── A starts
 ├── B starts
 └── C starts
```

So:

> **The same resource-unit mechanisms that enable on-demand activation can also be used to parallelize boot.**

---

## 11. Trade-off: More Parallelism Isn't Always Better

Parallel startup can temporarily increase system load.

If many units start simultaneously:

```
       ┌── A
       ├── B
       ├── C
       ├── D
Boot ──┼── E
       ├── F
       └── G
```

the system may experience a temporary resource spike:

- CPU
- disk I/O
- memory
- other system resources

Therefore:

> **Parallelization can reduce boot latency, but excessive simultaneous startup can temporarily slow the system down.**

---

# 12. Real-World Examples

This approach is commonly used with important system services such as:

- `systemd-journald`
- D-Bus-related systemd units

The general pattern is:

```
             Resource Unit
                  │
       ┌──────────┼──────────┐
       ↓          ↓          ↓
   dependent   dependent   dependent
     unit        unit        unit
                  │
                  ↓
            Actual service
            takes over
```

---

# 13. systemd Auxiliary Components

Systemd has grown beyond simply:

> **starting and stopping services**

It also contains several system components and compatibility helpers.

Many of their executables can be found under:

```
/lib/systemd/
```

These programs provide functionality related to the broader system-management ecosystem.

---

## 14. Important systemd Components

|Component|Purpose|
|---|---|
|`systemd-udevd`|Device management|
|`systemd-journald`|System logging|
|`systemd-resolved`|DNS/name-service caching and resolution|
|`systemd-fsck`|Filesystem checking helper|

### `udevd`

Handles device-management functionality.

Executable:

```
systemd-udevd
```

### `journald`

Provides system logging and handles several logging mechanisms, including compatibility with traditional Unix syslog functionality.

Executable:

```
systemd-journald
```

### `resolved`

Provides DNS/name-service caching and resolution.

Executable:

```
systemd-resolved
```

### `fsck`

Some systemd programs are relatively simple wrappers around traditional system utilities.

For example:

```
systemd-fsck
```

can run filesystem-checking utilities and then communicate the result to systemd.

Conceptually:

```
systemd
   │
   ▼
systemd-fsck
   │
   ▼
standard fsck utility
   │
   ▼
result
   │
   ▼
systemd
```

---

# 15. Finding Unknown Programs in `/lib/systemd`

If you encounter an unfamiliar executable:

```
ls /lib/systemd
```

don't assume it's a core daemon.

Some programs are:

- full system services
- wrappers around existing utilities
- helpers associated with particular unit types
- compatibility components

Check its manual page:

```
man <program>
```

The manual will often tell you:

1. What the program does.
2. Which unit type uses it.
3. What role it plays in systemd.

---

# Key Takeaways

- `@` in a unit filename means **template/parameterized unit**.
- `echo@.service` can create multiple service instances.
- `Accept=true` tells systemd to **accept connections and create a service instance for each connection**.
- With `Accept=true` → typically use `service@.service`.
- If the service accepts connections itself → use a normal `service.service` and don't use `Accept=true`.
- Resource units can make a resource available **before the service providing it has fully started**.
- This allows systemd to **parallelize boot** rather than unnecessarily serializing dependencies.
- Boot parallelization and socket activation use related mechanisms, but parallelization **does not necessarily mean on-demand startup**.
- Too much parallel startup can temporarily increase system load.
- `systemd.socket(5)`, `systemd.path(5)`, `systemd.device(5)`, and especially `systemd.exec(5)` are useful references for resource handoff.
- `/lib/systemd/` contains both major system services and smaller helper/wrapper programs.
- `systemd-fsck` is an example of a wrapper that runs a standard utility and reports the result to systemd.