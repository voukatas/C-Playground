CC = gcc
CFLAGS = -Iunity -Isrc -Iinclude -Wall -Wextra -Werror -g
OPTIMIZED_FLAGS = -O2 -s
SRC = $(wildcard src/*.c)
TEST_SRC = $(wildcard test/*.c) $(wildcard unity/*.c)

APP_SRC = $(filter-out src/test_app.c, $(SRC))
APP_OBJ = $(APP_SRC:.c=.o)
TEST_OBJ = $(TEST_SRC:.c=.o) $(filter-out src/main.o, $(SRC:.c=.o))

APP_TARGET = hashtable
TEST_TARGET = test_app
OPTIMIZED_TARGET = hashtable_optimized

all: $(APP_TARGET) $(TEST_TARGET)

$(APP_TARGET): $(APP_OBJ)
	@$(CC) -o $@ $^

$(TEST_TARGET): $(TEST_OBJ)
	@$(CC) -o $@ $^

$(OPTIMIZED_TARGET): $(APP_OBJ)
	@$(CC) $(OPTIMIZED_FLAGS) -o $@ $^

%.o: %.c
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(APP_OBJ) $(TEST_OBJ) $(APP_TARGET) $(TEST_TARGET) $(OPTIMIZED_TARGET)

