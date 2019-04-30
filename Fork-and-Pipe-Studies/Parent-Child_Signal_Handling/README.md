# Assignment 5: singal()


# About
This is assignment five for WSU-V CS 360. The purpose of this program is to
> Have the program spawn a child process with a pipe shared between the parent and child. The parent will write to the pipe and the child will read from it. There are no required command line arguments

....

The following files were provide prior to starting the assignment:
- Fork function template

### Features

- Deals with fork()
- Allow the child and parents to commincate via pipe

### How to use
Run the following commands
```

> make run

>(Ctrl+c)

[Optional: delay time (in whole +integers)] <message>

To exit: enter
>exit

as a message with no timer.
```


#### Example
```

>make run

>(Ctrl+c)

>2 hello there

>exit

```