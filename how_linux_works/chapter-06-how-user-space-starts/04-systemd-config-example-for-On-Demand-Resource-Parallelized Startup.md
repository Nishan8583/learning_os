## `[Install]` Section and Enabling Units

- The `[Install]` section lets a unit declare **which other unit wants or requires it**, rather than modifying the other unit's configuration.
- This is useful when you want to add a dependency **without editing an existing system unit file**.
- `[Install]` is primarily used with `systemctl enable` / `disable`.
- **Important:** `[Install]` is **not used during normal unit activation**. It tells `systemctl enable` how to create dependency symlinks.
- Enabling a unit is persistent across reboots.
- **Enabling ≠ starting**
    - `systemctl enable` → configures the unit to be pulled in automatically.
    - `systemctl start` → activates the unit now.

### Example

Instead of putting:

```
# test2.target
[Unit]
Wants=test1.target
```

you can put the dependency in `test1.target`:

```
# test1.target
[Unit]
Description=test 1

[Install]
WantedBy=test2.target
```

Then:

```
systemctl enable test1.target
```

creates:

```
/etc/systemd/system/test2.target.wants/test1.target
```

which is effectively a symlink to `test1.target`.

Now when `test2.target` is started, `test1.target` is pulled in.

### Disable

```
systemctl disable test1.target
```

removes the dependency symlink.

---

## `[Install]` Parameters

|Parameter|Meaning|
|---|---|
|`WantedBy=`|When the unit is enabled, create a symlink in the `<target>.wants/` directory. This makes the target **want** the unit.|
|`RequiredBy=`|When the unit is enabled, create a symlink in the `<target>.requires/` directory. This makes the target **require** the unit.|

### `WantedBy=`

```
[Install]
WantedBy=test2.target
```

After:

```
systemctl enable test1.target
```

systemd creates approximately:

```
/etc/systemd/system/test2.target.wants/test1.target
```

Conceptually:

```
test2.target
    └── Wants → test1.target
```

If `test1.target` fails, `test2.target` itself does not necessarily fail.

### `RequiredBy=`

```
[Install]
RequiredBy=test2.target
```

creates:

```
/etc/systemd/system/test2.target.requires/test1.target
```

Conceptually:

```
test2.target
    └── Requires → test1.target
```

The `Requires=` relationship is stronger than `Wants=`: failure or disappearance of a required unit can affect the requiring unit.

---

## What `systemctl enable` Actually Does

Think of:

```
systemctl enable test1.target
```

as:

> "Create the appropriate symlinks so that systemd will pull this unit in through its dependency relationship."

It **does not start the unit**.

```
systemctl enable test1.target
systemctl start test1.target
```

performs two different operations:

|Command|Effect|
|---|---|
|`systemctl enable UNIT`|Configure persistent activation through symlinks|
|`systemctl disable UNIT`|Remove those symlinks|
|`systemctl start UNIT`|Activate UNIT now|
|`systemctl stop UNIT`|Deactivate UNIT now|
|`systemctl status UNIT`|Inspect current state|

### Persistence

An enabled unit remains enabled across reboots because the symlink exists in the systemd configuration hierarchy.

---

## `.wants` and `.requires` Directories

Typical locations:

```
/etc/systemd/system/
```

and:

```
/usr/lib/systemd/system/
```

You may encounter directories such as:

```
test2.target.wants/
test2.target.requires/
```

They contain symlinks representing dependencies.

### Manual dependency links

You can manually create symlinks in `.wants` or `.requires` directories.

This can add dependencies without modifying the unit file itself.

However, this is generally discouraged because:

- the dependency is harder to discover/trace;
- it isn't obvious from the unit's configuration;
- manually modified files may be harder to maintain;
- editing files under `/usr/lib/systemd/system` can be overwritten by package upgrades.

For administrator customizations, `/etc/systemd/system` is generally the appropriate configuration hierarchy.

---

# On-Demand / Resource-Parallelized Startup

systemd can delay starting a service until **something actually needs it**.

Instead of:

```
Boot
 ↓
Start service
 ↓
Service waits for clients
```

you can have:

```
Boot
 ↓
Activate resource unit
 ↓
Wait for resource access
 ↓
Client accesses resource
 ↓
systemd starts service
 ↓
Service handles request
```

This is called **socket/path/device activation**, depending on the resource.

### General model

1. Create **Unit A** — the actual service.
2. Identify a resource Unit A provides/uses.
    - Network socket
    - File/path
    - Device
3. Create a resource unit **Unit R**.
4. Associate Unit R with Unit A.
5. systemd activates Unit R.
6. systemd monitors the resource.
7. Something accesses the resource.
8. systemd activates Unit A.
9. Unit A takes control of the resource and handles the request.

### Benefits

- Services don't necessarily need to run continuously.
- Startup can be delayed until the service is actually needed.
- systemd can perform resource monitoring and activation.
- Multiple services can be started in parallel when their resources become active.

### Limitations

- The resource unit must cover the resources through which the service is accessed.
- The resource unit must be correctly associated with its service.
- Not every server/application knows how to use systemd-provided resources.

Conceptually similar ideas exist in:

```
inetd
xinetd
automount
```

---

# Socket Activation Example

A socket unit can represent a listening network socket.

### `echo.socket`

```
[Unit]
Description=echo socket

[Socket]
ListenStream=22222
Accept=true
```

### Important `[Socket]` parameters

|Parameter|Meaning|
|---|---|
|`ListenStream=`|Creates/listens on a TCP stream socket at the specified port/address. `22222` means TCP port 22222.|
|`Accept=`|Controls whether systemd creates a separate service instance for each incoming connection. `true` means each connection gets its own service instance.|

Start the socket:

```
systemctl start echo.socket
```

At this point, the **socket is listening**, but the actual echo service doesn't necessarily need to be running yet.

A client connects:

```
telnet localhost 22222
```

The connection causes systemd to activate the corresponding service.

---

# Socket → Service Association

The service:

```
echo@.service
```

corresponds to:

```
echo.socket
```

because they share the same prefix:

```
echo.socket
echo@.service
```

With `Accept=true`, systemd creates an **instance** of the template service for each connection.

Conceptually:

```
echo.socket
     │
     │ incoming connection
     ▼
echo@.service
     │
     ▼
/bin/cat
```

The `@` indicates a **template unit**.

An instance might conceptually look like:

```
echo@12345.service
```

where the instance identifier is supplied by systemd.

---

## `echo@.service`

```
[Unit]
Description=echo service

[Service]
ExecStart=/bin/cat
StandardInput=socket
```

### New parameters

|Parameter|Section|Meaning|
|---|---|---|
|`ExecStart=`|`[Service]`|Command that systemd executes to start the service.|
|`StandardInput=`|`[Service]`|Determines where the service's standard input comes from. `socket` means input comes from the socket connection handed to the service.|

So:

```
ExecStart=/bin/cat
StandardInput=socket
```

means essentially:

```
TCP connection
      ↓
service stdin
      ↓
/bin/cat
      ↓
service stdout
      ↓
TCP connection
```

Therefore anything the client sends is echoed back.

---

# Explicit Socket-to-Service Association

Normally:

```
echo.socket
echo@.service
```

are associated by their common prefix.

But you can explicitly specify the relationship when the names don't match.

For example:

```
# foo.service

[Service]
Socket=bar.socket
```

This tells systemd that:

```
bar.socket
     ↓
foo.service
```

instead of relying on matching prefixes.

---

# Important Commands

|Command|Purpose|
|---|---|
|`systemctl enable test1.target`|Enable unit and create appropriate dependency symlink|
|`systemctl disable test1.target`|Disable unit and remove dependency symlink|
|`systemctl start test2.target`|Start target and its enabled/wanted dependencies|
|`systemctl start echo.socket`|Start listening socket|
|`systemctl stop echo.socket`|Stop socket and associated socket-activated service|
|`systemctl status UNIT`|Show unit state and useful diagnostic information|

---

# Key Mental Model

### `[Install]`

```
[Install]
WantedBy=test2.target
```

means:

> "When this unit is enabled, make `test2.target` want me."

It does **not** mean:

> "Immediately start me."

---

### Enable vs Start

```
systemctl enable
        ↓
create persistent symlink
        ↓
survives reboot
```

versus:

```
systemctl start
        ↓
activate unit now
```

---

### Socket activation

```
echo.socket
     │
     │ client connects
     ▼
systemd notices connection
     │
     ▼
echo@.service
     │
     ▼
/bin/cat
     │
     ▼
echo response
```

**Core takeaway:** systemd can manage not only _when a process starts_, but also _when a resource becomes active_, allowing a service to start only when that resource is actually used.

---


### Question:
```
So wait, there is 2 separate service file for this echo service ?
```


Yes — **there are two separate unit files**, but only **one of them is actually the service**.

For the example:

```
echo.socket
echo@.service
```

### 1. `echo.socket` — socket unit

```
[Socket]
ListenStream=22222
Accept=true
```

Its job is to **listen for incoming connections** on TCP port `22222`.

It doesn't run `/bin/cat`.

### 2. `echo@.service` — service unit

```
[Service]
ExecStart=/bin/cat
StandardInput=socket
```

Its job is to **handle the connection** once someone connects.

So the flow is:

```
                TCP :22222
                    │
                    ▼
              echo.socket
          "Someone connected!"
                    │
                    ▼
             systemd starts
             echo@.service
                    │
                    ▼
                 /bin/cat
                    │
                    ▼
              Echoes input
```

The important distinction is:

> **`echo.socket` is not another service. It's a socket unit.**

And `echo@.service` is a **template service unit**. With `Accept=true`, systemd can create an instance of it for each incoming connection.

For example:

```
echo@.service          ← template
     │
     ├── connection 1 → echo@<instance>.service
     ├── connection 2 → echo@<instance>.service
     └── connection 3 → echo@<instance>.service
```

So you can think of it as **one socket definition + one service definition**, rather than two service files.

For this example:

|Role|Unit|Purpose|
|---|---|---|
|**Resource unit**|`echo.socket`|Represents and listens on TCP port `22222`|
|**Service unit**|`echo@.service`|Runs `/bin/cat` to handle each connection|

The relationship is:

```
Resource Unit
echo.socket
    │
    │ Client connects to :22222
    ▼
systemd activates
    │
    ▼
Service Unit
echo@.service
    │
    ▼
/bin/cat
```

And because the socket has:

```
Accept=true
```

systemd can create a **service instance per connection**.

So the key concept from the book is: