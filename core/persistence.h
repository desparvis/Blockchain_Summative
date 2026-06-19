// core/persistence.h
#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#define STORAGE_FILE "alu_blockchain_state.bin"

// Save the complete state of the ledger, models, policies, and structures to a binary file
int persistence_save_state();

// Load, reconstruct, and verify the entire state back from disk into live memory arrays
int persistence_load_state();

#endif // PERSISTENCE_H
