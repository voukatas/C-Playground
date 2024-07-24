# HashTable

A simple HashTable implementation where both the key and value are strings.

# How to run
```bash
make
# or you can manually run this
gcc hashtable.c -g -o hs && ./hs
valgrind --leak-check=full --track-origins=yes -s ./hs
```

# How to run tests
```bash
valgrind --leak-check=full --track-origins=yes -s ./test_app
```
