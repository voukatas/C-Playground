# CCache

A simple single-threaded event-driven program that wannabe a TTL cache

## Features
- Event-driven, single-threaded
- Uses a text-based protocol
- For resource management, it uses a Time-To-Live (TTL) value

## Configuration

The cache is configured using the `config.h` file. The following are the current default configurations:

- **Maximum Value Size**: Configured to support almost `1024 * 1024` bytes as a value. The actual size is slightly reduced to accommodate the `SET` keyword and the bytes for the key.
- **Port**: The server listens on port `8080`.
- **Maximum Epoll Events**: The maximum number of events that `epoll` can retrieve is set to `64`.
- **Cleanup Interval**: The cleanup mechanism for expired keys kicks in every `60` seconds.
- **Initial Hashtable Size**: The starting size of the internal core hashtable is set to `1000`.

## How to run
```bash
make
# For production
./ccache_prod
# With debug symbols
./ccache_dbg
# To check for memory leaks
valgrind --leak-check=full --track-origins=yes -s ./ccache_dbg
```

## How to run tests
```bash
# check for memory issues
valgrind --leak-check=full --track-origins=yes -s ./test_app

# All in one
make clean && make && valgrind --leak-check=full --track-origins=yes -s ./test_app && valgrind --tool=helgrind ./test_app
make clean && make && valgrind --leak-check=full --track-origins=yes -s ./test_app && valgrind --tool=helgrind ./ccache_dbg
```
## Check the Code Coverage
```bash
#Enable code coverage flags in Makefile
make test_app
./test_app
# check quickly the coverage
#gcov your_file (eg. src/server.c)
cd test_obj/
lcov --capture --directory . --output-file coverage.info && genhtml coverage.info --output-directory out
# Open index.html

# Cleanup
make clean

```
## Manual check
```bash
# 30 seconds TTL
echo -ne "SET key_test value_test 30\r\n" | nc localhost 8080
```

## To-Do
- Either remove the linked list that keeps track of the connections (since it adds management complexity) or implement a periodic validation of the connections
- Add support for more cache eviction policies (e.g., LRU, LFU)
- Add more unit tests to increase code coverage and stability
