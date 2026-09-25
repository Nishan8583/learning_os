## systemd Unit Files

### Unit file structure

- **systemd unit files** use a format derived from the XDG Desktop Entry specification.
- Structure:
    - Sections use `[SectionName]`.
    - Settings are written as `option=value`.
- Common sections include:
    - `[Unit]` — description and dependency information.
    - `[Service]` — how a service is started, stopped, reloaded, and managed.
    - `[Install]` — information used when enabling a unit.

Example:

```
[Unit]
Description=D-Bus System Message Bus
Documentation=man:dbus-daemon(1)
Requires=dbus.socket
RefuseManualStart=yes

[Service]
ExecStart=/usr/bin/dbus-daemon --system ...
ExecReload=/usr/bin/dbus-send ...
```

### Important unit-file directives

- `Description=` — human-readable description of the unit.
- `Documentation=` — documentation associated with the unit.
- `Requires=` — declares a dependency on another unit.
- `ExecStart=` — command used to start the service.
- `ExecReload=` — command used to reload the service's configuration.
- `EnvironmentFile=` — specifies a file containing environment variables.
- `ExecStartPre=` — command executed before the main service starts.

---

## Variables in unit files

- Variables begin with `$`.
- Example:

```
[Service]
EnvironmentFile=/etc/sysconfig/sshd
ExecStart=/usr/sbin/sshd -D $OPTIONS $CRYPTO_POLICY
ExecReload=/bin/kill -HUP $MAINPID
```

- `$OPTIONS` and `$CRYPTO_POLICY`
    - Their values come from the file specified by `EnvironmentFile=`.
    - In this example, check `/etc/sysconfig/sshd` to determine their values.
- `$MAINPID`
    - Represents the PID of the process tracked by systemd.
    - systemd records this PID when the unit starts.
    - It can subsequently be used to send signals to the service.

**Mental model:**

```
EnvironmentFile
      ↓
variables such as $OPTIONS
      ↓
ExecStart command
```

---

## Specifiers

- **Specifiers** are another variable-like mechanism used in unit files.
- They begin with `%`, rather than `$`.
- Examples:
    - `%n` → current unit name.
    - `%H` → current hostname.

### Instance units

systemd can use a single unit file as a template for multiple instances.

Example:

```
getty@.service
```

can create:

```
getty@tty1
getty@tty2
```

- The part after `@` is the **instance**.
- `%I` or `%i` can be used inside the template to refer to the instance.
- When systemd activates the unit, it substitutes the specifier with the instance name.

**Mental model:**

```
getty@.service
       │
       ├── getty@tty1
       └── getty@tty2
```

---

# systemctl

`systemctl` is the primary command used to interact with systemd.

It can:

- Activate/deactivate services.
- Start/stop/restart services.
- View unit status.
- Reload configuration.
- List units.
- View jobs.
- Enable/disable units.

### List units

```
systemctl list-units
```

- Lists currently **active** units.
- `list-units` is the default `systemctl` operation, so:

```
systemctl
```

is effectively equivalent to:

```
systemctl list-units
```

### Useful options

```
systemctl list-units --full
```

- Displays full unit names instead of truncating long names.

```
systemctl list-units --all
```

- Shows all units, rather than only active units.

---

# Checking service status

```
systemctl status sshd.service
```

The output provides considerably more than simply whether the service is running.

Useful information includes:

```
Loaded: loaded (...)
Active: active (running)
Main PID: 1110 (sshd)
Tasks: 1
CGroup: /system.slice/sshd.service
```

You can therefore determine:

- Whether the unit is loaded.
- Whether it is active/running.
- When it started.
- The main process PID.
- Processes associated with the service.
- The service's cgroup.
- Recent diagnostic log messages.

### Cgroups

A service may have a cgroup such as:

```
/system.slice/sshd.service
```

The processes belonging to the service appear underneath it.

For viewing systemd-related cgroups separately:

```
systemd-cgls
```

---

# systemd logs

`systemctl status` only displays the **most recent** diagnostic messages.

To view all messages for a unit:

```
journalctl --unit=unit_name
```

For example:

```
journalctl --unit=sshd.service
```

`journalctl` accesses the systemd journal, and depending on system/user configuration, may require superuser privileges.

---

# Starting, stopping, and restarting units

Basic service lifecycle:

```
systemctl start unit
systemctl stop unit
systemctl restart unit
```

- `start` → activate a unit.
- `stop` → deactivate a unit.
- `restart` → stop and start the unit again.

### Reloading configuration

There are two different concepts:

```
systemctl reload unit
```

- Reloads the configuration **for that specific unit**.

```
systemctl daemon-reload
```

- Makes systemd reload **all unit configuration files**.

Important distinction:

```
Changed service configuration
        │
        ├── Service supports reload?
        │       └── systemctl reload unit
        │
        └── Changed the unit file itself?
                └── systemctl daemon-reload
```

---

# systemd Jobs

systemd calls requests to activate, reactivate, or restart units **jobs**.

A job represents a requested unit state change.

View current jobs:

```
systemctl list-jobs
```

Normally, a running system has few or no active jobs because boot-time jobs have completed.

During boot, however, you may see jobs waiting for other jobs to finish. For example:

```
JOB  UNIT                 TYPE   STATE
76   sendmail.service     start  running
71   systemd-...service   start  waiting
```

If job 76 is taking a long time, other jobs may remain in `waiting` until it completes.

**Important:** systemd's "jobs" are unrelated to shell job control.

---

# Adding your own units

Custom unit files should normally be placed in:

```
/etc/systemd/system/
```

This separates your own unit files from distribution-provided units and prevents package upgrades from overwriting them.

### Example dependency

Create:

```
/etc/systemd/system/test1.target
```

```
[Unit]
Description=test 1
```

Then:

```
/etc/systemd/system/test2.target
```

```
[Unit]
Description=test 2
Wants=test1.target
```

`Wants=` establishes a dependency such that activating `test2.target` causes `test1.target` to be activated as well.

Activate:

```
systemctl start test2.target
```

Then verify:

```
systemctl status test1.target test2.target
```

Both targets should be active.

---

# Enable vs Start

These are different operations.

### Start

```
systemctl start unit
```

Starts the unit **now**.

### Enable

```
systemctl enable unit
```

Enables the unit through systemd's dependency mechanism so that it can be activated as configured, typically during boot.

If a unit has an `[Install]` section, it needs to be enabled before the corresponding dependency links are created.

**Remember:**

```
start  = activate now
enable = configure automatic activation
```

---

# Removing a unit

Typical sequence:

```
systemctl stop unit
systemctl disable unit
```

Then remove the unit file if desired.

- `stop` → deactivate the running unit.
- `disable` → remove symbolic links created by the dependency system.
- Then the unit file itself can be deleted.

If a unit is **implicitly enabled** because it has no `[Install]` section, `systemctl disable` has no effect.

---

## Quick systemd mental model

```
                    systemd
                       │
              ┌────────┴────────┐
              │                 │
           Units              Jobs
              │                 │
      ┌───────┼───────┐         │
    service   target  mount     │
      │                         │
      └── managed by ── systemctl
                              │
             ┌────────────────┼────────────────┐
             │                │                │
           status           start            stop
             │
        journalctl
             │
          logs
```

The most important distinction to remember is:

> **A unit is the configuration/object systemd manages; a job is a requested state change involving a unit.**