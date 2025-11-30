# 🧠 TL;DR

Flash memory stores bits by **trapping electrons inside an insulated “floating gate.”**  
Those electrons stay there for **years** because they are surrounded by a special insulating oxide that prevents them from leaking out.

- **Lots of electrons = 0**
- **No electrons = 1**
    

This trapped charge changes the transistor’s threshold voltage, allowing it to represent data even when **power is off**.

---

# 🔬 Big Picture: Flash Uses Charge Storage

Flash cells use **electric charge**, not magnetism like hard drives.

A single flash memory cell is essentially a specially-designed MOSFET transistor:


```Control Gate
     |
  +--+----+
  | Floating Gate | ← stores electrons
  +-------+
     |
Source -- Channel -- Drain
```


### Key feature:

The **floating gate** is fully surrounded by insulating oxide (SiO₂).  
Electrons placed there cannot easily escape → **non-volatility**.


# ⚡ How Data is Written (Program Operation)

To store a `0`:

1. A large voltage is applied to the **control gate**.
2. Electrons gain high energy and tunnel through the oxide layer.
3. They get trapped in the **floating gate**.
    
This is called **Fowler–Nordheim tunneling** (quantum tunneling).

Once trapped, electrons:
- shift the transistor's threshold voltage
- remain there for _years_ until erased
This is a programmed cell → **represents 0**.

---
# 🔥 How Erase Works (Remove Electrons)

Erase applies a huge negative voltage to pull electrons **out** of the floating gate:
- electrons tunnel back through the thin oxide
- floating gate becomes empty again → **represents 1**
This erase only works on a **whole block** of cells at once, which is why SSDs must erase blocks (not pages).

---

# 🔍 How Reading Works

When reading, the SSD applies a small test voltage and checks whether the transistor conducts:
- **Floating gate empty (1):** transistor turns on at low voltage
- **Floating gate charged (0):** transistor needs higher voltage to turn on, or won’t turn on at all

So the drive measures the **threshold voltage**, not the electrons directly.

---

# 💾 Why Data Persists

The floating gate is surrounded by oxide insulation.

This oxide:
- is extremely high-quality
- has very low leakage
- keeps electrons trapped 10–20 years
Even without power, the electrons remain where they are.
Flash = **non-volatile** because electrons are stuck until erased.

---

# 🔍 Comparison to Hard Drives

|Property|Hard Disk (HDD)|Flash (SSD)|
|---|---|---|
|Storage mechanism|Magnetism|Electric charge|
|Unit|Magnetic domains|Floating-gate transistors|
|Write process|Change magnetic polarity|Tunnel electrons into gate|
|Erase|Overwrite sector|Remove electrons from _entire block_|
|Persistence|Magnetic orientation stays|Charge stays trapped|
|Speed|Mechanical|Electronic|

HDDs use “magnetic bumps,” yes — each bit is a small magnetic region pointing ↑ or ↓.

SSDs use **number of electrons** in a gate, not orientation of a magnetic field.

---

# 🎨 Visual Intuition

Think of the floating gate like a tiny bucket inside a sealed glass box:
- You can shoot electrons inside using high voltage
- They get trapped
- They stay for years
- Only a huge “erase voltage” can pull them back out
---

# 🧨 Why Flash Wears Out

Because repeated programming & erasing:
- damages the thin oxide
- causes it to leak
- eventually the floating gate cannot reliably hold electrons
This is why SSDs have limited write cycles.

---

# 🧠 Why Flash Needs to Erase a Block Before Writing

**Short answer:**  
Because **NAND flash can only change bits from 1 → 0 when writing**, and **cannot change 0 → 1 without an erase operation**.  
Erase operations only work **at the block level**, not per-page.

---

# 🔬 The Electrical Explanation (What’s actually happening)

NAND flash stores data in cells using **floating-gate transistors**:

- **1 bit (or more)** is stored in each cell by controlling the amount of trapped electrons.
    
- To **program** (write), electrons are _added_, which changes cell from **1 → 0**.
    
- To **erase**, electrons are _removed_, which resets cell to **1**.
    

But here’s the limitation:

---

# ❌ You _cannot_ individually reset a single cell back to 1

Flash programming mechanism only supports:

- **Programming:** 1 → 0
    
- **Erase:** resets an entire _block_ to all 1s
    

You **cannot** flip 0 → 1 on a single cell, page, or sector.

Example:

|Operation|Allowed?|Reason|
|---|---|---|
|1 → 0 (write)|✔️ Yes|Add charge|
|0 → 1 (write)|❌ No|Requires removing charge, only block erase can do that|
|Reset page to all 1s|❌ No|Only entire block can be erased|
|Erase block to all 1s|✔️ Yes|Electrical erase works on blocks only|

---

# 📦 Why It Affects Writing a Page

Imagine a page already has some data like:

`10110011`

If you want to write:

`01011001`

You can change any `1` in old data to `0` in new data.

But you _cannot_ change any `0` back to `1`.

So the following bits would be impossible to fix without erase:

`Old: 10110011 New: 01011001          ↑    ↑  (0 → 1 impossible)`

Thus the SSD must:

1. Read the “valid” pages into RAM
    
2. Erase the entire block → all pages become `11111111...`
    
3. Rewrite pages with updated values
    

---

# 🔥 This is why Flash Uses “Erase Blocks”

Flash is organized like this:

`Block  ├── Page 0  ├── Page 1  ├── Page 2  └── ...`

- You can **read** individual pages
    
- You can **program** (write) individual pages
    
- But you can **only erase** an entire block
    
- Erase resets **all pages to 1**
    

---

# 🧹 Why “Valid” Block = All Bits = 1

In flash terminology:

- **Erased block = all 1s = clean = writable**
    
- **Dirty block = contains 0s = must erase before writing again**
    

This is why the book says:

> “To write to a block, it must be in the valid (erased) state, meaning all 1s.”

Because otherwise you will need to set some bits to 1 (impossible).

---

# 🚚 What Happens If You Just Rewrite Without Erasing

The SSD would accumulate more and more 0s.

Example:

Initial erased page:

`11111111`

Write #1:

`10111111`

Write #2 (trying to write completely new data):

`00110111  ← impossible because some positions require 0→1`

It becomes garbage.

---

# 💡 That’s why SSDs need:

- **Garbage collection**
    
- **Wear-leveling**
    
- **Out-of-place writes (log-structured storage)**
    

Because you can’t update pages in place.

---

# 📝 Summary

Flash programming rules:

- **Writes = set 1 → 0 only**
    
- **Erase = sets every bit in a block back to 1**
    
- **Cannot flip 0 → 1 without erasing the whole block**
    
- **Therefore every rewrite requires block erase if page contains any 0s**
    

This is the core design challenge of flash memory.

---

If you want, I can also explain:

✔️ Why garbage collection exists  
✔️ Why SSDs use log-structured design  
✔️ Why this causes write amplification  
✔️ Why write endurance is limited  
✔️ Multi-level cell (MLC/TLC/QLC) programming mechanics  
✔️ How FTL (Flash Translation Layer) handles these problems