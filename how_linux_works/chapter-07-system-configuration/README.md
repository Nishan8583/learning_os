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