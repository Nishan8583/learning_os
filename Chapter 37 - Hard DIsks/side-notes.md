# 🔍 Seek Time vs Rotational Delay (Hard Drives)

## **1. Seek Time**

**What it is:**  
The time it takes for the **disk arm** to move the **head** to the correct **track**.

### 🛠 Think of it like:

Moving the needle on a record player to the correct circle.

### 🧩 Why it happens:

Data on a disk is stored in many **tracks** (concentric circles).  
If your data is on a different track, the head must **physically move**.

### ⏱ Typical seek time:

- **4–10 ms** on modern drives
    
- Seek time is the **slowest part** of HDD access.
    

---

## **2. Rotational Delay (Rotational Latency)**

**What it is:**  
Once the head is over the correct track, you must wait for the **correct sector** to rotate under the head.

### 🛠 Think of it like:

You placed the needle on the correct circle,  
but now you wait for the vinyl record to spin to the exact song start point.

### 🧩 Why it happens:

The disk is always spinning at a fixed RPM (e.g., 7200 RPM).  
You can’t read the sector until it rotates beneath the head.

### ⏱ Typical rotational delay:

- Average = **½ rotation**
    
- Example: 7200 RPM → 8.33 ms per rotation → **~4 ms rotational delay**
    

---

# 📌 Summary Table

|Concept|What moves?|Why does it happen?|Typical Time|
|---|---|---|---|
|**Seek Time**|Disk arm moves **sideways** to correct track|You requested data on a _different track_|4–10 ms|
|**Rotational Delay**|Disk platters **spin** to bring sector under head|Your sector is at a _different angular position_|~4 ms|
|**Transfer Time**|Actual data read/write|After seek + rotation are done|<0.5 ms|