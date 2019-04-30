# Threaded quicksort



# About
This program is a (successful) attempt at implementing threads to quicksort. The goal of this program is implementing threads so that the result is faster (assuming no overhead) than no threads and qsort.

NOTICE: the Quicksort algorithm was provided in advance, I was tasked with implementing the thread features and writing the main.c

### Features

- Threads
- Mutex (to avoid deadlock)
- Error catching
- Qsort
### How to use
Run the following commands
```
make run # Compile and execute automatically
make all # Compile all files but no execution

```


#### Example
```

> make run
./a8 Data\ Test/poem.txt
qsort threads:
2249700 nano seconds
0 threads:
1966700 nano seconds
10 threads:
1735900 nano seconds
                ...
```