# Understanding Unix Permission Bits (rwx + Special Bits)

This document clarifies how Unix file permission bits work inside `st_mode`, especially how **setuid**, **setgid**, and **sticky bits** relate to the normal `rwx` bits.

---

# ✅ Permission Bit Layout (12 bits total)

`st_mode` permission bits are organized like this:

```
15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
┌──────────┬──────────┬───────────────┬─────────────┬──────────────┐
│ file type│ special  │ user (rwx)    │ group (rwx) │ other (rwx)  │
│  bits    │ bits     │ bits          │ bits        │ bits         │
└──────────┴──────────┴───────────────┴─────────────┴──────────────┘
```

```
BITS:  15 14 13 12   11 10 9    8 7 6    5 4 3    2 1 0
       ───────────   ───────    ─────    ─────    ─────
       File type     Special     User     Group    Other
```
# 🟦 **File type constants and their stored values**

These live in the top bits of `st_mode`.

|File Type|Mask|Stored Octal Value|C Macro to Test|
|---|---|---|---|
|Regular file|`S_IFREG`|0100000|`S_ISREG(mode)`|
|Directory|`S_IFDIR`|0040000|`S_ISDIR(mode)`|
|Character device|`S_IFCHR`|0020000|`S_ISCHR(mode)`|
|Block device|`S_IFBLK`|0060000|`S_ISBLK(mode)`|
|FIFO|`S_IFIFO`|0010000|`S_ISFIFO(mode)`|
|Symlink|`S_IFLNK`|0120000|`S_ISLNK(mode)`|
|Socket|`S_IFSOCK`|0140000|`S_ISSOCK(mode)`
```
0100 000 111 101 101
│    │   │   │   └── others r-x
│    │   │   └────── group r-x
│    │   └────────── user rwx
│    └────────────── special bits (all zero)
└──────────────────── file type bits: 0100 (regular file)
```


# Permission
- **3 special bits**: apply to the whole file  
- **9 rwx bits**: for user, group, and others  

Total = **12 permission bits**

These correspond to the four octal digits commonly seen in modes like:

```
4755
```

Which maps to:

```
[ special ] [ user ] [ group ] [ other ]
     4           7        5        5
```

---

# 🎯 Special Permission Bits (Top Octal Digit Only)

The *highest* octal digit contains **only** the three special bits:

| Bit | Meaning | Octal | C Mask |
|-----|----------|--------|-----------|
| 11 | setuid | 04000 | `S_ISUID` |
| 10 | setgid | 02000 | `S_ISGID` |
| 9 | sticky | 01000 | `S_ISVTX` |

There is **only one copy** of each — **NOT per user/group/other**.

These bits apply to the entire file, not individuals.

---

# 🎯 Normal rwx Bits (User, Group, Other)

Each category has 3 bits:

### USER (bits 8–6)
| Bit | Meaning | Octal | Mask |
|-----|---------|--------|----------|
| 8 | read | 0400 | `S_IRUSR` |
| 7 | write | 0200 | `S_IWUSR` |
| 6 | execute | 0100 | `S_IXUSR` |

### GROUP (bits 5–3)
| Bit | Meaning | Octal | Mask |
|-----|---------|--------|----------|
| 5 | read | 040 | `S_IRGRP` |
| 4 | write | 020 | `S_IWGRP` |
| 3 | execute | 010 | `S_IXGRP` |

### OTHER (bits 2–0)
| Bit | Meaning | Octal | Mask |
|-----|---------|--------|----------|
| 2 | read | 04 | `S_IROTH` |
| 1 | write | 02 | `S_IWOTH` |
| 0 | execute | 01 | `S_IXOTH` |

---

# 🔥 Example: Mode `4755`

```
4755 (octal)
 ↓
4 | 7 | 5 | 5   ← octal digits
```

Convert each to binary:

```
4 → 100   (special bits: setuid on)
7 → 111   (user: rwx)
5 → 101   (group: r-x)
5 → 101   (other: r-x)
```

Full binary:

```
100 111 101 101
└─┘ └──┘ └──┘ └──┘
special user group other
```

### Key point:
### **Only the top group (`100`) contains setuid/setgid/sticky bits.**
User/group/other NEVER contain special bits.

---

# ⭐ Final Summary

- There are **12 permission bits** (3 special + 9 rwx).
- Layout is:

```
setuid | setgid | sticky | user-rwx | group-rwx | other-rwx
```

- The special bits appear **only once**, in the **top octal digit**.
- `4755` means:

  - setuid bit ON  
  - user = `rwx`  
  - group = `r-x`  
  - others = `r-x`  

---
# 🔥 Final full structure (binary + octal)

Example mode: `0104755`
```
bits:   0100  100  111  101  101
        │     │    │    │    │
        │     │    │    │    └─ other (r-x)
        │     │    │    └──── group (r-x)
        │     │    └───────── user (rwx)
        │     └────────────── special (setuid)
        └──────────────────── file type (regular file)
```
Where:

- `0100 0000 0000 0000` ← file type
- `0000 1000 0000 0000` ← setuid
- `0000 0000 1110 0000` ← user bits
- etc.
