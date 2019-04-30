# Linux Directory Traversal

# About
This is assignment Three for WSU-V CS 360. The purpose of this program is to
> Write a program (in C) called readable.c targeted at the Linux platform that takes one or zero command line arguments.
Argument, if present, is a pathname (relative or absolute). If no argument is present, the pathname of the present working
directory is assumed (getcwd).

The requirements of this program is that it prints out all the files, folders, and subfolders/files in a given directory/current work directory. It must print out the path of the files and folders and only print them if the user has READ permission to the folder/file (where this is different from 'ls' command on linux). This program MUST implement a recursive function

The following files were provide prior to starting the assignment:
- n/a

### Features

- Uses recursion function
- Error catching with errno
- Display files and folders that the user have read permission to.

### How to use
Run the following commands
```

> make

> ./readable <Path>

```

Where ./ represent the location of the compiled program.

Warning: only pass in one argument (1 word) that you want to look up
#### Example
```

>make

>./readable ~

```