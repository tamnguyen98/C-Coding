# Final project

## Description
This is an assignment for WSU CS 360 where the goal of it is (in a way) like terminal based FTP application.

Client's available commands are:
```
1. exit: terminate the client program after instructing the server’s child process to do the same.
2. cd <pathname>: change the current working directory of the client process to <pathname>. It is an error if
<pathname> does not exist, or is not a readable directory.
3. rcd <pathname>: change the current working directory of the server child process to ¡pathname¿. It is an error
if ¡pathname¿ does not exist, or is not a readable directory.
4. ls: execute the “ls –l” command locally and display the output to standard output, 20 lines at a time, waiting
for a space character on standard input before displaying the next 20 lines. See “rls” and “show”.
5. rls: execute the “ls –l” command on the remote server and display the output to the client’s standard output,
20 lines at a time in the same manner as “ls” above.
6. get <pathname>: retrieve <pathname> from the server and store it locally in the client’s current working
directory using the last component of <pathname> as the file name. It is an error if <pathname> does not exist,
or is anything other than a regular file, or is not readable. It is also an error if the file cannot be opened/created
in the client’s current working directory.
7. show <pathname>: retrieve the contents of the indicated remote <pathname> and write it to the client’s standard
output, 20 lines at a time. Wait for the user to type a space character before displaying the next 20 lines.
8. put <pathname>: transmit the contents of the local file <pathname> to the server and store the contents in the
server process’ current working directory using the last component of <pathname> as the file name. It is an
error if <pathname> does not exist locally or is anything other than a regular file. It is also an error if the file
cannot be opened/created in the server’s child process’ current working directory.
```

### Features

- Sockets IPV4 TCP connections
- Forks
- Low level read/write from between connections

### How to use
Run the following commands
```
make run # Compile and execute automatically
make clean # Delete all tmp file generated.
make all # Compile all files but no execution
make client # compile client code to executable
make server # Same but for server
```
Have the host run ther server executable
```
./server [-d]
```
Note: you can toggle debug mode by adding -d without the brackets

Then have the client connect to the server with it's ip
```
./client [-d] localhost
```

#### Example
##### Server
```

> make server
>./server -d
*sits and wait for connection*
Debug> Root: New client connection detected.
Debug> Child created with PID: 89
Child 89: 127.0.0.1 connected
```
##### Client
```

> make client
>./client -d localhost
MFTP> *waits for user input*
```
