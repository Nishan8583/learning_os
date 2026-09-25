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
| `systemctl show -p type unit` | `show`                              | Displays properties of a unit.                                             |
| `-p type`                     | `-p` / `--property` + property name | Requests a specific unit property; the excerpt uses `type` in its example. |
| `unit`                        | Unit name                           | Specifies which systemd unit the command operates on.                      |

# Unit-file keywords / directives

Combining this section with the previous material you provided, here is a consolidated reference for the important unit-file keywords.

|Keyword|Section|Description|
|---|---|---|
|`Description=`|`[Unit]`|Human-readable description of the unit.|
|`Documentation=`|`[Unit]`|Specifies documentation associated with the unit.|
|`Requires=`|`[Unit]`|Strict dependency. systemd attempts to activate the dependency; failure can deactivate the dependent unit.|
|`Wants=`|`[Unit]`|Activation dependency. systemd attempts to activate the dependency but doesn't propagate its failure to the dependent unit.|
|`Requisite=`|`[Unit]`|Requires the specified unit to already be active; otherwise activation fails.|
|`Conflicts=`|`[Unit]`|Negative dependency; conflicting units should not be active simultaneously.|
|`Before=`|`[Unit]`|Causes the current unit to activate before the specified unit(s).|
|`After=`|`[Unit]`|Causes the current unit to activate after the specified unit(s).|
|`DefaultDependencies=`|`[Unit]`|Controls whether systemd's automatically added default dependencies are applied. `no` disables them.|
|`ConditionPathExists=`|`[Unit]`|Allows activation only if the specified path exists.|
|`ConditionPathIsDirectory=`|`[Unit]`|Allows activation only if the specified path is a directory.|
|`ConditionFileNotEmpty=`|`[Unit]`|Allows activation only if the specified file exists and is non-empty.|
|`RefuseManualStart=`|`[Unit]`|Prevents manual starting of the unit when set to `yes`.|
|`Type=`|`[Service]`|Defines how systemd should interpret the service's startup behavior.|
|`ExecStart=`|`[Service]`|Command used to start the service.|
|`ExecStartPre=`|`[Service]`|Command executed before the main service process starts.|
|`ExecReload=`|`[Service]`|Command used to reload the service.|
|`EnvironmentFile=`|`[Service]`|Specifies a file from which environment variables can be loaded.|
|`RemainAfterExit=`|`[Service]`|Controls whether a service remains considered active after its process exits. `Type=oneshot` gets `RemainAfterExit=yes` by default according to the source.|
|`Type=simple`|`[Service]`|Service remains as the main process; systemd doesn't wait for an explicit readiness notification.|
|`Type=forking`|`[Service]`|Service forks; systemd expects the original process to terminate before considering startup complete.|
|`Type=notify`|`[Service]`|Service explicitly notifies systemd when it is ready.|
|`Type=dbus`|`[Service]`|Service is considered ready after registering on D-Bus.|
|`Type=oneshot`|`[Service]`|Service runs to completion and terminates; systemd waits for termination before considering startup complete.|
|`Type=idle`|`[Service]`|Similar to `simple`, but startup is delayed until active systemd jobs finish.|
|`EnvironmentFile=`|`[Service]`|Provides environment variables used by commands such as `ExecStart=`.|
|`[Install]`|`[Install]`|Section used by `systemctl enable` to create dependency relationships for activation.|

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