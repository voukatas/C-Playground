# CCache

A simple single-threaded event-driven program that wannabe a cache

# How to run
```bash
make
# For production
./ccache_prod
# With debug symbols
./ccache_dbg
# To check for memory leaks
valgrind --leak-check=full --track-origins=yes -s ./ccache_dbg
```

# How to run tests
```bash
# check for memory issues
valgrind --leak-check=full --track-origins=yes -s ./test_app

# All in one
make clean && make && valgrind --leak-check=full --track-origins=yes -s ./test_app && valgrind --tool=helgrind ./test_app
make clean && make && valgrind --leak-check=full --track-origins=yes -s ./test_app && valgrind --tool=helgrind ./ccache_dbg
```
## To check the code coverage
```bash
- Enable code coverage flags in Makefile
make test_app
./test_app
# check quickly the coverage
#gcov your_file (eg. src/server.c)
cd test_obj/
lcov --capture --directory . --output-file coverage.info && genhtml coverage.info --output-directory out
- open index.html

# cleanup
make clean

```
