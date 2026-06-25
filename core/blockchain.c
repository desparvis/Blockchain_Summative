// core/blockchain.c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "blockchain.h"
#include "cryptography.h"
#include "../models/account.h"
#include "../models/utxo.h"


// In-memory states
Block blockchain[MAX_CHAIN_SIZE];
MempoolEntry mempool[MAX_MEMPOOL_SIZE];
MempoolEntry fraud_review_pool[50];
int fraud_pool_count = 0;
int chain_height = 0;
int mempool_count = 0;
static ChainState global_state;

// Internal helper to calculate block header hash
static void calculate_block_hash(const Block *blk, char *out_hash) {
    char header_buffer[2048];
    // Formats fields exactly to build an aggregate header tracking block properties
    snprintf(header_buffer, sizeof(header_buffer), "%u%ld%s%s%u%s%u",
             blk->block_id, blk->timestamp, blk->previous_hash,
             blk->merkle_root, blk->nonce, blk->miner_id, blk->difficulty);
    crypto_sha256(header_buffer, out_hash);
}

void blockchain_init() {
    chain_height = 0;
    mempool_count = 0;
    
    global_state.current_difficulty = 2; // Default difficulty: starts with 2 hex zeros ("00")
    global_state.block_reward = 50.0;    // 50 AHT per block mined
    global_state.total_blocks = 0;
    global_state.last_retarget_block = 0;

    // Build Genesis Block (Block #0)
    Block genesis;
    genesis.block_id = 0;
    genesis.timestamp = time(NULL);
    genesis.transaction_count = 0;
    strcpy(genesis.previous_hash, "0000000000000000000000000000000000000000000000000000000000000000");
    strcpy(genesis.merkle_root, "0000000000000000000000000000000000000000000000000000000000000000");
    genesis.nonce = 100; // Arbitrary hardcoded initialization value
    strcpy(genesis.miner_id, "SYSTEM_GENESIS_VAL");
    genesis.difficulty = global_state.current_difficulty;

    blockchain[0] = genesis;
    chain_height = 1;
    global_state.total_blocks = 1;
}

int mempool_add_transaction(const Transaction *tx, double fee) {
    if (mempool_count >= MAX_MEMPOOL_SIZE) return 0; // Mempool full
    
    // Check digital signature authenticity
    if (!crypto_verify_signature(tx)) {
        printf("Transaction validation failed: Invalid cryptographic signature.\n");
        return 0;
    }

    // Capture entry data properties
    MempoolEntry entry;
    strncpy(entry.transaction_id, tx->transaction_id, HASH_LEN);
    strncpy(entry.sender, tx->sender_address, ADDR_LEN);
    strncpy(entry.receiver, tx->receiver_address, ADDR_LEN);
    entry.amount = tx->amount;
    entry.transaction_type = tx->transaction_type;
    entry.fee = fee;
    entry.status = STATUS_PENDING;

    mempool[mempool_count] = entry;
    mempool_count++;
    
    // Auto-sort priority sorting engine layout immediately on injection
    mempool_sort_priority();
    return 1;
}

void mempool_sort_priority() {
    // Basic Bubble Sort to guarantee deterministic priority routing layout
    for (int i = 0; i < mempool_count - 1; i++) {
        for (int j = 0; j < mempool_count - i - 1; j++) {
            int swap_needed = 0;
            // Condition 1: Fee descending order
            if (mempool[j].fee < mempool[j+1].fee) {
                swap_needed = 1;
            } 
            // Condition 2: Ties broken by oldest timestamp (omitted here for mock simplicity)
            
            if (swap_needed) {
                MempoolEntry temp = mempool[j];
                mempool[j] = mempool[j+1];
                mempool[j+1] = temp;
            }
        }
    }
}

int mine_solo(const char *miner_address) {
    if (mempool_count == 0) {
        printf("Mining rejected: Mempool is empty.\n");
        return 0;
    }

    Block next_block;
    next_block.block_id = chain_height;
    next_block.timestamp = time(NULL);
    
    // Ensure chronological timestamp rule requirement is met
    if (next_block.timestamp <= blockchain[chain_height - 1].timestamp) {
        next_block.timestamp = blockchain[chain_height - 1].timestamp + 1;
    }

    // Extract previous block hash linkage pointer
    char prev_hash[HASH_LEN];
    calculate_block_hash(&blockchain[chain_height - 1], prev_hash);
    strcpy(next_block.previous_hash, prev_hash);
    
    strncpy(next_block.miner_id, miner_address, ADDR_LEN);
    next_block.difficulty = global_state.current_difficulty;

    // Collect up to MAX_TX_IN_BLOCK transactions marked PENDING
    unsigned int tx_idx = 0;
    int mempool_indices[MAX_TX_IN_BLOCK];
    
    for (int i = 0; i < mempool_count && tx_idx < MAX_TX_IN_BLOCK; i++) {
        if (mempool[i].status == STATUS_PENDING) {
            // Reconstruct full Transaction objects from mempool properties
            Transaction core_tx;
            strncpy(core_tx.transaction_id, mempool[i].transaction_id, HASH_LEN);
            strncpy(core_tx.sender_address, mempool[i].sender, ADDR_LEN);
            strncpy(core_tx.receiver_address, mempool[i].receiver, ADDR_LEN);
            core_tx.amount = mempool[i].amount;
            core_tx.transaction_type = mempool[i].transaction_type;
            core_tx.timestamp = time(NULL);
            core_tx.sender_nonce = account_get_nonce(mempool[i].sender) + 1;
            memset(core_tx.digital_signature, 0x7F, SIG_LEN); // Mock fill matching validator expected states

            next_block.transactions[tx_idx] = core_tx;
            mempool_indices[tx_idx] = i;
            tx_idx++;
        }
    }

    next_block.transaction_count = tx_idx;

    // Build Merkle Root from custom core scratch logic
    crypto_build_merkle_root(next_block.transactions, next_block.transaction_count, next_block.merkle_root);

    // --- PROOF OF WORK MINING LOOP ---
    char current_hash[HASH_LEN];
    unsigned int target_nonce = 0;
    int mining_success = 0;

    // Build target check condition template array matching difficulty count requirements
    char prefix_target[10] = {0};
    for(unsigned int d = 0; d < global_state.current_difficulty; d++) {
        prefix_target[d] = '0';
    }

    printf("Mining Block #%u with difficulty %u... \n", next_block.block_id, global_state.current_difficulty);
    while (target_nonce < 5000000) { // Safety ceiling boundary limit
        next_block.nonce = target_nonce;
        calculate_block_hash(&next_block, current_hash);
        
        if (strncmp(current_hash, prefix_target, global_state.current_difficulty) == 0) {
            mining_success = 1;
            break;
        }
        target_nonce++;
    }

    if (!mining_success) {
        printf("Mining timed out / failed to find hash target match.\n");
        return 0;
    }

    printf("Block successfully mined! Hash: %s\n", current_hash);

    // Mutate and apply account states for all finalized transactions
    for (unsigned int k = 0; k < next_block.transaction_count; k++) {
        Transaction *applied_tx = &next_block.transactions[k];
        account_apply_transaction(applied_tx->sender_address, applied_tx->receiver_address, applied_tx->amount, applied_tx->sender_nonce);
        utxo_create(applied_tx->transaction_id, k, applied_tx->receiver_address, applied_tx->amount);
    }

    // Issue Mining block processing reward to engine miner node address
    account_register(miner_address, account_get_balance(miner_address) + global_state.block_reward);

    // Commit mined block onto ledger array height index track position
    blockchain[chain_height] = next_block;
    chain_height++;
    global_state.total_blocks++;

    // Purge spent records from in-memory pool array track layout
    int removed = 0;
    for(unsigned int r = 0; r < next_block.transaction_count; r++) {
        int target_mempool_idx = mempool_indices[r] - removed;
        for(int m = target_mempool_idx; m < mempool_count - 1; m++) {
            mempool[m] = mempool[m+1];
        }
        mempool_count--;
        removed++;
    }

    return 1;
}

// Pool mining added

int mine_pool(unsigned int miner_count, char miners[][65], double *shares) {
    if (mempool_count == 0) {
        printf("Mining rejected: Mempool empty.\n");
        return 0;
    }

    // Run the same block setup as solo mining...
    Block next_block;
    next_block.block_id = chain_height;
    next_block.timestamp = time(NULL);
    strcpy(next_block.previous_hash, "MOCK_PREV_HASH");
    next_block.difficulty = global_state.current_difficulty;

    // Pull transactions up to MAX_TX_IN_BLOCK
    unsigned int tx_idx = 0;
    for (int i = 0; i < mempool_count && tx_idx < MAX_TX_IN_BLOCK; i++) {
        Transaction core_tx;
        strncpy(core_tx.transaction_id, mempool[i].transaction_id, HASH_LEN);
        strncpy(core_tx.sender_address, mempool[i].sender, ADDR_LEN);
        strncpy(core_tx.receiver_address, mempool[i].receiver, ADDR_LEN);
        core_tx.amount = mempool[i].amount;
        core_tx.transaction_type = mempool[i].transaction_type;
        next_block.transactions[tx_idx++] = core_tx;
    }
    next_block.transaction_count = tx_idx;
    strcpy(next_block.merkle_root, "MOCK_ROOT");
    next_block.nonce = 42; // Found nonce simulation

    // DISTRIBUTE REWARDS PROPORTIONALLY
    printf("\n--- DISTRIBUTING POOL MINING REWARDS (Total: %.2f AHT) ---\n", global_state.block_reward);
    for (unsigned int i = 0; i < miner_count; i++) {
        double payout = global_state.block_reward * shares[i];
        account_register(miners[i], account_get_balance(miners[i]) + payout);
        printf("Miner [%s] received proportional share (%.1f%%): %.2f AHT\n", miners[i], shares[i]*100, payout);
    }

    blockchain[chain_height++] = next_block;
    mempool_count = 0; // Clear processed transactions
    return 1;
}

// Tampering injector
void tamper_force_corrupt(unsigned int block_id) {
    if (block_id >= (unsigned int)chain_height) {
        printf("Error: Block ID out of range.\n");
        return;
    }
    // Explicitly corrupt transaction values to trigger validation alerts
    blockchain[block_id].transactions[0].amount = 999999.0;
    printf("CRITICAL: Block #%u data has been secretly modified in memory!\n", block_id);
}

// Fraud Handling Engine
void view_fraud_flags() {
    printf("\n====== FRAUD REVIEW WORKFLOW QUEUE ======\n");
    for (int i = 0; i < fraud_pool_count; i++) {
        printf("[SUSPICIOUS] TxID: %s | Sender: %s | Amount: %.2f AHT\n",
               fraud_review_pool[i].transaction_id, fraud_review_pool[i].sender, fraud_review_pool[i].amount);
    }
    if (fraud_pool_count == 0) printf("Clean system: 0 fraud flags flagged.\n");
    printf("==========================================\n");
}

int blockchain_verify() {
    char expected_prev_hash[HASH_LEN];
    strcpy(expected_prev_hash, "0000000000000000000000000000000000000000000000000000000000000000");

    for (int i = 0; i < chain_height; i++) {
        Block *current_blk = &blockchain[i];

        // 1. Linkage verification Check
        if (i > 0) {
            calculate_block_hash(&blockchain[i-1], expected_prev_hash);
            if (strcmp(current_blk->previous_hash, expected_prev_hash) != 0) {
                printf("Audit Failure: Linkage split mismatch at Block #%d\n", i);
                return 0;
            }
            
            // 2. Timestamp order check
            if (current_blk->timestamp <= blockchain[i-1].timestamp) {
                printf("Audit Failure: Monotonic time anomaly detected at Block #%d\n", i);
                return 0;
            }
        }

        // 3. Merkle Root validation Check
        char computed_merkle[HASH_LEN];
        crypto_build_merkle_root(current_blk->transactions, current_blk->transaction_count, computed_merkle);
        if (strcmp(current_blk->merkle_root, computed_merkle) != 0) {
            printf("Audit Failure: Merkle Root corruption found at Block #%d\n", i);
            return 0;
        }
    }
    printf("Blockchain Validation Success: State matches ledger logic rules.\n");
    return 1;
}

void mempool_display() {
    printf("\n====== CURRENT PENDING MEMPOOL ======\n");
    for(int i=0; i<mempool_count; i++) {
        printf("[%d] TxID: %s... | Sender: %s... | Amount: %.2f AHT | Fee: %.2f\n",
               i, mempool[i].transaction_id, mempool[i].sender, mempool[i].amount, mempool[i].fee);
    }
    if(mempool_count == 0) printf("No items inside processing memory queues.\n");
    printf("======================================\n");
}
