# Authentication — Summary Notes

## 1. Why Authentication Matters

- OS security decisions depend heavily on **who** is requesting an action.
    
- The entity requesting an action is a **principal**; the active process acting for the principal is its **agent**.
    
- To enforce security policies, the OS must:
    
    - Correctly identify the principal behind each process.
        
    - Ensure identities cannot be forged or improperly changed.
        
    - Maintain **credentials** of processes (e.g., page-table permissions).
        

---

## 2. Attaching Identities to Processes

- Processes inherit identity from their parent (e.g., user shell, window manager).
    
- The first user process is created by the OS at login and assigned the authenticated user's identity.
    
- Correct process identity is crucial—misidentification can violate security goals.
    
- OS must authenticate users accurately since all descendant processes rely on that initial decision.
    

---

## 3. How Users Are Authenticated

Three classical methods:

### (1) **What You Know**

- **Passwords**: the oldest and most common method.
    
- Requirements:
    
    - Hard to guess (long length, varied characters).
        
    - Prevent disclosure (don’t write them down, don’t store in plaintext).
        
- OS stores **hashed** passwords, not plaintext:
    
    - Uses _cryptographic hash functions_ (e.g., SHA-3).
        
    - To prevent precomputed attacks: use **salted hashes** (unique random value per password).
        
- Vulnerabilities:
    
    - Dictionary attacks.
        
    - Users choosing weak passwords.
        
    - Stolen password files → offline cracking.
        
- Mitigations:
    
    - Rate-limit login attempts or temporarily lock accounts.
        
    - Use password vaults.
        

---

### (2) **What You Have**

- Physical items: smart cards, USB tokens, security dongles, smartphone codes.
    
- Usually involve time-varying codes (OTP).
    
- Weaknesses:
    
    - Device theft or loss.
        
    - SMS-based codes can theoretically be intercepted (though rare).
        
- Still practical and effective in real systems.
    

---

### (3) **What You Are** (Biometrics)

- Physical or behavioral characteristics: fingerprints, face, voice, typing patterns.
    
- Challenges:
    
    - Need specialized hardware.
        
    - Variability in readings → imperfect matches.
        
    - Vulnerable to spoofing (e.g., fake fingerprints, photos).
        
- Biometrics have:
    
    - **False Positive Rate (FPR)** — authenticating an imposter.
        
    - **False Negative Rate (FNR)** — rejecting the real user.
        
    - Tradeoff managed through **sensitivity**.
        
    - **Crossover Error Rate (CER)**: point where FPR = FNR; used to compare systems.
        
- Remote biometric authentication is risky (easy to spoof bit patterns over a network).
    

---

## 4. Multi-Factor Authentication (MFA)

- Uses two or more of:
    
    1. What you know (e.g., password)
        
    2. What you have (e.g., phone OTP)
        
    3. What you are (e.g., fingerprint)
        
- Greatly increases security by compensating for weaknesses in any single method.
    

---

## 5. Authenticating Non-Human Entities

- Many processes have no human users (e.g., web servers, IoT devices).
    
- OS may assign identities to service accounts (e.g., `webserver`).
    
- Methods:
    
    - Give service accounts passwords.
        
    - Allow privileged users or the OS to spawn processes as these identities.
        
    - Tools like **sudo -u** allow user-switching with special privileges.
        
- Groups:
    
    - Users can belong to privilege groups.
        
    - Processes inherit group capabilities.
        
    - Group membership provides flexible permission management.
        

---

## 6. Login Example: Linux

Typical login steps:

1. Prompt for username.
    
2. Prompt for password (not echoed).
    
3. System looks up user data (UID, GID, shell, home directory, salt, hashed password).
    
4. Hash(user_input + salt) and compare.
    
5. If correct → spawn user shell with correct identity.
    

Security note:

- Login failure does not specify whether username or password was wrong to avoid information leakage.
    

---

## 7. Summary of Key Lessons

- Proper user and process identification is foundational to OS security.
    
- Authentication must be:
    
    - Hard to forge.
        
    - Resistant to theft.
        
    - Difficult to guess.
        
- Passwords alone are no longer considered sufficient.
    
- Combine authentication factors and securely store credentials (hashed + salted).
    
- Biometric and token-based methods enhance security but introduce their own limitations.