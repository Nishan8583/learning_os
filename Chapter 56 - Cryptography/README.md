# Protecting Information With Cryptography

Chapter by Peter Reiher (UCLA). This chapter discusses using cryptography to protect data outside the OS's direct control, focusing on secrecy, integrity, and authentication. It emphasizes proper use, limitations, and integration with operating systems.

## Introduction

- OS controls hardware but not external machines or unauthorized hardware access.
- Cryptography prepares data for potential loss or tampering by making it unreadable/alterable without keys.
- Key benefits: secrecy (opponents can't understand data), integrity (detect tampering), authentication.
- Using cryptography is computationally expensive; must be selective and implemented carefully.
- **Crux**: Protect data outside OS domain using cryptographic technologies, with proper usage and awareness of limitations.

## Cryptography Basics (Symmetric)

- Transforms plaintext (P) to ciphertext (C) using encryption algorithm E() and key K: C = E(P, K).
- Reverse with decryption D(): P = D(C, K).
- Example: Caesar cipher (simple shift, weak).
- Properties: Deterministic, hard to derive P from C without K (computationally infeasible).
- Relies on key secrecy; algorithms (e.g., AES) are public and studied.
- **Tip: Don't develop your own ciphers** – Use standards like AES; experts often fail, amateurs will.
- Additional uses: Integrity (tampering yields garbage or hash mismatch), authentication (shared key implies sender identity).
- Symmetric: Same key for encrypt/decrypt.

## Public Key Cryptography (Asymmetric)

- Uses two keys: Public (widely shared) and private (secret).
- Encrypt with one, decrypt with the other: C = E(P, K_encrypt), P = D(C, K_decrypt).
- Authentication: Encrypt with private key; anyone decrypts with public to verify origin.
- Secrecy: Encrypt with public key; only private key holder decrypts.
- For both: Double encryption (private for auth, then public for secrecy).
- Computationally expensive (100x symmetric); use selectively.
- Key distribution: Private never shared; public distributed but must be authenticated to prevent spoofing (e.g., via infrastructures).
- **Key secrecy critical**: Lose private key, lose all protection.
- History: Invented by Diffie et al.; avoids symmetric key distribution issues.

## Cryptographic Hashes

- One-way function: Hash plaintext to fixed-size value (S = H(P)); no decryption.
- Properties:
    - Collision-resistant: Hard to find two inputs with same hash.
    - Unpredictable changes: Any input change alters hash unpredictably.
    - Non-invertible: Can't infer input from hash.
- Uses: Integrity (encrypt hash, send with data; mismatch detects tampering), password storage, proof-of-work.
- Standards: SHA-3 (avoid broken ones like SHA-1).
- No key usually; encrypt hash for transmission if needed.

## Cracking Cryptography

- Rarely breaks algorithm; usually guesses/steals key or exploits poor implementation.
- Brute force: Long keys (e.g., AES 128-bit) make it infeasible (2^127 guesses avg.).
- Common flaws: Weak key generation (e.g., Netscape seeds from time/PID), shared keys in devices.
- Examples: WEP (key from traffic in ~1 min), Heartbleed (keys from OpenSSL memory).
- **Advice**: Use random keys (/dev/random), avoid deterministic selection for perfect forward secrecy.

## Cryptography and Operating Systems

- OS accesses all resources; can't hide data/keys from dishonest OS.
- Useful when: Data leaves OS control (network, stolen hardware).
- One-way hashes for passwords (can't decrypt).
- Limitations: If OS compromised during decrypt/use, protection fails.
- Minimize exposure: Decrypt infrequently, destroy plaintext/keys quickly.

## At-Rest Data Encryption

- Encrypt stored data (e.g., disks, flash) to protect if hardware accessed without OS.
- Full Disk Encryption (FDE): Encrypt entire device (e.g., BitLocker, FileVault).
    - Hardware/software-based; transparent post-boot (key/passphrase at start).
    - Overhead: ~few % latency in software.
- Protects against: Physical theft/resale (new OS can't decrypt without key).
- Doesn't protect: Dishonest users/admins, app/OS flaws, runtime access.
- Special cases: Archives/cloud backups (encrypt before sending), user-level (e.g., email).
- Password vaults: Encrypt passwords; key from user, stored in RAM temporarily.

## Cryptographic Capabilities

- Unforgeable capabilities: Encrypt access rights (e.g., resource, user, expiration).
- Symmetric: Shared key; good for single machine or trusted distributed.
- Asymmetric: Public/private; allows transferable rights without key sharing.
- Benefits: No ACLs needed; integrity ensured; tie to IP/time to prevent copying.
- Long patterns to prevent brute-force forging.

## Summary

- Cryptography provides secrecy, integrity, authentication via symmetric/asymmetric ciphers and hashes.
- Relies on key secrecy; use standards, random keys.
- OS uses: Network transit, at-rest data; limited against internal compromises.
- Capabilities enable secure, transferable access control.

## References

- Key sources: Diffie (PK history), Gutmann (crypto flaws), Schneier (applied crypto), etc. (See chapter for full list).