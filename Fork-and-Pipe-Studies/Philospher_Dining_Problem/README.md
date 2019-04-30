# Philospher_Dining_Problem
Simulate the famous dining philospher challenge using Fork and semaphors.

For threads and mutex version, go to C-Coding\Threads\Dining_Philospher_Problem

# About
This program attempts to 'solve' the famous dining philosopher problem using c while avoiding deadlock. 

### Features

- Fork (to generate the philosphers)
- semaphors (to avoid deadlock)
- error catching
### How to use
Run the following commands
```
make run # Compile and execute automatically
make clean # Delete all tmp file generated.
make all # Compile all files but no execution

```


#### Example
```

> make run
make all
make[1]: Entering directory '*/Philospher_Dining_Problem'
cc -o philo philo.c -lm
make[1]: Leaving directory '*/Philospher_Dining_Problem'
./philo
Philospher 0 is thinking for 15 seconds (0)
Philospher 1 is thinking for 3 seconds (0)
Philospher 2 is thinking for 9 seconds (0)
Philospher 3 is thinking for 20 seconds (0)
Philospher 4 is thinking for 11 seconds (0)
Philospher 1 is eating for 10 seconds (0)
                ...
```