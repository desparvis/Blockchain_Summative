// core/cryptography.h
#ifndef CRYPTOGRAPHY_H
#define CRYPTOGRAPHY_H

#include "structures.h"

// Generate a new public/private keypair (Hex representation)
int crypto_generate_keypair(char *out_public_key, unsigned char *out_private_key);

// Compute standard SHA-256 hash of a string buffer and output a hex string
void crypto_sha256(const char *input, char *out_hex);

// Compute a transaction digest specifically for signing/verifying
void crypto_hash_transaction_fields(const Transaction *tx, char *out_hex);

// Sign a transaction using a private key
int crypto_sign_transaction(Transaction *tx, const unsigned char *private_key);

// Verify a transaction's signature against its sender address (public key)
int crypto_verify_signature(const Transaction *tx);

// Build a Merkle Tree from scratch out of an array of transactions and return the root
void crypto_build_merkle_root(const Transaction *tx_array, unsigned int count, char *out_root);

#endif // CRYPTOGRAPHY_H
