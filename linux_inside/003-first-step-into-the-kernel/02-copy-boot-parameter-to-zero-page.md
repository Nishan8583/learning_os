# Copying boot parameters into the "zeropage"

At this stage:

👉 The **bootloader already loaded the kernel**  
👉 It passed some important info (memory map, cmdline, etc.)

Now the kernel does:

> “Let me copy all bootloader-provided data into my own safe structure”

That structure is called:

`boot_params`

And it lives in a special memory area called:

👉 **“zeropage”**

---

# 🧠 What is the "zeropage"?

👉 Just a **reserved memory area (first page)** used during boot

- Historically at low memory (first 4KB)
- Stores boot info
- Shared between bootloader and kernel

Think:

> “Temporary mailbox between bootloader → kernel”

---

# 🔥 copy_boot_params() — What does it do?

It does **2 things only**:

---

## ✅ 1. Copy kernel header (`hdr`)

boot_params.hdr = hdr;

- `hdr` comes from `header.S` (assembly)
- Contains:
    - Boot protocol info
    - Flags
    - Memory info
    - Entry points

👉 So kernel makes its own copy

---

## ✅ 2. Fix command line pointer

If bootloader used an **old format**, kernel adjusts pointer.

👉 Compatibility logic (not super important for understanding core flow)

---

# 🔧 Now the IMPORTANT part: memcpy in Assembly

You saw this:

```asm
GLOBAL(memcpy)  
    pushw   %si  
    pushw   %di  
    movw    %ax, %di  
    movw    %dx, %si  
    pushw   %cx  
    shrw    $2, %cx  
    rep; movsl  
    popw    %cx  
    andw    $3, %cx  
    rep; movsb  
    popw    %di  
    popw    %si  
    retl  
ENDPROC(memcpy)

```
Let’s decode it like a reverse engineer 🔥

---

# 🧠 Calling Convention (VERY IMPORTANT)

Because of:

`-mregparm=3

👉 First 3 arguments go into registers:

| Argument | Register |
| -------- | -------- |
| dest     | ax       |
| src      | dx       |
| size     | cx       |

So:

`memcpy(&boot_params.hdr, &hdr, sizeof hdr);`

becomes:

|Register|Value|
|---|---|
|ax|destination|
|dx|source|
|cx|size|

---

# 🔍 Step-by-Step Assembly Breakdown

---

## 🔹 1. Save registers
```
pushw %si  
pushw %di
```
👉 Save because we will overwrite them

---

## 🔹 2. Setup source & destination
```
movw %ax, %di   ; dest → DI  
movw %dx, %si   ; src → SI
```
👉 Now:

- `SI` → source
- `DI` → destination

---

## 🔹 3. Save size

`pushw %cx`

We’ll modify `cx`, so save original

---

## 🔹 4. Copy in 4-byte chunks (FAST PATH)
```
shrw $2, %cx     ; cx = cx / 4  
rep; movsl       ; copy 4 bytes repeatedly

```
👉 This is the optimization:

- Divide size by 4
- Copy **4 bytes at a time**

💡 `movsl` = move 4 bytes

---

## 🔹 5. Restore original size

`popw %cx`

---

## 🔹 6. Handle leftover bytes

```
andw $3, %cx     ; cx = size % 4  
rep; movsb       ; copy remaining bytes

```
👉 If size not divisible by 4:

- Copy remaining 1–3 bytes

---

## 🔹 7. Restore registers
```

popw %di  
popw %si

```
---

## 🔹 8. Return

retl

---

# 🧠 Visual Summary

### What memcpy does:
```
[ Source Memory ]  ----->  [ Destination Memory ]  
```
  
Step 1: Copy big chunks (4 bytes)  
Step 2: Copy leftovers (1–3 bytes)
