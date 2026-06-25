# Blockchain-Based Health Insurance Management System for ALU

This is a blockchain system that is programmed in C to help decentralize, audit, and automate the health insurance system at ALU. This program aims to replace traditional centralized insurance data systems with immutable cryptographic ledgers, automated multi-route token transfers, and risk management pools.

This is built specifically for the African Leadership University (ALU), the platform introduces **ALU Health Tokens (AHT)** to manage member coverage lifecycles, split-pool high-value reinsurance claims, and protect the network from malicious attacks and fraud.

---

## System Architecture & Code Design

The codebase enforces strict separation of concerns, partitioning the blockchain engine into isolated, translation-unit submodules:

ALU_Blockchain_Project/
├── main.c                      # Core Shell Orchestrator & CLI Loop
├── Makefile                    # Multi-Target Compiling Pipeline
├── core/
│   ├── blockchain.c / .h       # Block processing, Mempool Sorting, PoW Consensus
│   └── cryptography.c / .h     # SHA256 hashing & ECDSA Verification Wrappers
├── models/
│   ├── account.c / .h          # Account-Based Ledger State, Balance Tracking, Replay Protection
│   └── utxo.c / .h             # Spent/Unspent Input Historical Tracking
└── insurance/
    ├── policy.c / .h           # 365-Day Plan Enrollment & Lifecycle Status Management
    └── claims.c / .h           # Premium Splitting, Claims Settlement & Fraud Rules
Module Breakdown
core/: Implements raw block structures, the fee-prioritized mempool transaction engine, Proof of Work mining algorithms, and deep parent-pointer ledger verification.

models/: Manages account registration, global state balance adjustments, and sender_nonce tracking for transaction execution verification.

insurance/: Houses the system's operational business logic parameter loops, policy deadline timers, dual-pool reinsurance calculations, and fraud quarantine sandboxes.

 Key Features
Decentralized Member & Policy Identity: Maps a member's cryptographic wallet identity directly to localized institutional coverage profiles (STUDENT or STAFF tiers) with explicit 365-day expiry timestamps.

Automated Premium Token Routing: When a premium is deposited via pay_premium, the engine automatically spawns a secondary transaction routing a 5% safety cut directly into a dedicated Reinsurance fund wallet.

Dual-Pool Split Billing Payouts: Claims exceeding 1,000 AHT automatically trigger a split settlement mechanism, liquidating up to 1,000 AHT from the primary insurance pool and drafting the overflow directly from the Reinsurance buffer.

Top-Priority Fraud Quarantine Heuristics: Evaluates claim amounts at the entry gateway. Claims exceeding 50,000 AHT bypass standard execution to avoid mempool pollution and are quarantined inside an isolated administrative queue for manual inspection.

Fee-Prioritized Mempool Engine: Implements a sorting algorithm that organizes pending transactions into a priority queue based on gas fees, ensuring high-incentive data packets are mined first.

Consensus Flexibility: Supports mine_solo utilizing single-node Proof of Work (PoW) computation, alongside mine_pool simulating cooperative clusters splitting block rewards proportionally.

Deep Cryptographic Auditing: Traverses the historical ledger recursively to confirm block hash links. Malicious data changes introduced via tamper_chain are flagged instantly by the system verification loop.

Binary State Serialization: Automatically marshals live RAM tracking arrays down to disk into a packed binary layout (alu_blockchain_state.bin) to preserve states across runtimes.

 Compilation & Installation
The project uses a standard Linux compilation toolchain and requires OpenSSL development libraries for cryptographic primitives.

Prerequisites (Ubuntu/Debian)
Bash
sudo apt-get update
sudo apt-get install build-essential libssl-dev
Compiling the Ledger Binaries
To clean temporary compilation binary object state caches and compile the application executable, run:

Bash
make clean
make
