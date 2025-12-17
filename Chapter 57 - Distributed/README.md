# Distributed System Security Notes

## Overview

- **Author**: Peter Reiher (UCLA)
- **Chapter Focus**: Explores security challenges in distributed systems where multiple machines cooperate over untrusted networks.
- **Key Crux**: How to secure systems spanning multiple machines using tools like cryptography, while being cautious in implementation.
- **Main Problems**:
    - Remote machines may not enforce desired security policies or could be adversaries.
    - Networks are untrusted; adversaries can forge, alter, replay, or delay messages.
- **Primary Tool**: Cryptography, but use standard, carefully designed tools instead of building custom ones.

## 57.1 Introduction

- Operating systems control local resources but face issues in distributed environments.
- Trust in remote credentials and secure network communication are essential.
- Emphasizes using pre-built cryptographic tools for reliability.

## 57.2 The Role of Authentication

- Uncertainty about remote partners enforcing policies; often rely on trust similar to real-world relationships.
- Detection of misbehavior allows adjustments in trust or compensating actions.
- Authentication over networks relies on "what you know" (passwords or private keys).
- Passwords suit many-to-one authentication (e.g., users to a site).
- Public keys suit one-to-many (e.g., site to users).
- Even secure networks don't solve issues with compromised remote sites.

## 57.3 Public Key Authentication for Distributed Systems

- Public keys don't need secrecy but must be verified as belonging to the partner.
- Certificates: Bundles signed by trusted third parties (Certificate Authorities - CAs) containing public key, owner info, and signature.
- Example Process:
    - CA verifies requester and signs hash of key/info (e.g., using SHA-3).
    - Recipient verifies signature using CA's public key.
- Bootstrapping: Initial trusted keys come from software (e.g., browsers pre-configured with hundreds of CA keys).
- Anyone can create certificates (e.g., using PGP), but trust in signer is key.
- Replay attacks prevented by unique message info in protocols.
- Public key crypto is expensive; switch to symmetric after authentication.

## 57.4 Password Authentication for Distributed Systems

- Best for two-party scenarios (e.g., user to site).
- Send ID and password encrypted (using site's public key from prior authentication).
- Protects against eavesdropping; protocols handle replay.
- Post-verification: Knows sender has password, but future messages need tying to initial auth.
- Password management issues (choice, storage) apply as in local auth.

## 57.5 SSL/TLS

- Secure sockets for encrypted data transfer; TLS is the secure version (use 1.3 as of 2020).
- Setup: Libraries for crypto options (ciphers, hashes, key exchange).
- Key Distribution: New symmetric key per connection via Diffie-Hellman (authenticated usually via certificates).
- Negotiation: Client/server agree on ciphers (e.g., RSA-2048, AES-256).
- Common: Server authenticates via certificate; client via password later.
- Aside on Diffie-Hellman:
    - Secure key exchange over insecure channels using large prime/mod operations.
    - Vulnerable to man-in-the-middle without authentication.
- Flaws: Protocol vs. implementation (e.g., Heartbleed in OpenSSL).

## 57.6 Other Authentication Approaches

- Web Cookies: Server stores authenticated identity in encrypted cookie for future sessions.
    - Tradeoff: Convenience vs. security (e.g., stolen cookies).
- Challenge/Response: Server sends challenge; client computes response using secret (hash/crypto).
    - Prevents replay; requires pre-arrangement.
- Authentication Servers (e.g., Kerberos): Trusted server vouches for identity online.
    - More responsive to changes than offline certificates.

## 57.7 Some Higher Level Tools

- **HTTPS**:
    - HTTP over SSL/TLS; secures web data.
    - Uses browser-managed certificates; HSTS enforces strict HTTPS.
    - Leverages existing tools for efficiency.
- **SSH**:
    - Secure shell for remote access (commands, X Windows, port forwarding).
    - Auth via certificates/passwords/Kerberos; negotiates ciphers.
    - Vulnerable to initial man-in-the-middle if no prior key (warning prompt).
    - Independent of SSL; different bugs/uses.

## 57.8 Summary

- Distributed security relies on authenticating/encrypting over insecure networks.
- Tools: X.509 certificates, passwords, symmetric crypto, Diffie-Hellman.
- Use standards (SSL/TLS, HTTPS, SSH) to avoid pitfalls.
- Limitations: No full control over remote actions; always risky beyond local machine.

## References

- [H14] The Heartbleed Bug (heartbleed.com)
- [I12] ITU-T X.509 Certificate Frameworks
- [NT94] Kerberos Paper (Neuman & Ts'o)
- [P16] PGP Home Page (pgpi.org)