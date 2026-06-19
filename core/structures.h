// core/structures.h
#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <time.h>

#define MAX_TX_IN_BLOCK 10
#define ADDR_LEN 65          // To store standard public keys/hex strings
#define SIG_LEN 128          // To accommodate ECDSA signature bytes
#define HASH_LEN 65         // 64 hex characters + null terminator for SHA-256

// Enums

typedef enum {
    TRANSACTION_POLICY_ENROLLMENT,
    TRANSACTION_PREMIUM_PAYMENT,
    TRANSACTION_REINSURANCE_CONTRIBUTION,
    TRANSACTION_SERVICE_REQUEST,
    TRANSACTION_PREAUTH_REQUEST,
    TRANSACTION_PREAUTH_APPROVE,
    TRANSACTION_CLAIM_SUBMISSION,
    TRANSACTION_CLAIM_APPROVAL,
    TRANSACTION_CLAIM_REJECTION,
    TRANSACTION_CLAIM_SETTLEMENT,
    TRANSACTION_TOKEN_TRANSFER
} TransactionType;

typedef enum {
    STATUS_PENDING,
    STATUS_CONFIRMED,
    STATUS_SUSPICIOUS
} MempoolStatus;

typedef enum {
    POLICY_ACTIVE,
    POLICY_EXPIRED,
    POLICY_RENEWED
} PolicyStatus;

// --- Core Blockchain Structures ---

// Note: Explicitly containing NO hash field as mandated by instructions
typedef struct {
    char transaction_id[HASH_LEN];
    char sender_address[ADDR_LEN];
    char receiver_address[ADDR_LEN];
    double amount;
    TransactionType transaction_type;
    time_t timestamp;
    unsigned int sender_nonce;
    unsigned char digital_signature[SIG_LEN];
} Transaction;

typedef struct {
    unsigned int block_id;
    time_t timestamp;
    unsigned int transaction_count;
    char previous_hash[HASH_LEN];
    char merkle_root[HASH_LEN];
    unsigned int nonce;
    char miner_id[ADDR_LEN];
    unsigned int difficulty;
    Transaction transactions[MAX_TX_IN_BLOCK]; // The actual block body payload
} Block;

typedef struct {
    char transaction_id[HASH_LEN];
    char sender[ADDR_LEN];
    char receiver[ADDR_LEN];
    double amount;
    TransactionType transaction_type;
    double fee;
    MempoolStatus status;
} MempoolEntry;

// State Management Structures

typedef struct {
    char owner_address[ADDR_LEN];
    double balance;
    unsigned int nonce; // Increments only when a tx from this account is mined
} Account;

typedef struct {
    char tx_id[HASH_LEN];
    unsigned int output_index;
    char owner_address[ADDR_LEN];
    double amount;
    int is_spent; // 0 for unspent (UTXO), 1 for spent
} UTXO;

typedef struct {
    unsigned int current_difficulty;
    double block_reward;
    unsigned int total_blocks;
    unsigned int last_retarget_block;
} ChainState;

// Health Insurance Business Structures

typedef struct {
    char member_id[ADDR_LEN];
    char policy_id[HASH_LEN];
    char coverage_plan[32];
    time_t enrollment_date;
    time_t expiry_date; // enrollment_date + 365 days
    PolicyStatus status;
} Policy;

#endif // STRUCTURES_H
