# Journalctl Commands

The following table consolidates the commands covered in the source material, including common examples and their purposes.

|Command|Explanation|
|---|---|
|`journalctl`|Displays journal messages, usually through a pager.|
|`journalctl _PID=8792`|Filters journal messages by process ID.|
|`journalctl -S -4h`|Displays messages from the past four hours.|
|`journalctl -S 06:00:00`|Displays messages since 6 AM today.|
|`journalctl -S 2020-01-14`|Displays messages since the specified date.|
|`journalctl -S '2020-01-14 14:30:00'`|Displays messages since a specific date and time.|
|`journalctl -U '2020-01-14 15:30:00'`|Sets the upper time boundary for displayed messages.|
|`journalctl -u cron.service`|Filters messages by systemd unit.|
|`journalctl -u cron`|Filters by unit without specifying the `.service` suffix.|
|`journalctl -F _SYSTEMD_UNIT`|Lists distinct values for the `_SYSTEMD_UNIT` field.|
|`journalctl -N`|Lists available journal fields.|
|`journalctl -g 'kernel.*memory'`|Searches message text using a regular expression, if supported.|
|`journalctl -b`|Displays logs from the current boot.|
|`journalctl -b -1`|Displays logs from the previous boot.|
|`journalctl --list-boots`|Lists recorded boot sessions and their IDs.|
|`journalctl -b <boot-id>`|Displays logs for a particular boot ID.|
|`journalctl -r -b -1`|Displays previous-boot messages in reverse chronological order.|
|`journalctl -k`|Displays kernel messages.|
|`journalctl -k -b`|Displays kernel messages from the current boot.|
|`journalctl -p 3`|Displays messages with priorities 0 through 3.|
|`journalctl -p 2..3`|Displays messages with priorities 2 and 3 only.|
|`journalctl -f`|Follows the journal and prints new messages as they arrive.|
|`journalctl -u ssh.service -f`|Follows new messages for the SSH service.|
|`tail -f /var/log/syslog`|Follows a traditional plaintext logfile as new entries are written.|
|`less +F /var/log/syslog`|Opens a logfile in follow mode.|
|`last`|Displays login history using records such as those stored in `wtmp`.|
|`lastlog`|Displays users' most recent login information.|

## Quick reference: Important options

|Option|Meaning|Example|
|---|---|---|
|`-S`|Since: start time|`journalctl -S -4h`|
|`-U`|Until: end time|`journalctl -U 06:00:00`|
|`-u`|Filter by systemd unit|`journalctl -u ssh`|
|`-F`|List distinct values for a field|`journalctl -F _SYSTEMD_UNIT`|
|`-N`|List available fields|`journalctl -N`|
|`-g`|Filter messages by regular expression|`journalctl -g 'error.*failed'`|
|`-b`|Select a boot session|`journalctl -b -1`|
|`-r`|Reverse chronological order|`journalctl -r`|
|`-k`|Show kernel messages|`journalctl -k`|
|`-p`|Filter by priority|`journalctl -p 3`|
|`-f`|Follow new messages|`journalctl -f`|

# /etc  Configuration Files and Commands

The following table consolidates the file paths, example configurations, their meanings, and the commands covered in the excerpt.

## Configuration files

| Filepath            | Example configuration                                    | What the configuration indicates                                                                                    |
| ------------------- | -------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------- |
| `/etc/passwd`       | `juser:x:3119:1000:J. Random User:/home/juser:/bin/bash` | Defines the account's username, password placeholder, UID, primary GID, real name, home directory, and login shell. |
| `/etc/shadow`       | `juser:$6$example_hash:20000:0:99999:7:::`               | Illustrative entry showing a password hash and password-aging fields. Stores sensitive authentication data.         |
| `/etc/group`        | `disk:*:6:juser,beazley`                                 | Defines the `disk` group with GID `6` and lists additional members `juser` and `beazley`.                           |
| `/etc/systemd/`     | Example: `/etc/systemd/system/myservice.service`         | Holds local systemd configuration, including possible service overrides.                                            |
| `/etc/grub.d/`      | Example: `40_custom`                                     | Contains GRUB configuration scripts and customization files.                                                        |
| `/etc/network/`     | Distribution-specific network configuration              | Holds network configuration for systems using this directory.                                                       |
| `/usr/lib/systemd/` | Example: `systemd-journald.service`                      | Contains packaged systemd unit files and system defaults rather than local customizations.                          |

Note: The `/etc/shadow` entry is illustrative, not copied from the source. The exact format and supported configuration locations can vary by distribution.

## Commands

|Command|Example usage|Explanation|
|---|---|---|
|`ls -F`|`ls -F /etc`|Lists `/etc` entries and appends indicators such as `/` to directory names.|
|`ps`|`ps ao args \\| grep getty`|Displays process arguments and filters for processes containing `getty`.|
|`passwd`|`passwd`|Changes the current user's password.|
|`passwd`|`sudo passwd juser`|Sets the password for the specified user with administrative privileges.|
|`chfn`|`chfn juser`|Changes the user's real name and related GECOS information.|
|`chsh`|`chsh -s /bin/bash juser`|Changes the user's login shell. The selected shell must be listed in `/etc/shells`.|
|`adduser`|`sudo adduser juser`|Adds a user account.|
|`userdel`|`sudo userdel juser`|Removes a user account.|
|`vipw`|`sudo vipw`|Safely edits `/etc/passwd`, using locking and backup precautions.|
|`vipw -s`|`sudo vipw -s`|Safely edits `/etc/shadow`.|
|`groups`|`groups`|Displays the groups to which the current user belongs.|
|`exec()`|Used internally by `login`|Replaces the current process image with the user's shell after successful authentication.|

Key takeaways

- `/etc` is intended primarily for machine-specific configuration and local customization.
    
- `/etc/passwd` maps usernames to UIDs and provides account details; it does not normally store actual password hashes.
    
- `/etc/shadow` stores sensitive password authentication and expiration information.
    
- `/etc/group` defines groups and their membership.
    
- Use dedicated account-management utilities instead of editing user and group files directly.
    
- `getty`, `login`, and PAM participate in traditional terminal login and authentication.

---


# Time and Scheduled Jobs: Configuration Files and Commands

## Configuration files

| Filepath                                 | Example configuration                                                         | What the configuration indicates                                                                      |
| ---------------------------------------- | ----------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------- |
| `/etc/localtime`                         | Symbolic link to `/usr/share/zoneinfo/America/Vancouver`                      | Determines the system's local time zone. The file is typically binary time-zone data or a link to it. |
| `/usr/share/zoneinfo/`                   | `America/Vancouver`                                                           | Contains time-zone definitions and aliases used to interpret local time.                              |
| `/etc/systemd/timesyncd.conf`            | `[Time]` followed by server settings such as `NTP=...`                        | Configures systemd-timesyncd, including its network time servers.                                     |
| `/etc/crontab`                           | `42 6 * * * root /usr/local/bin/cleansystem > /dev/null 2>&1`                 | Schedules a system-wide task to run as root at 6:42 AM every day.                                     |
| `/etc/cron.d/`                           | `15 9 * * * root /usr/local/bin/cleanup`                                      | Holds additional system-wide cron configuration files, including an explicit execution user.          |
| `/etc/cron.daily/`                       | A shell script such as `logrotate`                                            | Contains scripts commonly executed by a scheduled daily cron job.                                     |
| `/var/spool/cron/crontabs/`              | `15 09 * * * /home/juser/bin/spmake`                                          | Typical storage location for individual user crontabs.                                                |
| `/etc/systemd/system/loggertest.timer`   | `OnCalendar=*-*-* *:00,20,40`                                                 | Defines a timer that activates a service every 20 minutes.                                            |
| `/etc/systemd/system/loggertest.service` | `Type=oneshot` and `ExecStart=/usr/bin/logger -p local3.debug "I'm a logger"` | Defines the command that the timer activates and specifies that it runs to completion.                |

## Commands

|Command|Example usage|Explanation|
|---|---|---|
|`date`|`date`|Displays the current date and time in the configured local time zone.|
|`date +%s`|`date +%s`|Displays the current Unix timestamp in seconds.|
|`hwclock`|`sudo hwclock --systohc --utc`|Copies the current system clock to the hardware RTC, interpreting it as UTC.|
|`adjtimex`|`adjtimex`|Utility for inspecting or adjusting kernel clock parameters and timekeeping behavior.|
|`tzselect`|`tzselect`|Helps identify an appropriate time zone.|
|`ln`|`sudo ln -sf /usr/share/zoneinfo/America/Vancouver /etc/localtime`|Creates or replaces a symbolic link to set the local time-zone data file.|
|`export`|`export TZ=US/Central`|Sets a time-zone environment variable for the current shell and its child processes.|
|`TZ`|`TZ=US/Central date`|Runs a single command with a temporary time-zone override.|
|`crontab`|`crontab myjobs`|Installs a file as the current user's crontab.|
|`crontab -l`|`crontab -l`|Lists the current user's scheduled jobs.|
|`crontab -e`|`crontab -e`|Edits the current user's crontab.|
|`crontab -r`|`crontab -r`|Removes the current user's crontab.|
|`journalctl`|`journalctl -f -u loggertest.service`|Follows journal messages associated with a specific service unit.|
|`logger`|`logger -p local3.debug "I'm a logger"`|Sends a message to the system logging facility with the specified priority.|

---

# One Time Scheduling Commands

|Command|Explanation|
|---|---|
|`at 22:30`|Schedules a one-time job for 10:30 PM.|
|`at 22:30 30.09.26`|Schedules a job for 10:30 PM on September 30, 2026.|
|`atq`|Lists pending `at` jobs.|
|`atrm 2`|Removes the scheduled `at` job with ID `2`.|
|`systemd-run --on-calendar='2026-10-05 18:00' /bin/echo test`|Creates a transient timer to run a command at a specific future date and time.|
|`systemd-run --on-active=30m /bin/echo test`|Schedules a command to run 30 minutes after timer activation.|
|`systemd-run --user --on-active=30m /bin/echo test`|Creates a relative-delay timer under the current user's systemd manager.|
|`systemctl list-timers`|Lists system-level timers and their activation information.|
|`systemctl --user list-timers`|Lists timers managed by the current user's systemd manager.|
|`loginctl enable-linger`|Enables lingering for the current user.|
|`loginctl enable-linger user`|Enables lingering for the specified user; normally requires root privileges.|

### Filepaths and Configuration

The examples in this section primarily use commands to create jobs dynamically. They do not require manually created configuration files. `at` stores its queued jobs internally, while `systemd-run` creates transient units managed by systemd.

The following table summarizes the relevant files and unit concepts.

|Filepath / unit|Example configuration or content|What it indicates|
|---|---|---|
|`/var/spool/at/`|Internally managed queued job files|Storage location commonly used by `at` for pending jobs. Exact implementation and location may vary by distribution.|
|`/run/systemd/transient/run-example.timer`|`OnCalendar=2026-10-05 18:00:00`|Illustrative timer-unit setting for a scheduled calendar activation. Transient unit names are generated dynamically; this is not a file you normally create manually.|
|`/run/systemd/transient/run-example.service`|`ExecStart=/bin/echo test`|Illustrative service-unit setting specifying the command executed when the timer activates.|
|`~/.config/systemd/user/`|`backup.timer`, `backup.service`|Conventional location for persistent user-level systemd unit files, if you choose to create permanent timers instead of using transient units.|
|`/etc/systemd/system/`|`backup.timer`, `backup.service`|Conventional location for persistent system-level unit files, typically managed by an administrator.|

---

# 9. Quick Reference: User Access Commands and Configuration Files

## 9.1 Commands and Functions

| Command / function                 | Explanation                                                                                |
| ---------------------------------- | ------------------------------------------------------------------------------------------ |
| `ls -l /usr/bin/sudo`              | Inspects file permissions and ownership to identify the setuid bit on `sudo`.              |
| `ps -eo pid,euser,ruser,comm`      | Lists process IDs, effective usernames, real usernames, and command names.                 |
| `man 2 setuid`                     | Displays the manual page for the `setuid()` system call.                                   |
| `geteuid()`                        | Retrieves the effective UID of the calling process.                                        |
| `getpwuid(uid)`                    | Retrieves user account information associated with a UID.                                  |
| `man -k pam_`                      | Searches manual page descriptions for PAM-related entries.                                 |
| `locate pam_unix.so`               | Locates indexed files matching the PAM Unix module name.                                   |
| `man pam_unix`                     | Displays documentation for the Unix PAM module.                                            |
| `grep password.*unix /etc/pam.d/*` | Searches PAM configuration files for password-related Unix module rules.                   |
| `sudo visudo`                      | Safely edits the sudoers configuration, including settings such as `Defaults stay_setuid`. |

## 9.2 Filepaths and Configuration

| Filepath                    | Example configuration / content                 | What it indicates                                                                                          |
| --------------------------- | ----------------------------------------------- | ---------------------------------------------------------------------------------------------------------- |
| `/etc/passwd`               | `alice:x:1000:1000:Alice:/home/alice:/bin/bash` | Defines account information, including username, UID, GID, home directory, and login shell.                |
| `/etc/shadow`               | `alice:$6$...:...`                              | Stores password hashes and password-aging information for local accounts.                                  |
| `/etc/sudoers`              | `Defaults stay_setuid`                          | Configures `sudo` to preserve its setuid behavior instead of changing the real UID in the usual way.       |
| `/etc/shells`               | `/bin/bash`                                     | Lists valid login shells; used by modules such as `pam_shells.so`.                                         |
| `/etc/pam.d/chsh`           | `auth requisite pam_shells.so`                  | Illustrates a PAM authentication rule that requires a valid shell.                                         |
| `/etc/pam.d/login`          | `auth ... pam_unix.so`                          | Illustrative location for login authentication rules. Exact contents vary by distribution.                 |
| `/etc/pam.d/passwd`         | `password ... pam_unix.so`                      | Contains password-changing rules for the `passwd` service.                                                 |
| `/etc/pam.d/cron`           | `account ...`                                   | Contains PAM rules used by the cron service, where PAM integration is configured.                          |
| `/etc/pam.d/other`          | `auth required pam_deny.so`                     | Defines default PAM behavior for services without a dedicated configuration file.                          |
| `/etc/pam.conf`             | `auth requisite pam_shells.so`                  | Older, centralized PAM configuration format used on some systems.                                          |
| `/etc/login.defs`           | `ENCRYPT_METHOD SHA512`                         | Illustrative password-hashing setting for traditional shadow password tools.                               |
| `/etc/security/`            | Module-specific configuration files             | Holds additional configuration used by certain PAM modules.                                                |
| `/lib/security/pam_unix.so` | Shared PAM module                               | Illustrative module location for Unix authentication; actual path varies by distribution and architecture. |