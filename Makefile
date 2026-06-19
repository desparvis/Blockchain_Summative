# Makefile
CC = gcc
CFLAGS = -Wall -g
LIBS = -lssl -lcrypto

# Track targets across modular subdirectory environments
# FIXED: Added insurance/claims.c to the list below!
SRCS = main.c core/blockchain.c core/cryptography.c core/persistence.c models/account.c models/utxo.c insurance/policy.c insurance/claims.c
OBJS = $(SRCS:.c=.o)
TARGET = alu_blockchain_system

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) alu_blockchain_state.bin
