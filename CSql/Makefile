CC = gcc -std=gnu11
# Flags to check for race conditions
# You might need this in order to run properly
# sudo sysctl vm.mmap_rnd_bits=28
#CFLAGS = -Iunity -Iinclude -Wall -Wextra -Werror -g -pthread -fsanitize=thread -O0
#CFLAGS = -Iunity -Iinclude -Wall -Wextra -Werror -g -pthread -fprofile-arcs -ftest-coverage -O0 
CFLAGS = -Iunity -Iinclude -Wall -Wextra -Werror -g -O0 
OPTIMIZED_FLAGS = -Iinclude -Wall -Wextra -Werror -O2 -s -pthread -DNDEBUG

UNITY_SRC = unity/unity.c

SRC_FILES = $(wildcard src/*.c)
TESTS = test_statement test_meta_command
TARGET = csql

$(TARGET): $(SRC_FILES)
	$(CC) $(CFLAGS) $^ -o $(TARGET)

test_statement: tests/test_statement.c src/statement.c $(UNITY_SRC)
	$(CC) $(CFLAGS) -DTESTING $^ -o $@

test_meta_command: tests/test_meta_command.c src/meta_command.c $(UNITY_SRC)
	$(CC) $(CFLAGS) -DTESTING $^ -o $@

# Run all tests
.PHONY: test
test: $(TESTS)
	./test_statement
	./test_meta_command

.PHONY: clean
clean:
	rm -f $(TARGET) $(TESTS)

