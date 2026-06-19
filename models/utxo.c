// models/utxo.c
#include <stdio.h>
#include <string.h>
#include "utxo.h"

// Global in-memory storage array tracking the state of all UTXOs
static UTXO utxo_pool[MAX_UTXO_POOL];
static int utxo_pool_count = 0;

int utxo_create(const char *tx_id, unsigned int output_index, const char *owner, double amount) {
    if (utxo_pool_count >= MAX_UTXO_POOL) return 0; // Pool full
    
    // Check for duplicate entry to prevent data corruption
    for (int i = 0; i < utxo_pool_count; i++) {
        if (strcmp(utxo_pool[i].tx_id, tx_id) == 0 && utxo_pool[i].output_index == output_index) {
            return 0; 
        }
    }

    strncpy(utxo_pool[utxo_pool_count].tx_id, tx_id, HASH_LEN);
    utxo_pool[utxo_pool_count].output_index = output_index;
    strncpy(utxo_pool[utxo_pool_count].owner_address, owner, ADDR_LEN);
    utxo_pool[utxo_pool_count].amount = amount;
    utxo_pool[utxo_pool_count].is_spent = 0; // Explicitly marked UNSPENT on initialization

    utxo_pool_count++;
    return 1;
}

int utxo_validate_input(const char *tx_id, unsigned int output_index) {
    for (int i = 0; i < utxo_pool_count; i++) {
        if (strcmp(utxo_pool[i].tx_id, tx_id) == 0 && utxo_pool[i].output_index == output_index) {
            // Must exist and must NOT be spent
            if (utxo_pool[i].is_spent == 0) {
                return 1; // Valid UTXO target
            }
            return 0; // Already spent (Detected double-spending attempt)
        }
    }
    return 0; // UTXO does not exist
}

int utxo_consume(const char *tx_id, unsigned int output_index) {
    for (int i = 0; i < utxo_pool_count; i++) {
        if (strcmp(utxo_pool[i].tx_id, tx_id) == 0 && utxo_pool[i].output_index == output_index) {
            if (utxo_pool[i].is_spent == 1) {
                return 0; // Fail: double-spend block triggered!
            }
            utxo_pool[i].is_spent = 1; // Mark as spent
            return 1;
        }
    }
    return 0; // Target output target missing
}

double utxo_get_balance(const char *owner) {
    double balance = 0.0;
    for (int i = 0; i < utxo_pool_count; i++) {
        if (strcmp(utxo_pool[i].owner_address, owner) == 0 && utxo_pool[i].is_spent == 0) {
            balance += utxo_pool[i].amount;
        }
    }
    return balance;
}

void utxo_display_pool() {
    printf("\n====== ACTIVE UTXO SET (UNSPENT OUTPUTS) ======\n");
    int active_count = 0;
    for (int i = 0; i < utxo_pool_count; i++) {
        if (utxo_pool[i].is_spent == 0) {
            printf("Tx ID: %s... | Index: %u | Owner: %s... | Amount: %.2f AHT\n",
                   utxo_pool[i].tx_id,
                   utxo_pool[i].output_index,
                   utxo_pool[i].owner_address,
                   utxo_pool[i].amount);
            active_count++;
        }
    }
    if (active_count == 0) {
        printf("No current unspent transaction outputs found in engine.\n");
    }
    printf("================================================\n");
}
