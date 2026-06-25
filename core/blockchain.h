// core/blockchain.h
#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include "structures.h"

#define MAX_MEMPOOL_SIZE 200
#define MAX_CHAIN_SIZE 1000

// Initialize the blockchain state and genesis block
void blockchain_init();

// Add a transaction to the mempool (performs signature and balance checks first)
int mempool_add_transaction(const Transaction *tx, double fee);

// Sort the mempool by priority: Fee (descending), then Timestamp (ascending)
void mempool_sort_priority();

// Execute solo mining: picks top transactions, hashes block header, and resolves difficulty
int mine_solo(const char *miner_address);

// Print out current mempool status
void mempool_display();

// Print out the complete structured blockchain ledger
void blockchain_display();

// Comprehensive validation and tamper detection audit of the entire chain
int blockchain_verify();

// Pool mining
int mine_pool(unsigned int miner_count, char miners[][65], double *shares);


// Tampering
void tamper_force_corrupt(unsigned int block_id);

// Fraud logs
void view_fraud_flags();

// Resolve
int admin_resolve_fraud(const char *tx_id, int approve);

#endif // BLOCKCHAIN_H
