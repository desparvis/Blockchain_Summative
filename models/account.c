// models/account.c
#include <stdio.h>
#include <string.h>
#include "account.h"


// In-memory global state storage for balances and nonces
Account account_state_pool[MAX_ACCOUNTS];
int total_accounts_count = 0;

// Hardcoded addresses for our 5 mandatory internal tracking wallets
const char* WALLET_INSURANCE_POOL = "ALU_INSURANCE_POOL_WALLET_ADDRESS";
const char* WALLET_REINSURANCE_POOL = "ALU_REINSURANCE_POOL_WALLET_ADDRESS";
const char* WALLET_MINER_POOL = "ALU_MINER_REWARD_WALLET_ADDRESS";

// Helper to look up an account index in our state pool array
static int find_account_index(const char *address) {
    for (int i = 0; i < total_accounts_count; i++) {
        if (strcmp(account_state_pool[i].owner_address, address) == 0) {
            return i;
        }
    }
    return -1; // Account not found
}

void account_init_system_wallets() {
    // Clear pool
    total_accounts_count = 0;
    
    // Seed our mandatory system buffer wallets with foundational reserves
    account_register(WALLET_INSURANCE_POOL, 500000.00);    // Insurance seed
    account_register(WALLET_REINSURANCE_POOL, 200000.00);  // Reinsurance pool
    account_register(WALLET_MINER_POOL, 10000.00);         // Miner vault
}

int account_register(const char *address, double initial_balance) {
    if (total_accounts_count >= MAX_ACCOUNTS) return 0; // Pool capacity full
    if (find_account_index(address) != -1) return 0;    // Already exists
    
    strncpy(account_state_pool[total_accounts_count].owner_address, address, ADDR_LEN);
    account_state_pool[total_accounts_count].balance = initial_balance;
    account_state_pool[total_accounts_count].nonce = 0; // Starts explicitly at 0
    
    total_accounts_count++;
    return 1;
}

double account_get_balance(const char *address) {
    int idx = find_account_index(address);
    if (idx != -1) return account_state_pool[idx].balance;
    return 0.0;
}

unsigned int account_get_nonce(const char *address) {
    int idx = find_account_index(address);
    if (idx != -1) return account_state_pool[idx].nonce;
    return 0;
}

int account_validate_nonce(const char *address, unsigned int tx_nonce) {
    int idx = find_account_index(address);
    if (idx == -1) return 0; // Unregistered account
    
    // As per assignment prompt criteria: network validates that incoming tx_nonce == current_nonce + 1
    if (tx_nonce == account_state_pool[idx].nonce + 1) {
        return 1;
    }
    return 0; // Invalid nonce (prevents replay attacks)
}

int account_apply_transaction(const char *sender, const char *receiver, double amount, unsigned int tx_nonce) {
    int s_idx = find_account_index(sender);
    int r_idx = find_account_index(receiver);
    
    if (s_idx == -1 || r_idx == -1) return 0; // Target wallets must exist
    if (account_state_pool[s_idx].balance < amount) return 0; // Insufficient fund check
    if (!account_validate_nonce(sender, tx_nonce)) return 0; // Nonce sync failure
    
    // Mutate state balances directly
    account_state_pool[s_idx].balance -= amount;
    account_state_pool[r_idx].balance += amount;
    
    // CRITICAL: Account nonce ONLY increments when the transaction is successfully confirmed in a block
    account_state_pool[s_idx].nonce += 1;
    
    return 1;
}

void account_display_all() {
    printf("\n====== CURRENT ACCOUNT BALANCES & NONCES ======\n");
    for (int i = 0; i < total_accounts_count; i++) {
        printf("Address: %s... | Balance: %.2f AHT | Nonce: %u\n",
               account_state_pool[i].owner_address,
               account_state_pool[i].balance,
               account_state_pool[i].nonce);
    }
    printf("================================================\n");
}
