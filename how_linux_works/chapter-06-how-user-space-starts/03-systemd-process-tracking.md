## systemd Process Tracking & Synchronization

### Why systemd uses cgroups

- Services can start in many different ways:
    - A process may remain attached to the original process.
    - A service may `fork()` into another process.
    - A daemon may detach/daemonize itself.
    - A service can spawn an unpredictable number of subprocesses.
- Historically, this made tracking and controlling all processes belonging to a service difficult.
- systemd uses **Linux cgroups** to track the process hierarchy belonging to an activated unit.
- Because cgroups handle process tracking, unit-file authors don't need to manually account for every possible subprocess.
- For service startup behavior, the important question is generally whether the service forks. This is specified with the `Type=` option.

---

# `Type=` — Service Startup Behavior

`Type=` tells systemd how the service starts and, importantly, **when systemd should consider it started/ready**.

### `Type=simple`

```
[Service]
Type=simple
```

- The service process does **not fork and terminate**.
- The original process remains the main service process.
- systemd considers the service started without waiting for an explicit readiness notification.
- Problem: the process may need additional time to initialize, but systemd doesn't necessarily know when it is actually ready.
- This can matter when other units strictly depend on the service being ready.

**Mental model:**

```
systemd
   │
   └── starts service
          │
          └── process stays alive
```

---

### `Type=forking`

```
[Service]
Type=forking
```

- The service forks.
- systemd expects the original service process to terminate.
- Once that original process terminates, systemd assumes the service is ready.

**Mental model:**

```
systemd
   │
   └── parent service
          │
          ├── forks
          │    └── child/daemon
          │
          └── parent terminates
                    ↓
             systemd considers
              service ready
```

---

### `Type=notify`

```
[Service]
Type=notify
```

- Similar to `simple`, but the service explicitly tells systemd when it is ready.
- The service uses a special systemd notification mechanism/function call.
- This solves the problem where simply starting the process doesn't mean that initialization is complete.

```
systemd starts service
        ↓
service initializes
        ↓
service notifies systemd
        ↓
systemd knows: READY
```

---

### `Type=dbus`

```
[Service]
Type=dbus
```

- systemd considers the service ready when it registers itself on **D-Bus**.
- Useful for services whose readiness is represented by successful D-Bus registration.

---

### `Type=oneshot`

```
[Service]
Type=oneshot
```

- The service process performs its task and **terminates completely**.
- It is similar to `Type=simple`, except systemd doesn't consider the service started until the process terminates.
- Strict dependencies won't start until the `oneshot` process has terminated.
- `Type=oneshot` gets a default:

```
RemainAfterExit=yes
```

- Therefore, systemd can consider the service **active even after its process has exited**.

**Useful mental model:**

```
start
  ↓
run task
  ↓
process exits
  ↓
systemd considers service started/active
```

---

### `Type=idle`

```
[Service]
Type=idle
```

- Behaves similarly to `simple`.
- However, systemd delays starting the service until **all currently active jobs have finished**.
- Intended primarily to prevent services from interfering with each other's output during startup.
- Once a service has started, the systemd job that started it terminates, so waiting for all other jobs to finish effectively delays the service until other startup activity has settled.

---

# `Type=` comparison

|Type|Startup/readiness behavior|
|---|---|
|`simple`|Process stays as the main service process; systemd doesn't wait for an explicit readiness signal.|
|`forking`|Service forks; systemd expects the original process to terminate, then considers the service ready.|
|`notify`|Service explicitly notifies systemd when it is ready.|
|`dbus`|Service becomes ready when it registers on D-Bus.|
|`oneshot`|Process performs a task and terminates; systemd considers it started after termination. Defaults to `RemainAfterExit=yes`.|
|`idle`|Similar to `simple`, but startup is delayed until currently active jobs finish.|

---

# systemd Dependencies

Dependencies determine how units relate to one another during activation.

A key distinction:

> **Dependencies and ordering are separate concepts.**

For example:

```
Wants=database.service
```

means:

> "Activate the database service when this unit activates."

It does **not necessarily mean**:

> "Wait for database.service to finish starting before starting me."

For ordering, you use `After=` or `Before=`.

---

# `Requires=` — Strict dependency

```
[Unit]
Requires=database.service
```

- Creates a **strict dependency**.
- When the dependent unit is activated, systemd attempts to activate the required unit.
- If the dependency fails, systemd also deactivates the dependent unit.

**Mental model:**

```
A Requires B

Start A
  ↓
Start B
  ↓
B fails
  ↓
A is also deactivated
```

This can create a strong failure chain, so strict dependencies should be used deliberately.

---

# `Wants=` — Activation dependency

```
[Unit]
Wants=database.service
```

- Causes systemd to attempt to activate the dependency.
- However, failure of the dependency does **not** cause the dependent unit to fail.
- This makes `Wants=` more fault-tolerant than `Requires=`.

```
A Wants B

Start A
  ↓
systemd attempts B
  ↓
B fails
  ↓
A can still remain active
```

The source specifically emphasizes `Wants=` as a useful dependency type because it avoids propagating failures through the system.

**Rule of thumb from the text:**

> Prefer `Wants=` when possible because it provides a more robust dependency relationship.

---

# `Requisite=` — Must already be active

```
[Unit]
Requisite=database.service
```

- The dependency must **already be active**.
- systemd checks the dependency before activating the unit.
- If the dependency isn't active, activation of the dependent unit fails.
- Unlike `Requires=`/`Wants=`, it doesn't primarily mean "start this dependency for me."

```
Start A
  ↓
Is B already active?
  │
  ├── YES → start A
  └── NO  → fail A
```

---

# `Conflicts=` — Negative dependency

```
[Unit]
Conflicts=other.service
```

- Defines units that should not be active simultaneously.
- When the current unit is activated, systemd deactivates the conflicting unit if it is active.
- Simultaneous activation of conflicting units fails.

```
A Conflicts B

Start A
  ↓
B active?
  ↓
deactivate B
```

Think of `Conflicts=` as the **opposite direction of dependency**: instead of requiring another unit, it establishes that another unit must not be active alongside it.

---

# Viewing dependencies

The source gives:

```
systemctl show -p type unit
```

This uses `systemctl show` with the `-p` option to display a particular property of a unit. The text presents it in the context of viewing dependency information by specifying a dependency type.

---

# Ordering Dependencies

Dependency relationships don't automatically determine startup order.

For example:

```
Wants=foo.service
```

primarily says:

```
"Also activate foo.service."
```

If you need:

```
foo → then → current service
```

you need an ordering relationship.

---

## `Before=`

```
[Unit]
Before=bar.target
```

Means:

> The current unit should activate **before** `bar.target`.

Example:

```
# foo.target

[Unit]
Before=bar.target
```

Startup ordering:

```
foo.target
     ↓
bar.target
```

---

## `After=`

```
[Unit]
After=foo.service
```

Means:

> The current unit should activate **after** `foo.service`.

Startup ordering:

```
foo.service
     ↓
current.service
```

When ordering is specified, systemd waits for the relevant unit to reach an active state before activating the dependent unit.

---

# Dependency vs Ordering

This is one of the most important concepts in this section.

### Dependency

Answers:

> **"Should this other unit be activated?"**

Examples:

```
Requires=foo.service
Wants=foo.service
```

### Ordering

Answers:

> **"Which unit should activate first?"**

Examples:

```
Before=foo.service
After=foo.service
```

You can combine them:

```
[Unit]
Wants=database.service
After=database.service
```

Meaning:

```
1. Attempt to activate database.service
2. Start this service after database.service becomes active
3. If database.service fails, this service isn't necessarily
   forced down because Wants= is not strict
```

---

# Default / Implicit Dependencies

Not every dependency you see in systemd is explicitly written in a unit file.

systemd can automatically add dependencies internally.

These are called **default dependencies**.

For example, the source notes that target units with `Wants=` dependencies may receive an automatically added `After=` relationship.

So you might have:

```
Wants=foo.service
```

but systemd internally adds an ordering relationship as well.

These automatically calculated dependencies:

- Aren't necessarily visible in the unit file.
- Are calculated by systemd at boot.
- Depend on the type of unit.
- Exist to avoid common mistakes and keep unit files simpler.

### Disable default dependencies

```
[Unit]
DefaultDependencies=no
```

This disables the default dependencies for that unit.

**Important:** this is not something you normally add casually; default dependencies exist to provide sensible system behavior.

---

# Conditional Dependencies

Conditional dependencies are different from normal unit-to-unit dependencies.

Instead of asking:

> "Is another systemd unit active?"

they ask:

> **"Does the operating system currently satisfy this condition?"**

### `ConditionPathExists=`

```
[Unit]
ConditionPathExists=/some/path
```

True if the specified path exists.

```
/path exists?
   │
   ├── YES → unit can activate
   └── NO  → unit doesn't activate
```

### `ConditionPathIsDirectory=`

```
ConditionPathIsDirectory=/some/directory
```

True if the specified path is a directory.

### `ConditionFileNotEmpty=`

```
ConditionFileNotEmpty=/some/file
```

True if the specified file exists and is not zero-length.

The important behavior is:

> If a condition is false when systemd attempts to activate the unit, **that unit isn't activated**.

However, the condition only applies to the unit containing the condition. systemd can still attempt to activate that unit's other dependencies.

---

# Important Dependency Mental Model

```
                    systemd dependencies
                           │
          ┌────────────────┼────────────────┐
          │                │                │
      Activation        Ordering       Conditions
          │                │                │
    ┌─────┴─────┐      ┌───┴───┐       ┌────┴─────────────┐
    │           │      │       │       │        │          │
Requires     Wants   After   Before   PathExists  IsDirectory ...
    │           │
  strict    tolerant
```

### The key questions

When creating a unit file, ask:

1. **What should be activated with my unit?**
    - `Requires=`
    - `Wants=`
2. **Must another unit already be running?**
    - `Requisite=`
3. **What must NOT run simultaneously?**
    - `Conflicts=`
4. **Which unit must start first?**
    - `After=`
    - `Before=`
5. **Should startup depend on an OS condition?**
    - `ConditionPathExists=`
    - `ConditionPathIsDirectory=`
    - `ConditionFileNotEmpty=`
6. **Should systemd's automatically added dependencies be disabled?**
    - `DefaultDependencies=no`

---