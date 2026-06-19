// models/utxo.h
#ifndef UTXO_H
#define UTXO_H

#include "../core/structures.h"

#define MAX_UTXO_POOL 500

// Add a newly generated unspent output to the global state pool
int utxo_create(const char *tx_id, unsigned int output_index, const char *owner, double amount);

// Search and mark a specific UTXO as spent; returns 0 if already spent or missing (Double-spend prevention)
int utxo_consume(const char *tx_id, unsigned int output_index);

// Calculate total spendable balance for an address by summing its unspent outputs
double utxo_get_balance(const char *owner);

// Validate if a transaction input references a valid, unspent output
int utxo_validate_input(const char *tx_id, unsigned int output_index);

// View all current unspent transaction outputs in the ledger system
void utxo_display_pool();

#endif // UTXO_H
