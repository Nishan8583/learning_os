## 1. Linux has two display modes

Historically, Linux could display output in two major ways:

### Text Mode

This is the traditional console interface:

```
Ubuntu 24.04 LTS
server login:
```

No windows, no mouse, just text.

Example:

```
$ ls
file1.txt
file2.txt

$ top
```

This is often called a **console**, **terminal console**, or **TTY**.

---

### Graphical Mode

This is your desktop environment:

- GNOME
- KDE Plasma
- XFCE
- Cinnamon

You get:

- Windows
- Mouse support
- Icons
- Web browsers

Example:

```
+----------------------+
| Firefox              |
|                      |
|     Web Page         |
|                      |
+----------------------+
```

---

## 2. Why you rarely see text mode during boot anymore

Older Linux systems booted entirely in text mode.

You would see messages like:

```
Starting network...
Loading modules...
Starting sshd...
```

Modern distributions hide most of this.

Instead they show:

- Vendor logos
- Progress bars
- Animated splash screens

Commonly using:

Plymouth

Example:

```
[ Ubuntu Logo ]
Loading...
```

The kernel is still producing text messages internally, but Plymouth hides them.

---

## 3. What is a Virtual Console?

A **virtual console** lets one monitor act like multiple independent terminals.

Think of it as:

```
Physical Monitor
       |
  +----+----+----+----+
  | tty1 | tty2 | tty3 |
  +----+----+----+----+
```

Each virtual console has:

- Its own screen contents
- Its own shell
- Its own login session

You can switch between them instantly.

---

## 4. What is tty1, tty2, etc.?

Linux exposes these consoles as device files:

```
/dev/tty1
/dev/tty2
/dev/tty3
...
```

Each corresponds to a virtual console.

For example:

|Virtual Console|Device|
|---|---|
|Console 1|`/dev/tty1`|
|Console 2|`/dev/tty2`|
|Console 3|`/dev/tty3`|

If you're logged into tty1 and switch to tty2:

```
tty1:
nishan@login:~$

tty2:
login:
```

tty1 remains exactly as you left it.

---

## 5. Switching between text consoles

When you're already in text mode:

```
ALT + F1  -> tty1
ALT + F2  -> tty2
ALT + F3  -> tty3
```

Example:

You are running:

```
$ top
```

on tty1.

Press:

```
ALT + F2
```

You move to tty2.

Press:

```
ALT + F1
```

You return to the still-running `top` session.

Nothing was interrupted.

---

## 6. What is getty?

The paragraph mentions:

> Many of these virtual consoles may be occupied by a getty process.

A **getty** is the program that displays:

```
login:
```

and waits for a username.

For example:

```
tty1 -> getty
tty2 -> getty
tty3 -> getty
```

After you log in:

```
login: nishan
Password:
```

getty starts your shell:

```
bash
fish
zsh
```

---

## 7. How graphics mode uses a virtual console

The graphical desktop also occupies a virtual console.

Suppose:

```
tty1 -> login prompt
tty2 -> login prompt
tty3 -> graphical desktop
```

Then:

```
GNOME/KDE/XFCE
```

is actually running on tty3.

The graphical environment "takes over" that virtual console.

---

## 8. Why CTRL+ALT is needed

When the desktop environment is active, keyboard shortcuts like:

```
ALT + F1
```

are often used by the desktop itself.

So Linux requires:

```
CTRL + ALT + F1
CTRL + ALT + F2
CTRL + ALT + F3
```

to switch away from graphics mode.

Example:

```
Desktop (tty3)

CTRL + ALT + F1
```

Switches to:

```
tty1 login console
```

---

## 9. Returning to the graphical desktop

After switching to a text console:

```
CTRL + ALT + F1
```

you can return to graphics by switching back to the TTY where the desktop is running.

Example:

```
tty1 -> text login
tty2 -> text login
tty3 -> GNOME
```

Then:

```
ALT + F3
```

(or sometimes `CTRL + ALT + F3` depending on the system)

returns to GNOME.

---

## 10. Why the note about tty1?

The book says:

> Some distributions use tty1 in graphics mode.

Historically:

```
tty1 -> text
tty2 -> text
tty3 -> graphics
```

Many modern systems now do:

```
tty1 -> graphics
tty2 -> text
tty3 -> text
```

For example, modern Ubuntu often runs the graphical login manager on tty1.

So if:

```
CTRL + ALT + F1
```

doesn't show a text console, try:

```
CTRL + ALT + F2
CTRL + ALT + F3
CTRL + ALT + F4
```

until you find one.

---

## 11. What does `chvt` do?

`chvt` means:

```
Change Virtual Terminal
```

It forces the kernel to switch consoles.

Example:

```
# chvt 1
```

Switches to:

```
/dev/tty1
```

Equivalent to manually pressing:

```
CTRL + ALT + F1
```

Another example:

```
# chvt 3
```

Switches to:

```
/dev/tty3
```

This is useful if:

- Keyboard shortcuts are broken
- You're debugging graphics issues
- You're writing scripts that manipulate consoles

---

## How this looks on a modern Linux desktop

A common setup today might be:

```
tty1  -> GDM login screen / GNOME
tty2  -> text console
tty3  -> text console
tty4  -> text console
tty5  -> text console
tty6  -> text console
```

You are browsing the web in GNOME.

Press:

```
CTRL + ALT + F2
```

You get:

```
Ubuntu login:
```

Log in and run:

```
$ top
```

Press:

```
ALT + F1
```

(or `CTRL + ALT + F1` depending on the system)

and you're back at the graphical desktop.

---

### The key idea

A **virtual console (TTY)** is a separate terminal session managed directly by the Linux kernel. Linux provides multiple virtual consoles (`/dev/tty1`, `/dev/tty2`, etc.) on a single monitor, and both text terminals and graphical desktops run on top of these consoles. The keyboard shortcuts (`Ctrl+Alt+Fn`) simply tell the kernel to switch which virtual console is currently displayed.


---

# More on `/dev/tty*`
Yes, exactly. The `/dev/tty*` devices mentioned in this paragraph are the same TTYs (virtual consoles) discussed in the previous section.

The paragraph is expanding the idea by introducing **three related but different kinds of terminal devices**:

```
Physical monitor/keyboard
        │
        ▼
+-------------------+
| Virtual Consoles  |
| /dev/tty1         |
| /dev/tty2         |
| /dev/tty3         |
+-------------------+

and

+-------------------+
| Terminal Windows  |
| /dev/pts/0        |
| /dev/pts/1        |
| /dev/pts/2        |
+-------------------+
```

## 1. `/dev/tty1`, `/dev/tty2`, ...

These are the virtual consoles we were talking about earlier.

For example:

```
Ctrl+Alt+F1
```

takes you to:

```
/dev/tty1
```

and

```
Ctrl+Alt+F2
```

takes you to:

```
/dev/tty2
```

These are managed directly by the kernel and typically correspond to full-screen text consoles.

You can verify which TTY you're currently on:

```
$ tty
/dev/tty2
```

---

## 2. `/dev/pts/*`

This is where many people get confused.

When you open a terminal emulator in GNOME, KDE, XFCE, etc.:

```
GNOME Terminal
Konsole
xterm
Alacritty
kitty
```

you're **not** connected to `/dev/tty1`.

Instead, the kernel creates a **pseudoterminal (PTY)**.

Example:

```
$ tty
/dev/pts/0
```

Open another terminal window:

```
$ tty
/dev/pts/1
```

Another:

```
$ tty
/dev/pts/2
```

Each terminal window gets its own PTY.

So:

|Device|Meaning|
|---|---|
|`/dev/tty1`|Virtual console|
|`/dev/tty2`|Virtual console|
|`/dev/pts/0`|First terminal emulator window|
|`/dev/pts/1`|Second terminal emulator window|

---

## 3. Why do PTYs exist?

Historically Linux had real terminals:

```
Computer
   │
   ├── Physical Terminal #1
   ├── Physical Terminal #2
   └── Physical Terminal #3
```

Each terminal was a separate hardware device.

Modern systems emulate those terminals in software.

For example:

```
GNOME Terminal Window
        │
        ▼
    /dev/pts/0
```

The shell thinks it's talking to a real terminal, but it's actually talking to a software-emulated one.

---

## 4. What is `/dev/tty`?

This is the clever one.

`/dev/tty` means:

> "the controlling terminal of the current process"

It's not a specific terminal.

Instead it resolves dynamically.

For example:

### If you're on tty2

```
$ tty
/dev/tty2
```

then:

```
$ ls -l /dev/tty
```

represents:

```
/dev/tty2
```

for that process.

---

### If you're in GNOME Terminal

```
$ tty
/dev/pts/0
```

then:

```
/dev/tty
```

effectively refers to:

```
/dev/pts/0
```

for that process.

---

## Example

Suppose you're on tty2:

```
$ echo "hello" > /dev/tty
```

Output:

```
hello
```

appears on tty2.

Now run the same command from a GNOME terminal:

```
$ echo "hello" > /dev/tty
```

Output:

```
hello
```

appears in that terminal window.

The same command works because `/dev/tty` means "my terminal."

---

## Why is this useful?

Programs don't need to know:

```
Am I on tty1?
Am I on tty4?
Am I on pts/7?
```

They can simply use:

```
/dev/tty
```

and the kernel figures it out.

This is used by many utilities.

Example:

```
sudo
ssh
passwd
```

Even if their stdin is redirected, they can still talk directly to the user's terminal via `/dev/tty`.

---

## "A process does not need to be attached to a terminal"

This sentence is important.

Many processes have **no terminal at all**.

Examples:

- Web servers
- Database servers
- Systemd services
- Daemons

For example:

```
nginx
postgres
sshd
```

These often run in the background and are not connected to any TTY.

If such a process tries to access:

```
/dev/tty
```

it will fail because there is no controlling terminal.

---

### Mental model

Think of it this way:

```
TTY devices
│
├── /dev/tty1
├── /dev/tty2
├── /dev/tty3
│
├── /dev/pts/0
├── /dev/pts/1
├── /dev/pts/2
│
└── /dev/tty
      ^
      |
      "whatever terminal this process is using"
      
```

So yes—the `/dev/tty1` in this paragraph is the same virtual console device that becomes visible when you press `Ctrl+Alt+F1` (or another function key, depending on your distribution). The new concept introduced here is that graphical terminal windows are usually backed by `/dev/pts/*` pseudoterminals rather than the virtual consoles `/dev/tty1`, `/dev/tty2`, etc.