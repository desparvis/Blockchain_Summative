// insurance/claims.h
#ifndef CLAIMS_H
#define CLAIMS_H

#include "../core/structures.h"

// Handle a premium payment transaction, auto-triggering the 5% reinsurance split
int insurance_pay_premium(const char *member_addr, double premium_amount, double fee);

// Process a claim settlement. If > 1000 AHT, automatically splits logic across pools
int insurance_settle_claim(const char *claim_id, const char *provider_addr, double total_amount, double fee);

#endif // CLAIMS_H
