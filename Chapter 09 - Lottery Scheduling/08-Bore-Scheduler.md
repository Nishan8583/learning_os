# BORE Scheduler (CachyOS) — Deep Dive

A complete, technical explanation of the **BORE (Burst-Oriented Response Enhancer)** scheduler used in CachyOS, written for developers who already understand the **Linux Completely Fair Scheduler (CFS)**.

This README explains:

- how BORE modifies CFS  
- why it improves desktop/gaming performance  
- how vruntime decay works  
- patch-level differences  
- diagrams and comparisons  

---

# Table of Contents

1. [Overview](#overview)  
2. [Recap: CFS Basics](#recap-cfs-basics)  
3. [What BORE Changes](#what-bore-changes)  
4. [BORE vs CFS — Side-By-Side](#bore-vs-cfs)  
5. [Annotated Patch Snippets](#annotated-patch-snippets)  
6. [Diagrams](#diagrams)  
7. [Performance Notes](#performance-notes)  
8. [Summary](#summary)

---

# Overview

**BORE = Burst-Oriented Response Enhancer**  
A lightweight patch built **on top of the Linux CFS scheduler**.

Purpose:

- Lower latency  
- Boost responsiveness  
- Improve desktop & gaming performance  
- Produce smoother frametimes  
- Make GUI interactions more consistent  

BORE keeps **CFS’s structure**:

- red-black tree  
- vruntime accounting  
- fairness logic  

But changes the **priority calculation** using **vruntime decay**.

---

# Recap: CFS Basics

CFS attempts to give each runnable task an equal share of CPU over time using a concept called **virtual runtime (vruntime)**.

### How CFS works

- Tasks with **lower vruntime** run first  
- Tasks that run more increase vruntime faster  
- Tasks that sleep have lower vruntime and get priority when waking  
- The next task is the **leftmost node** in the RB-tree (min vruntime)

### CFS vruntime update

```c
vruntime += delta_exec * (1024 / weight[nice]);
```

### Desktop Problem with CFS
- Prioritizes _fairness_, not _latency_
- GUI actions sometimes delayed
- Under heavy load: stutters, micro-lag, input latency

This is where BORE steps in.

# What BORE Changes

## 1. **Vruntime Decay (Core Concept)**

BORE applies a **decay factor** to vruntime whenever a task wakes or yields:
```
vruntime = vruntime * DECAY
```

Typical decay: `0.5` or `0.75`.

### Effect:

- interactive tasks → boosted
- background CPU hogs → deprioritized
- newly awakened tasks → run _immediately_
- overall latency → dramatically lower

## 2. Simplified Heuristics

CFS uses many tuning knobs:
- wakeup_granularity
- target_latency
- weight calculations
- vruntime jitter smoothing
BORE **removes or simplifies** most of these → lower overhead, more predictable results.

---

## 3. Increased Bias for Bursty Tasks

Examples:
- opening applications
- GUI drawing
- handling mouse or keyboard events
- game render loops
These get **significant vruntime advantage**, keeping the system snappy even at 100% CPU usage.

# BORE vs CFS

## Task Wake-Up Behavior

| Behavior               | CFS                                                      | BORE                                |
| ---------------------- | -------------------------------------------------------- | ----------------------------------- |
| Task wakes after sleep | Given low vruntime, but constrained by granularity rules | vruntime forcibly reduced via decay |
| Priority               | “Fair”                                                   | Highly boosted                      |
| Result                 | May not run immediately                                  | Runs _immediately_                  |

## Long-Running Tasks

| Behavior       | CFS                      | BORE                         |
| -------------- | ------------------------ | ---------------------------- |
| Long CPU tasks | Slowly increase vruntime | Quickly become deprioritized |
| Effect         | More fairness            | More responsiveness          |
| Result         | Balanced workloads       | Better for gaming/GUI        |

## Gaming

|Issue|CFS|BORE|
|---|---|---|
|Frame delivery|Small delays possible|Much smoother|
|Input latency|Can spike|Very low|
|Background noise|Can interfere|Is deprioritized|
# Annotated Patch Snippets

Below is a simplified conceptual representation of BORE changes.

## CFS vruntime update
```c
// CFS logic
p->vruntime += delta_exec * weight_factor;
```

BORE vruntime update
```c
// Reduce vruntime to boost interactivity
p->vruntime = p->vruntime * DECAY;
p->vruntime += delta_exec;
```

CFS wakeup path

```c
place_entity(cfs_rq, se, 0);
```

BORE wakeup path
```c
// BORE: Prefer newly awakened tasks heavily
se->vruntime = cfs_rq->min_vruntime;
se->vruntime *= DECAY;
```

**Interpretation:**

- Newly awakened tasks’ vruntime aligns with the leftmost node
- Then gets multiplied by DECAY → even lower
- Becomes the next task to run
# Diagrams

## CFS vruntime distribution

```
|-----|-----|-----|-----|
0    10    20    30    40
```
Wake-up task lands near:
```
≈ 15
```

BORE vruntime distribution
```
|--|--|--|--|--|--|--|--|
0  5  10 15 20 25 30 35
```

After decay:
```
≈ 2
```

**It is guaranteed to run next.**

---

## Vruntime Decay Example

Before decay:
# Performance Notes

## Desktop Experience

- Faster app launch
- Smooth UI animations
- No input lag
- Mouse movement unaffected by load
- Background tabs do not lag core UI
    

---

## Gaming Benefits

- Stable frametimes
- Less microstutter
- Lower input latency
- Better responsiveness during load spikes
    

---

## Tradeoffs

| Benefit        | Cost                          |
| -------------- | ----------------------------- |
| Lower latency  | Slightly less fairness        |
| Smooth gaming  | Not ideal for servers         |
| Responsiveness | Heavy background tasks slowed |

# Summary

- BORE is a **modified CFS scheduler**, not a new scheduler.
- Adds **vruntime decay** → huge boost to interactive tasks.
- Simplifies many CFS heuristics.
- Improves responsiveness, gaming performance, and GUI behavior.
- Best suited for desktops and gaming environments.

---
# 🎮 **Scenario**

You are playing a game.  
Meanwhile, your system is running:
- **Long CPU-bound process A** (e.g., `cargo build --release`)
- **Long CPU-bound process B** (e.g., video encoding)
- **Your game’s render thread** (short bursts of CPU every frame)
- **Your mouse/keyboard input thread** (tiny bursts, very latency-sensitive)

Your CPU is **100% loaded.**
Under pure CFS, your game may micro-stutter or experience input lag.
Under BORE, it stays smooth.
Let’s see why.

---

# 🧠 **1. Understanding how vruntime normally behaves**

In CFS:

- A task that **runs a lot** gets **high vruntime**
- A task that sleeps (like game input thread) gets **low vruntime**
- The scheduler always picks the task with the **lowest vruntime** to run next

But **all tasks increase vruntime based on CPU time consumed.**
### Problem:

Long-running processes **consume more CPU time → vruntime increases → but they stay near the front of the RB-tree often because they keep running.**
Meanwhile interactive tasks wake up _many times per frame_ and need to run ASAP.
CFS tries to help them, but _not aggressively enough_.

---

# 🧩 **2. Now see what happens when we introduce BORE’s decay**

### 🔥 BORE applies:
```c
vruntime = vruntime * DECAY   (e.g., DECAY = 0.5 or 0.75)
```

This applies when:
- a task wakes up
- a task yields
- a task finishes a small burst
Meaning:

➡ **Long-running tasks get their vruntime multiplied downward**  
➡ **Interactive tasks get a HUGE relative boost**

Let’s compare numbers.

---

# 📊 **3. Concrete Example With Numbers**

### Initial vruntimes:
```c
A (compiler): vruntime = 200
B (encoder):  vruntime = 250
Game thread:  vruntime = 190
Input thread: vruntime = 120
```
Now CPU time passes…
- A and B keep running → vruntime increases FAST
- Game thread runs short bursts → increases slightly
- Input thread sleeps often → increases almost nothing

Let's say after frame #1:
```c
A: 350
B: 400
Game: 210
Input: 130
```

In CFS, this game thread may **still get scheduled slower**, because A/B are still considered “fair”.

---

# 💥 **4. Apply BORE decay**

When the game thread wakes up each frame:
```c
Game.vruntime = Game.vruntime * 0.5
```
So:
```c
Game: from 210 → 105
Input: from 130 → 65   (even lower)
A: stays ~350
B: stays ~400
```

## ⬅ Result: Game and Input “teleport” to the front of the RB-tree

Meaning:  
**They preempt immediately**  
Even if A and B were hogging CPU.

---

# 🎮 **5. What does this mean for gaming?**

### A task with vruntime = 65

will ALWAYS be picked before tasks with vruntime 350, 400, etc.

So even if the CPU is fully saturated:
- The **game render thread** gets immediate CPU access
- The **input thread** is processed instantly
- System feels 100% responsive
- No micro-stutters
- No input lag
- Mouse movement stays smooth even at 100% CPU

Because every frame, the game and input threads get their vruntime **decayed**, keeping them _permanently_ ahead of long CPU tasks.

---

# 🧠 **6. The Core Logic Simplified**

### **Long-running tasks:**
```c
vruntime grows + grows + grows
↓
Priority drops over time
```

Interactive tasks:
```c
small burst → vruntime increases slightly
↓
decay immediately pulls it back down
↓
always stays the lowest
↓
scheduler runs it instantly
```

# ✅ **Key Idea**

> **Decay multiplies vruntime by a number < 1 (e.g., 0.5 or 0.75).  
> So vruntime SHRINKS every time it’s applied.**

This shrinkage affects EVERY task, but:

- **Interactive tasks → shrink A LOT** because they have _low vruntime_
- **Long-running tasks → shrink LESS** because they have _very high vruntime_

This difference is the entire magic of BORE.

Games (and interactive apps in general) behave as **short-burst tasks**, and this naturally makes them have **lower vruntime** than long-running background processes.

Let’s break this down very cleanly so you truly _see_ why.

---

# 🎮 **1. Why games are “short-burst” tasks**

A typical game loop looks like this:
```c
while (running):
    process input      (tiny)
    update game state  (tiny)
    render frame       (short burst)
    wait for next frame (sleep)

```
Games constantly:

- **wake up for very short CPU bursts**
- **then sleep until the next frame** (typically 16.6 ms for 60 FPS)
This creates a pattern:
```c
Run a little → Sleep a lot → Run a little → Sleep a lot
```
# 🧠 **2. How vruntime behaves for short vs long tasks**

### ✔ Long-running CPU tasks

(e.g., compiling, encoding, background indexing)
```c
run continuously
→ accumulate large vruntime
```
If they run for 100 ms straight, vruntime may go from:
```
100 → 200 → 300 → 400 → ...
```
✔ Short-burst tasks (games, mouse, keyboard, GUI)
```
run tiny amount
→ accumulate tiny vruntime
→ then sleep
→ scheduler boosts them
```


So their vruntime moves like:

```
20 → 25 → sleep → (wake-up boost) → 12 → 14 → sleep → 8 → ...
```
Their vruntime stays **very small** forever.

---

# 🔥 **3. BORE makes the difference even more extreme**

### BORE applies:
```c
vruntime = vruntime * DECAY   (e.g., 0.5)
```
When the game wakes up every frame:
```
vruntime → becomes even smaller
```
Example:

| Task    | Before | After decay (0.5) |
| ------- | ------ | ----------------- |
| Game    | 40     | 20                |
| CPU hog | 400    | 200               |
So even if both shrink:

→ Game stays **10× smaller**  
→ And gets scheduled **immediately**

Every.

Single.

Frame.

---
# 🔁 **4. Over time, the gap becomes massive**

Game (interactive):
```c
40 → 20 → 10 → 5 → 2.5 → 1.2
```
CPU hog:
```c
400 → 200 → 100 → 50 → 25 → 12.5
```

Game’s vruntime will **always** be dramatically smaller.

Therefore:

> **Yes — games will almost always have lower vruntime than other long processes.  
> And BORE makes this effect even stronger.**

---

# 🎯 **5. Why this ensures smooth gaming**

With lower vruntime:

- game’s render thread runs immediately
    
- input thread gets instant CPU
    
- background processes cannot interrupt frame timing
    
- microstutters disappear
    
- mouse latency stays low
    

Because the game behaves like:

> “I run for tiny bursts, then I sleep, then I get boosted, then I run again.”

And CPU hogs behave like:

> “I run forever, so my vruntime explodes.”


# Math Example
# 🎯 **Step 1 — Simplify the math**

Let’s pick:
```
DECAY = 0.5
```
BORE will do:
```
vruntime = vruntime * 0.5
```
Doesn’t matter if a number is big or small — everything shrinks.

But the _effect_ is different depending on whether the value is small or huge.

---
# 🎮 **Step 2 — Compare two processes**

| Task    | Type         | vruntime before decay |
| ------- | ------------ | --------------------- |
| Game    | short bursts | 40                    |
| CPU hog | long running | 400                   |
Apply decay:
```
Game: 40  → 20
Hog:  400 → 200
```
Both shrank by 50%.
BUT…
### ❗ After shrinkage:
```
Game: 20
Hog:  200
```
**Game is now 10× lower.**

In scheduling terms:
- lower vruntime = higher priority
- so game → runs first
- hog → runs less
    

This is the key.