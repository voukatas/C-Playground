# HashTable

A simple HashTable implementation where the key is a string and the value can be anything

# How to run
```bash
make
# or you can manually run this
gcc hashtable.c -g -o hs && ./hs
valgrind --leak-check=full --track-origins=yes -s ./hs
```

# How to run tests
```bash
# check for memory issues
valgrind --leak-check=full --track-origins=yes -s ./test_app
# check for data races
valgrind --tool=helgrind ./test_app

# All in one
make clean && make && valgrind --leak-check=full --track-origins=yes -s ./test_app && valgrind --tool=helgrind ./test_app
```
