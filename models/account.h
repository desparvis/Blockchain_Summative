// models/account.h
#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "../core/structures.h"

#define MAX_ACCOUNTS 100

// Initialize the 5 core system wallets with starting balances
void account_init_system_wallets();

// Register a completely new member wallet address
int account_register(const char *address, double initial_balance);

// Retrieve an account's current balance
double account_get_balance(const char *address);

// Retrieve an account's current internal nonce
unsigned int account_get_nonce(const char *address);

// Validate if a transaction's sender_nonce perfectly matches (account.nonce + 1)
int account_validate_nonce(const char *address, unsigned int tx_nonce);

// Apply balance updates and increment the account nonce (Only called on MINING confirmation)
int account_apply_transaction(const char *sender, const char *receiver, double amount, unsigned int tx_nonce);

// Debug/Audit tool to print out all accounts and their statuses
void account_display_all();

#endif // ACCOUNT_H
