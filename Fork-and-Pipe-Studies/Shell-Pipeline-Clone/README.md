# Pipeline Clone


# About
This is assignment four for WSU-V CS 360. The purpose of this program is to
> Write a program (in C) called connect.c targeted at the Linux platform that performs like a shell pipeline.

The requirements of this program is that it acts like a pipe (|) that accepts at least two commands (<cmd1> | <cmd2>). Students are not allow to use system() or popen and concentrate on forking and exec'ing.

The following files were provide prior to starting the assignment:
- Fork function template

### Features

- Deals with fork()
- Allow the child and parents to commincate via dup2
- Able to read all shell commands that's in the linux's /bin/

### How to use
Run the following commands
```

> make

> ./connect <cmd1> : <cmd2>

```

Where ./ represent the location of the compiled program.

Notice: we are tasked with using : in replacement of |

#### Example
```

>make

>./connect ls -l : sort

Which is the same as executing
>ls -l | sort

```