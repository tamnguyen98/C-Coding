# About
This is assignment TWO for WSU-V CS 360. The purpose of this program is to
> Overall: Write a program (in C) targeted at the Linux platform which checks whether a specified word, passed as an argument to the executable, exists in a dictionary or not. The specified dictionary is webster and on the website.

The wordLen and file path must be set according in ok.c's define values. Wordlen is how many character are per line in the dictionary. All lines in the dictionary must be x long (short words needs to be padded with space).

The following files were provide prior to starting the assignment:
- tiny
- webster

### Features

- Access file using open(), lseek(), and read();
- Error catching with errno
- O(log n) complexity (Binary search).

### How to use
Navigate to the the main directory /Dict-Lookup/ and run the following commands
```> make
> ./ok <word>```

Warning: only pass in one argument (1 word) that you want to look up
#### Example
```>make
>./ok dog```
