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