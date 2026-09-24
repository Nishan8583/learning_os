# Systemd Helpful Commands.
## Commands, options, and what they do

|Command|Options / arguments|What it does|
|---|---|---|
|`systemctl list-units`|`list-units`|Lists active systemd units.|
|`systemctl`|_(none)_|`list-units` is the default operation.|
|`systemctl list-units --full`|`--full`|Displays full unit names without truncating long names.|
|`systemctl list-units --all`|`--all`|Lists all units, not just active units.|
|`systemctl status sshd.service`|`status <unit>`|Displays unit state, PID, cgroup, start time, processes, and recent diagnostic messages.|
|`systemd-cgls`|_(none)_|Displays systemd-related control groups and their processes.|
|`journalctl --unit=unit_name`|`--unit=<unit>`|Displays journal messages associated with the specified unit.|
|`systemctl start unit`|`start <unit>`|Activates/starts a unit.|
|`systemctl stop unit`|`stop <unit>`|Deactivates/stops a unit.|
|`systemctl restart unit`|`restart <unit>`|Restarts a unit.|
|`systemctl reload unit`|`reload <unit>`|Reloads the configuration for the specified unit.|
|`systemctl daemon-reload`|`daemon-reload`|Makes systemd reload all unit configuration files.|
|`systemctl list-jobs`|`list-jobs`|Shows currently queued/running systemd jobs.|
|`systemctl enable unit`|`enable <unit>`|Enables a unit through systemd's dependency mechanism.|
|`systemctl disable unit`|`disable <unit>`|Removes dependency links created when a unit was enabled.|


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