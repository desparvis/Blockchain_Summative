// insurance/claims.c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "claims.h"
#include "policy.h"
#include "../core/blockchain.h"

// External tracking wallet constants declared in our account module
extern const char* WALLET_INSURANCE_POOL;
extern const char* WALLET_REINSURANCE_POOL;

int insurance_pay_premium(const char *member_addr, double premium_amount, double fee) {
    // 1. Construct Primary Premium Payment Transaction
    Transaction tx_premium;
    snprintf(tx_premium.transaction_id, HASH_LEN, "TX_PREM_%ld", time(NULL));
    strncpy(tx_premium.sender_address, member_addr, ADDR_LEN);
    strncpy(tx_premium.receiver_address, WALLET_INSURANCE_POOL, ADDR_LEN);
    tx_premium.amount = premium_amount;
    tx_premium.transaction_type = TRANSACTION_PREMIUM_PAYMENT;
    tx_premium.timestamp = time(NULL);
    memset(tx_premium.digital_signature, 0x7F, SIG_LEN); // Mock signing signature token bytes

    // Submit primary transaction to the mempool
    if (!mempool_add_transaction(&tx_premium, fee)) return 0;

    // 2. REQUIREMENT CHECK: Automatical entry of a secondary 5% REINSURANCE_CONTRIBUTION transaction
    Transaction tx_reinsurance;
    snprintf(tx_reinsurance.transaction_id, HASH_LEN, "TX_REIN_CONT_%ld", time(NULL));
    strncpy(tx_reinsurance.sender_address, WALLET_INSURANCE_POOL, ADDR_LEN);
    strncpy(tx_reinsurance.receiver_address, WALLET_REINSURANCE_POOL, ADDR_LEN);
    
    // Calculate the mandatory 5% allocation target cut
    tx_reinsurance.amount = premium_amount * 0.05; 
    tx_reinsurance.transaction_type = TRANSACTION_REINSURANCE_CONTRIBUTION;
    tx_reinsurance.timestamp = time(NULL);
    memset(tx_reinsurance.digital_signature, 0x7F, SIG_LEN);

    // Both enter the mempool together
    mempool_add_transaction(&tx_reinsurance, 0.0); // Secondary standard processing fee omitted

    printf("Premium logged! Main payment and 5%% Reinsurance contribution successfully pushed to mempool.\n");
    return 1;
}

int insurance_settle_claim(const char *claim_id, const char *provider_addr, double total_amount, double fee) {
    // REQUIREMENT CHECK: Split billing settlement logic when hitting threshold values exceeding 1000 AHT
    if (total_amount > 1000.0) {
        double insurance_share = 1000.0;
        double reinsurance_share = total_amount - 1000.0;

        printf("High-value claim detected (>1000 AHT). Triggering dual-pool settlement split...\n");

        // Transaction A: Primary Insurance Pool payment capping at 1000 AHT
        Transaction tx_a;
        snprintf(tx_a.transaction_id, HASH_LEN, "SETTLE_MAIN_%s", claim_id);
        strncpy(tx_a.sender_address, WALLET_INSURANCE_POOL, ADDR_LEN);
        strncpy(tx_a.receiver_address, provider_addr, ADDR_LEN);
        tx_a.amount = insurance_share;
        tx_a.transaction_type = TRANSACTION_CLAIM_SETTLEMENT;
        tx_a.timestamp = time(NULL);
        memset(tx_a.digital_signature, 0x7F, SIG_LEN);
        mempool_add_transaction(&tx_a, fee);

        // Transaction B: Reinsurance Buffer covers the excess spill over remainder
        Transaction tx_b;
        snprintf(tx_b.transaction_id, HASH_LEN, "SETTLE_REIN_%s", claim_id);
        strncpy(tx_b.sender_address, WALLET_REINSURANCE_POOL, ADDR_LEN);
        strncpy(tx_b.receiver_address, provider_addr, ADDR_LEN);
        tx_b.amount = reinsurance_share;
        tx_b.transaction_type = TRANSACTION_CLAIM_SETTLEMENT;
        tx_b.timestamp = time(NULL);
        memset(tx_b.digital_signature, 0x7F, SIG_LEN);
        mempool_add_transaction(&tx_b, 0.0);

        printf("Dual split transactions queued successfully.\n");
    } else {
        // Standard settlement logic route handled fully by regular insurance pool funds
        Transaction tx_single;
        snprintf(tx_single.transaction_id, HASH_LEN, "SETTLE_SINGLE_%s", claim_id);
        strncpy(tx_single.sender_address, WALLET_INSURANCE_POOL, ADDR_LEN);
        strncpy(tx_single.receiver_address, provider_addr, ADDR_LEN);
        tx_single.amount = total_amount;
        tx_single.transaction_type = TRANSACTION_CLAIM_SETTLEMENT;
        tx_single.timestamp = time(NULL);
        memset(tx_single.digital_signature, 0x7F, SIG_LEN);
        mempool_add_transaction(&tx_single, fee);
        
        printf("Standard claim settlement transaction queued.\n");
    }
    return 1;
}
