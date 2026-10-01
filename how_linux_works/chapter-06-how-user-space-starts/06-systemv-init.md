# Chapter 6: How User Space Starts — System V Runlevels, Init, Shutdown, and Emergency Booting

## 1. System V Runlevels

A runlevel represents a particular operating state of a Linux system, defined by the set of processes and services running at a given time.

- System V init uses runlevels numbered 0 through 6.
    
- A system typically stays in one runlevel during normal operation.
    
- When shutting down, init switches to a different runlevel to terminate services and stop the kernel.
    

### Common runlevel purposes

|Runlevel|Purpose|
|---|---|
|0|Halt / shut down|
|1|Single-user mode|
|2–4|Traditionally used for text-console or multiuser configurations, depending on distribution|
|5|Typically starts a GUI login|
|6|Reboot|

Checking the current runlevel

```
who -r
```

Example output:

```
run-level 5  2019-01-27 16:43
```

This indicates:

- Current runlevel: `5`
    
- Date and time when the runlevel was established.
    

Runlevels and systemd

- Although systemd supports runlevels, it considers them obsolete as system end states.
    
- systemd uses target units instead.
    
- Runlevels mainly remain relevant to systemd for compatibility with legacy System V init scripts.
    

## 2. System V init

System V init is one of the oldest Linux initialization systems. Its primary purpose is to bring the system into a desired runlevel through an organized startup sequence.

It is now uncommon on modern desktop and server systems, but can still be encountered in:

- Older RHEL versions (before 7.0).
    
- Embedded Linux environments, such as routers and phones.
    
- Legacy software packages that provide System V startup scripts.
    

systemd can support these legacy scripts through its System V compatibility mode.

### 2.1 Main components

A typical System V init installation consists of:

- Central configuration file: `/etc/inittab`
    
- Startup scripts: Usually located in `/etc/init.d/`
    
- Runlevel directories: Such as `/etc/rc5.d/`
    
- Symbolic link farm: Links scripts in the runlevel directories to their actual files in `/etc/init.d/`
    

### 2.2 The `/etc/inittab` configuration file

The `/etc/inittab` file defines the initial runlevel and the actions init should perform.

Example:

```
id:5:initdefault:
```

This specifies runlevel `5` as the default runlevel.

Each `inittab` entry has four colon-separated fields:

```
id:runlevels:action:process
```

|Field|Description|
|---|---|
|ID|Unique identifier for the entry, usually a short string|
|Runlevels|Runlevel numbers to which the entry applies|
|Action|Defines when and how init executes the command|
|Process|Command to execute; optional for some actions|

### 2.3 Common `inittab` actions

|Action|Description|
|---|---|
|`initdefault`|Specifies the default runlevel.|
|`wait`|Runs a command once when entering the specified runlevel and waits for it to finish before proceeding.|
|`respawn`|Runs a command and restarts it if it terminates.|
|`ctrlaltdel`|Defines what happens when `Ctrl + Alt + Del` is pressed on a virtual console.|
|`sysinit`|Runs a command during init startup, before entering any runlevel.|

Example:

```
l5:5:wait:/etc/rc.d/rc 5
```

Breakdown:

- `l5`: Entry identifier.
    
- `5`: Applies to runlevel 5.
    
- `wait`: Run once upon entering runlevel 5 and wait for completion.
    
- `/etc/rc.d/rc 5`: Executes the runlevel 5 startup sequence.
    

The `rc` command executes scripts from the appropriate runlevel directory, such as `/etc/rc5.d/`, in numeric order.

Example: virtual console login

```
1:2345:respawn:/sbin/mingetty tty1
```

- Applies to runlevels 2, 3, 4, and 5.
    
- Starts a login prompt on the first virtual console (`/dev/tty1`).
    
- Uses `respawn` to bring back the login prompt after a user logs out or the process terminates.
    

## 3. System V Startup Command Sequence

System V init starts services by executing scripts associated with the current runlevel.

For example, the following entry in `/etc/inittab` triggers the runlevel 5 sequence:

```
l5:5:wait:/etc/rc.d/rc 5
```

The `5` argument identifies the runlevel. Its startup scripts are typically located in either:

```
/etc/rc.d/rc5.d/
/etc/rc5.d/
```

Other runlevels use corresponding directories, such as `rc1.d` or `rc2.d`.

### 3.1 Startup script naming convention

Example directory contents:

```
S10sysklogd
S12kerneld
S15netstd_init
S18netbase
S20ppp
S99httpd
S99sshd
```

The naming convention determines the execution order and action.

|Component|Meaning|
|---|---|
|`S`|Start the service|
|`K`|Kill or stop the service|
|Number (`00–99`)|Determines the execution order|
|Remaining name|Identifies the service or script|

For example:

```
S10sysklogd
S12kerneld
S99sshd
```

The `rc` command executes the scripts in numeric order, passing the `start` argument to `S` scripts.

Equivalent execution examples:

```
S10sysklogd start
S12kerneld start
S99sshd start
```

Scripts beginning with `K` are executed with the `stop` argument instead. These are commonly found in runlevels associated with system shutdown.

Most `rc*.d` entries are shell scripts or symbolic links to shell scripts that start programs in `/sbin` or `/usr/sbin`.

To understand what a script does, inspect it using a pager:

```
less /etc/init.d/httpd
```

## 4. The System V init Link Farm

The scripts in the runlevel directories are generally symbolic links to actual service scripts in `/etc/init.d/`.

Example:

```
/etc/rc5.d/S99httpd -> ../init.d/httpd
```

This means:

- `/etc/rc5.d/S99httpd` is a symbolic link.
    
- The actual script is `/etc/init.d/httpd`.
    
- The link determines that the script should start in runlevel 5 and its position in the startup sequence.
    

A collection of symbolic links distributed across several runlevel directories is called a link farm.

### Why use a link farm?

- The same service script can be reused across multiple runlevels.
    
- Each runlevel can have a different combination of enabled services.
    
- Startup ordering can be adjusted without duplicating the actual scripts.
    

### 4.1 Starting and stopping services manually

Instead of directly executing a runlevel link, use the actual script in `/etc/init.d/`.

Start a service:

```
/etc/init.d/httpd start
```

Stop a service:

```
/etc/init.d/httpd stop
```

The script accepts arguments such as `start` and `stop` to determine the requested operation.

### 4.2 Modifying the boot sequence

The startup sequence can be modified by changing symbolic links in the appropriate `rc*.d` directory.

Disabling a service

Rather than deleting a symbolic link, rename it:

```
mv /etc/rc5.d/S99httpd /etc/rc5.d/_S99httpd
```

Why this works:

- The `rc` command looks for scripts beginning with `S` or `K`.
    
- The renamed link begins with `_`, so it is ignored.
    
- The original service name and ordering information remain visible.
    

Adding a service

1. Create a startup script in `/etc/init.d/`.
    
2. Follow the structure of an existing service script.
    
3. Create a symbolic link in the appropriate runlevel directory.
    
4. Select a suitable startup position based on service dependencies.
    

For nonessential services, administrators often use numbers in the `90s`, so those services start after most of the system-provided services.

Important: Starting a service too early can cause failures if it depends on another service that has not started yet.

## 5. `run-parts`

`run-parts` is a utility that executes a collection of executable programs in a directory, generally in a predictable order.

It is used by various Linux systems, including systems that do not use System V init.

### Main characteristics

- Executes programs in a specified directory.
    
- Normally runs all eligible programs.
    
- Some implementations support selecting programs through regular expressions.
    
- Some implementations allow arguments to be passed to the executed programs.
    

For example, Debian and Ubuntu implementations may support a regular expression such as:

```
S[0-9]{2}
```

This can select startup scripts whose names begin with `S` followed by two digits.

Different distributions implement `run-parts` differently. Fedora's implementation, for example, is described in the source as relatively simple.

Key takeaway: `run-parts` is primarily a directory-based program execution utility. It also appears in scripts and can be used to organize the execution of startup or maintenance tasks.

## 6. System V init Control with `telinit`

The `telinit` command controls the System V init process.

It can be used to:

- Switch runlevels.
    
- Reload the `/etc/inittab` configuration.
    
- Enter single-user mode.
    

### Common commands

Switch to runlevel 3

```
telinit 3
```

When switching runlevels, init attempts to terminate processes that are not defined for the new runlevel. Changing runlevels can therefore stop active services.

Reload `/etc/inittab`

```
telinit q
```

Use this after modifying `/etc/inittab` to make init reread its configuration.

Enter single-user mode

```
telinit s
```

This switches the system to single-user mode.

## 7. systemd System V Compatibility

systemd supports legacy System V init scripts by associating them with systemd service units.

### Compatibility process

1. systemd activates `runlevel<N>.target`, where `N` is the requested runlevel.
    
2. It examines the symbolic links in `/etc/rc<N>.d/` and identifies the associated scripts in `/etc/init.d/`.
    
3. It associates each script with a corresponding service unit. For example, `/etc/init.d/foo` becomes `foo.service`.
    
4. It activates the service unit and runs the script with either the `start` or `stop` argument, based on the link name.
    
5. It attempts to associate processes started by the script with the corresponding service unit.
    

### Benefits and limitations

Benefits

- Legacy services can be managed through systemd.
    
- You can use `systemctl` to inspect the status of or restart compatible services.
    
- systemd attempts to track processes started by legacy scripts.
    

Limitations

- Compatibility mode does not eliminate the limitations of the original startup scripts.
    
- System V init scripts may still execute serially, rather than taking full advantage of systemd's service-starting capabilities.
    

## 8. Shutting Down Your System

The `shutdown` command is the standard way to shut down or reboot a Linux machine, regardless of the init implementation.

### 8.1 Basic shutdown commands

Halt the system immediately

```
shutdown -h now
```

- `-h`: Halt the system.
    
- `now`: Start the shutdown process immediately.
    

On most systems, a halt also cuts power to the machine.

Reboot the system

```
shutdown -r now
```

- `-r`: Reboot instead of halting.
    
- `now`: Start immediately.
    

Reboot after 10 minutes

```
shutdown -r +10
```

- `+10`: Wait 10 minutes before starting the shutdown process.

The shutdown command requires a time argument, which can be `now` or a future time.

### 8.2 What happens during shutdown?

The shutdown process generally follows these steps:

1. init asks running processes to shut down cleanly.
    
2. Processes that do not respond are sent a `TERM` signal.
    
3. Processes that still do not terminate are sent a `KILL` signal.
    
4. The system locks system files into place and performs other shutdown preparations.
    
5. All filesystems except the root filesystem are unmounted.
    
6. The root filesystem is remounted read-only.
    
7. Buffered data is written to storage using `sync`.
    
8. The kernel is instructed to reboot or stop, typically through the `reboot(2)` system call.
    

### 8.3 The `/etc/nologin` file

When a shutdown is scheduled for a future time, the `shutdown` command creates:

```
/etc/nologin
```

While this file exists:

- Ordinary users are prevented from logging in.
    
- The superuser is exempt.
    

The file helps prevent new user sessions from starting while the system is preparing to shut down.

### 8.4 `reboot`, `halt`, and `poweroff`

These commands can behave differently depending on the system's current state.

- By default, they call `shutdown` with the appropriate `-r` or `-h` option.
    
- If the system is already at a halt or reboot runlevel, they may instruct the kernel to stop or reboot immediately.
    
- The `-f` (force) option can be used to force a faster shutdown, but it risks filesystem damage or data loss if the normal shutdown procedure is bypassed.
    

## 9. The Initial RAM Filesystem (initramfs)

The initial RAM filesystem (initramfs) is a temporary user-space environment loaded into memory during boot, before the normal root filesystem becomes available.

Its main purpose is to provide the drivers and utilities required to mount the real root filesystem.

### 9.1 Why initramfs is necessary

Linux systems support many types of storage hardware and configurations.

For example:

- A root filesystem may reside on a RAID array.
    
- The RAID controller may require a driver that is not built into the kernel.
    
- The driver may be available only as a loadable kernel module.
    

This creates a bootstrapping problem:

- The kernel needs the driver to access the root filesystem.
    
- The driver module is stored as a file on the root filesystem.
    
- The kernel cannot access that file until it has mounted the root filesystem.
    

initramfs solves this problem by providing a temporary filesystem containing the necessary drivers and tools.

### 9.2 initramfs boot sequence

Boot loader

Loads the kernel and initramfs archive into memory

Linux kernel starts

Reads the archive and mounts the temporary RAM filesystem at `/`

Temporary init environment

Starts utilities and loads required kernel modules and drivers

Mount the real root filesystem

Makes the actual system root available

Handoff to the real init

The system continues its normal user-space startup

### 9.3 Distribution differences

The implementation of initramfs varies between distributions.

- Some distributions use a relatively simple shell script as the temporary init process.
    
- This script may start `udevd`, load drivers, mount the real root filesystem, and execute the real init.
    
- Distributions using systemd may include a more complete systemd installation in the initramfs environment, along with selected udev configuration files.
    

### 9.4 Bypassing initramfs

If the kernel already contains all the drivers required to mount the real root filesystem, initramfs may not be necessary.

In theory, removing initramfs from the boot process can slightly shorten boot time.

The source describes a way to test this using the GRUB menu editor:

1. Enter the GRUB menu editor during boot.
    
2. Remove the `initrd` line from the boot entry.
    
3. Attempt to boot the system.
    

Caution: This may fail if the kernel needs drivers or features supplied by initramfs. Modern generic distribution kernels often depend on initramfs for features such as mounting filesystems by UUID. Avoid experimenting by permanently modifying GRUB configuration unless you know how to recover from a boot failure.

### 9.5 Inspecting and creating initramfs

Common tools mentioned in the source:

|Tool|Purpose|
|---|---|
|`unmkinitramfs`|Unpacks archives created by `mkinitramfs`.|
|`mkinitramfs`|Creates initial RAM filesystem images.|
|`dracut`|Another common tool for generating initramfs images.|
|`cpio`|Archive format and utility associated with older or manually inspected initramfs archives.|

A common archive-inspection workflow is to unpack the initramfs archive using the appropriate tool for the distribution.

### 9.6 initramfs vs. initrd

|Feature|initramfs|initrd|
|---|---|---|
|Meaning|Initial RAM filesystem|Initial RAM disk|
|Underlying format|Typically a `cpio` archive|A disk image|
|Implementation|Uses an archive as the source of the temporary filesystem|Uses a disk image as the basis of the temporary filesystem|
|Status|Modern approach|Older approach, largely fallen into disuse|
|Naming|Often called initramfs|The term is still commonly used in filenames and configuration, sometimes even for cpio-based initramfs|

The distinction is important because the two terms are often used interchangeably in practical Linux documentation.

## 10. Emergency Booting and Single-User Mode

When a Linux system becomes unbootable or encounters serious configuration problems, recovery options include:

- Booting from a live Linux image.
    
- Using a dedicated rescue image.
    
- Entering single-user mode.
    

### 10.1 Live and rescue environments

A live image is a Linux system that can boot and run without being installed on the machine.

Many Linux installation images also provide a live environment.

A dedicated rescue image, such as SystemRescueCD, can also be used to recover a damaged system.

Common recovery tasks include:

- Checking filesystems after a crash.
    
- Resetting a forgotten password.
    
- Fixing critical configuration files, such as `/etc/fstab` and `/etc/passwd`.
    
- Restoring files from backups after a system crash.
    

### 10.2 Single-user mode

Single-user mode provides a minimal system environment that boots directly to a root shell rather than starting the full collection of services.

|Init system|Single-user equivalent|
|---|---|
|System V init|Runlevel 1|
|systemd|`rescue.target`|

You can normally request single-user mode through the boot loader by supplying the `-s` parameter.

Depending on the system configuration, you may need the root password to enter this mode.

### 10.3 Limitations of single-user mode

Single-user mode is intended for minimal recovery, not normal system use.

Common limitations include:

- Network access is usually unavailable or difficult to use.
    
- There is no graphical user interface.
    
- Terminal functionality may be limited.
    
- Many services and supporting tools are not running.
    

For these reasons, a live or rescue image is often preferable for complex recovery operations.