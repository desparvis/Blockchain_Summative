// core/persistence.c
#include <stdio.h>
#include <stdlib.h>
#include "persistence.h"
#include "blockchain.h"
#include "../models/account.h"
#include "../models/utxo.h"
#include "../insurance/policy.h"

// Reference external state counts and arrays using global mapping link points
// These link back to the live structures running inside your submodules
extern int chain_height;
extern int mempool_count;
extern int total_accounts_count;
extern int utxo_pool_count;
extern int total_policies;

// Define explicit mock serialization pointers to read static arrays safely
// In your unified build script, you can expose shallow getters for these addresses
extern Block* get_blockchain_array_ptr();
extern MempoolEntry* get_mempool_array_ptr();
extern Account* get_account_array_ptr();
extern UTXO* get_utxo_array_ptr();
extern Policy* get_policy_registry_ptr();

int persistence_save_state() {
    FILE *file = fopen(STORAGE_FILE, "wb");
    if (!file) {
        printf("Disk Error: Unable to open storage system backup bin file for writing.\n");
        return 0;
    }

    // 1. Write metadata markers and array counters first (Header Layout)
    fwrite(&chain_height, sizeof(int), 1, file);
    fwrite(&mempool_count, sizeof(int), 1, file);
    fwrite(&total_accounts_count, sizeof(int), 1, file);
    fwrite(&utxo_pool_count, sizeof(int), 1, file);
    fwrite(&total_policies, sizeof(int), 1, file);

    // 2. Stream actual binary records sequentially out to file layout structures
    if (chain_height > 0)
        fwrite(get_blockchain_array_ptr(), sizeof(Block), chain_height, file);
    if (mempool_count > 0)
        fwrite(get_mempool_array_ptr(), sizeof(MempoolEntry), mempool_count, file);
    if (total_accounts_count > 0)
        fwrite(get_account_array_ptr(), sizeof(Account), total_accounts_count, file);
    if (utxo_pool_count > 0)
        fwrite(get_utxo_array_ptr(), sizeof(UTXO), utxo_pool_count, file);
    if (total_policies > 0)
        fwrite(get_policy_registry_ptr(), sizeof(Policy), total_policies, file);

    fclose(file);
    printf("State Saved Successfully! State committed securely to '%s'.\n", STORAGE_FILE);
    return 1;
}

int persistence_load_state() {
    FILE *file = fopen(STORAGE_FILE, "rb");
    if (!file) {
        // Safe fall back layout if execution environment has no saved snapshot history yet
        printf("No existing ledger backup found on system disk. Generating clean state architecture...\n");
        blockchain_init();
        account_init_system_wallets();
        return 1;
    }

    // 1. Recover global configuration height indexes
    fread(&chain_height, sizeof(int), 1, file);
    fread(&mempool_count, sizeof(int), 1, file);
    fread(&total_accounts_count, sizeof(int), 1, file);
    fread(&utxo_pool_count, sizeof(int), 1, file);
    fread(&total_policies, sizeof(int), 1, file);

    // 2. Stream data records directly back into active memory block allocations
    if (chain_height > 0)
        fread(get_blockchain_array_ptr(), sizeof(Block), chain_height, file);
    if (mempool_count > 0)
        fread(get_mempool_array_ptr(), sizeof(MempoolEntry), mempool_count, file);
    if (total_accounts_count > 0)
        fread(get_account_array_ptr(), sizeof(Account), total_accounts_count, file);
    if (utxo_pool_count > 0)
        fread(get_utxo_array_ptr(), sizeof(UTXO), utxo_pool_count, file);
    if (total_policies > 0)
        fread(get_policy_registry_ptr(), sizeof(Policy), total_policies, file);

    fclose(file);
    printf("State binary stream pulled successfully from disk. Verifying ledger integrity...\n");

    // MANDATORY PROMPT CONSTRAINT CHECK: Re-validate the entire loaded chain immediately
    if (!blockchain_verify()) {
        printf("CRITICAL SECURITY VIOLATION: Loaded blockchain state failed cryptographic validation! System Locked.\n");
        exit(EXIT_FAILURE); 
    }

    return 1;
}
