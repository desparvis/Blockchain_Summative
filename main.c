// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "core/structures.h"
#include "core/blockchain.h"
#include "core/cryptography.h"
#include "core/persistence.h"
#include "models/account.h"
#include "models/utxo.h"
#include "insurance/policy.h"
#include "insurance/claims.h"

// Exposing backend array tracking targets to fulfill persistence layer mapping
extern Block blockchain[1000];
extern MempoolEntry mempool[200];
extern Account account_state_pool[100];
extern UTXO utxo_pool[500];
extern Policy policy_registry[200];

Block* get_blockchain_array_ptr() { return blockchain; }
MempoolEntry* get_mempool_array_ptr() { return mempool; }
Account* get_account_array_ptr() { return account_state_pool; }
UTXO* get_utxo_array_ptr() { return utxo_pool; }
Policy* get_policy_registry_ptr() { return policy_registry; }

// FIXED: Changed to extern to eliminate "multiple definition" linker clashes
extern int chain_height;
extern int mempool_count;
extern int total_accounts_count;
extern int utxo_pool_count;
extern int total_policies;

// Prototype bindings for our new validation and review functions
int mine_pool(unsigned int miner_count, char miners[][65], double *shares);
void tamper_force_corrupt(unsigned int block_id);
void view_fraud_flags();

void print_help() {
    printf("\n--- ALU HEALTH INSURANCE BLOCKCHAIN CLI INTERFACE ---\n");
    printf("  register_member                 - Register a brand new wallet identity\n");
    printf("  enroll_policy                   - Setup a 365-day insurance coverage profile\n");
    printf("  pay_premium                     - Deposit fees; auto-spawns 5%% reinsurance share\n");
    printf("  settle_claim                    - Process claim payouts (auto splits values > 1000)\n");
    printf("  mempool_view                    - Audit current transaction waiting list queue\n");
    printf("  mine_solo                       - Run PoW hashing block extraction process\n");
    printf("  mine_pool                       - Run pool mining with proportional reward splits\n");
    printf("  blockchain_view                 - Output entire structured chain ledger data\n");
    printf("  blockchain_verify               - Run deep cryptographic network security health audit\n");
    printf("  tamper_chain                    - Manually corrupt a block to test audit detection\n");
    printf("  fraud_view                      - Review restricted admin suspicion queue flags\n");
    printf("  chain_save                      - Serialize active program state safely to disk\n");
    printf("  help                            - Show this manual menu dashboard guide\n");
    printf("  exit                            - Save data, quit runtime container shell\n");
    printf("----------------------------------------------------\n");
}

int main() {
    printf("Initializing ALU Health Token (AHT) Cryptographic Network Nodes...\n");
    
    // MANDATORY REQUIREMENT: Rehydrate application state history from local binary system database
    persistence_load_state();
    
    char command[64];
    print_help();

    while (1) {
        printf("\nALU-Blockchain-Shell> ");
        if (scanf("%63s", command) <= 0) break;

        if (strcmp(command, "exit") == 0) {
            printf("Shutting down node subsystem safely...\n");
            persistence_save_state();
            break;
        }
        else if (strcmp(command, "help") == 0) {
            print_help();
        }
        else if (strcmp(command, "register_member") == 0) {
            char addr[ADDR_LEN];
            double bal;
            printf("Enter unique Member Wallet Address string: ");
            scanf("%64s", addr);
            printf("Enter opening AHT Token balance deposit value: ");
            scanf("%lf", &bal);
            if (account_register(addr, bal)) {
                printf("Successfully minted profile record for address point.\n");
            } else {
                printf("Registration Failed: Profile identity exists or system capacity full.\n");
            }
        }
        else if (strcmp(command, "enroll_policy") == 0) {
            char m_id[ADDR_LEN], p_id[HASH_LEN], plan[32];
            printf("Enter Member Address: "); scanf("%64s", m_id);
            printf("Enter new Policy ID hash tracker: "); scanf("%64s", p_id);
            printf("Select Coverage Tier (STUDENT/STAFF): "); scanf("%31s", plan);
            
            if (policy_enroll(m_id, p_id, plan)) {
                printf("Policy established. Status explicitly initialized to ACTIVE for 365 days.\n");
            } else {
                printf("Enrollment failed. ID clash registered.\n");
            }
        }
        else if (strcmp(command, "pay_premium") == 0) {
            char m_addr[ADDR_LEN]; double amt, fee;
            printf("Enter Member Address: "); scanf("%64s", m_addr);
            printf("Enter Premium Cost Amount (AHT): "); scanf("%lf", &amt);
            printf("Enter prioritized Miner gas processing fee: "); scanf("%lf", &fee);
            
            insurance_pay_premium(m_addr, amt, fee);
        }
        else if (strcmp(command, "settle_claim") == 0) {
            char c_id[HASH_LEN], p_addr[ADDR_LEN]; double amt, fee;
            printf("Enter Claim ID string: "); scanf("%64s", c_id);
            printf("Enter Healthcare Provider Destination Wallet: "); scanf("%64s", p_addr);
            printf("Enter claim settlement invoice aggregate total: "); scanf("%lf", &amt);
            printf("Enter processing priority gas transaction fee: "); scanf("%lf", &fee);
            
            insurance_settle_claim(c_id, p_addr, amt, fee);
        }
        else if (strcmp(command, "mempool_view") == 0) {
            mempool_display();
        }
        else if (strcmp(command, "mine_solo") == 0) {
            char miner[ADDR_LEN];
            printf("Enter target Miner Address to receive Block coinbase incentives: ");
            scanf("%64s", miner);
            mine_solo(miner);
        }
        else if (strcmp(command, "mine_pool") == 0) {
            // Setup a mock cluster of two cooperative node addresses for the demo
            char pool_miners[2][65] = {"ALU_MINING_NODE_ALPHA", "ALU_MINING_NODE_BETA"};
            // Proportional hashing contribution split ratio allocation: 60% and 40%
            double split_shares[2] = {0.60, 0.40}; 
            
            mine_pool(2, pool_miners, split_shares);
        }
        else if (strcmp(command, "tamper_chain") == 0) {
            unsigned int b_id;
            printf("Enter Block ID index target to maliciously manipulate: ");
            scanf("%u", &b_id);
            tamper_force_corrupt(b_id);
        }
        else if (strcmp(command, "fraud_view") == 0) {
            view_fraud_flags();
        }
        else if (strcmp(command, "blockchain_view") == 0) {
            printf("\n--- CORE BLOCKCHAIN HEIGHT COUNTER: %d BLOCKS ---\n", chain_height);
            for(int c=0; c < chain_height; c++) {
                printf("Block #%d | Nonce Value: %u | Tx Count: %u | PrevHash: %s...\n",
                       c, get_blockchain_array_ptr()[c].nonce, 
                       get_blockchain_array_ptr()[c].transaction_count,
                       get_blockchain_array_ptr()[c].previous_hash);
            }
        }
        else if (strcmp(command, "blockchain_verify") == 0) {
            blockchain_verify();
        }
        else if (strcmp(command, "chain_save") == 0) {
            persistence_save_state();
        }
        else {
            printf("Command unrecognized. Type 'help' to review structural operational layouts.\n");
        }
    }
    return 0;
}
